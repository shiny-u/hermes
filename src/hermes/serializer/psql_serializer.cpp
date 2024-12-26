#include "hermes/serializer/psql_serializer.h"

#include <math.h>

#include <spdlog/spdlog.h>
#include <pqxx/pqxx>

#include "hermes/serialization_visitor.h"
#include "hermes/message.h"
#include "hermes/chrono.h"

namespace hermes {
  class InitVisitor : public Visitor {
   public:
    explicit InitVisitor(std::string table_name, pqxx::connection& connection) :
        m_table_name(std::move(table_name)),
        m_connection(connection) {}

    void visit(const Field<int> *field) override { CreateColumn(*field, "INTEGER NULL"); }
    void visit(const Field<float> *field) override { CreateColumn(*field, "FLOAT NULL"); }
    void visit(const Field<double> *field) override { CreateColumn(*field, "DOUBLE PRECISION NULL"); }
    void visit(const Field<bool> *field) override { CreateColumn(*field, "BOOL NULL"); }
    void visit(const Field<std::string> *field) override { CreateColumn(*field, "TEXT NULL"); }
    void visit(const Field<Eigen::Matrix<double, 3, 1>> *field) override {
      throw std::runtime_error{"Can't serialize Eigen::Matrix in a psql database"};
    }
    void visit(const Field<std::vector<double>> *field) override {
      throw std::runtime_error{"Can't serialize std::vector in a psql database"};
    }
    void visit(const Field<std::function<double()>> *field) override { CreateColumn(*field, "DOUBLE PRECISION NULL"); }
    void visit(const Field<std::chrono::time_point<std::chrono::system_clock>> *field) override {
      CreateColumn(*field, "TIMESTAMP NULL", true);
    }

   private:
    static std::string ColumnName(const FieldBase& field) {
      auto retval = field.GetName();
      std::transform(retval.begin(), retval.end(), retval.begin(), [](unsigned char c){ return std::tolower(c); });
      return retval;
    }

    void CreateColumn(const FieldBase& field, const std::string& column_type, bool is_time_point_type = false) {
      pqxx::work tx(m_connection);
      auto column_name = ColumnName(field);
      if (column_name == "time" && !is_time_point_type) {
        throw std::runtime_error{"Hermes PSQL integration only support std::chrono::time_point<std::chrono::system_clock> as base time for column 'time'"};
      }

      auto query = fmt::format("ALTER TABLE {} ADD \"{}\" {}", m_table_name, column_name, column_type);
      spdlog::trace("[hermes][PSQLSerializer][InitVisitor] {}", query);
      auto add_description_query = fmt::format(R"(COMMENT ON COLUMN {}."{}" is 'UNIT="{}";DESCRIPTION="{}"')",
                                               m_table_name, column_name, field.GetUnit(), field.GetDescription());
      spdlog::trace("[hermes][PSQLSerializer][InitVisitor] {}", add_description_query);
      try {
        tx.exec0(query);
        tx.exec0(add_description_query);
        tx.commit();
        spdlog::debug("[hermes][PSQLSerializer][InitVisitor] Table '{}' Column '{}' created", m_table_name, column_name);
      } catch (const pqxx::sql_error& err) {
        if (err.sqlstate() != "42701") {
          throw err;
        }
        spdlog::trace("[hermes][PSQLSerializer][InitVisitor] Table '{}' Column '{}' already exists", m_table_name, column_name);
      }
    }

   private:
    std::string m_table_name;
    pqxx::connection& m_connection;
  };

  class SerializeVisitor : public Visitor {
   public:
    explicit SerializeVisitor(std::map<std::string, std::string>& columns) : m_columns(columns) {}
    void visit(const Field<int> *field) override {
      AddColumn(*field, std::to_string(field->GetData()));
    }
    void visit(const Field<float> *field) override {
      float data = field->GetData();
      AddColumn(*field, (isnan(data) ? c_nan_str : std::to_string(data)));
    }
    void visit(const Field<double> *field) override {
      double data = field->GetData();
      AddColumn(*field, (isnan(data) ? c_nan_str : std::to_string(data)));
    }
    void visit(const Field<bool> *field) override {
      AddColumn(*field, field->GetData() ? "TRUE" : "FALSE");
    }
    void visit(const Field<std::string> *field) override {
      AddColumn(*field, fmt::format("'{}'", field->GetData()));
    }
    void visit(const Field<Eigen::Matrix<double, 3, 1>> *field) override {}
    void visit(const Field<std::vector<double>> *field) override {}
    void visit(const Field<std::function<double()>> *field) override {
      double data = field->GetData()();
      AddColumn(*field, (isnan(data) ? c_nan_str : std::to_string(data)));
    }
    void visit(const Field<std::chrono::time_point<std::chrono::system_clock>> *field) override {
      AddColumn(*field, fmt::format("'{}'", ISO8601(field->GetData())));
    }

   private:
    void AddColumn(const FieldBase& field, std::string value) {
      auto column_name = field.GetName();
      std::transform(column_name.begin(), column_name.end(), column_name.begin(), [](unsigned char c){ return std::tolower(c); });
      m_columns.emplace(std::move(column_name), std::move(value));
    }

   private:
    const std::string c_nan_str = "'NaN'";
    std::map<std::string, std::string>& m_columns;
  };

  class PSQLSerializer::Transaction {
   public:
    explicit Transaction(pqxx::connection& connection) : tx(connection) {}
    pqxx::work tx;
  };

  PSQLSerializer::PSQLSerializer(std::shared_ptr<pqxx::connection> connection) :
    m_connection(std::move(connection)),
    m_transaction(nullptr) {}

  std::string PSQLSerializer::GetTableName(const Message* msg) {
    auto table_name = msg->GetName();
    std::transform(table_name.begin(), table_name.end(), table_name.begin(), [](unsigned char c){ return std::tolower(c); });
    return table_name;
  }

  void PSQLSerializer::Initialize(const Message *msg) {
    auto table_name = GetTableName(msg);

    // Create table
    {
      pqxx::work tx(*m_connection);

      auto create_table_query = fmt::format("CREATE TABLE {0} (time TIMESTAMP NOT NULL);", table_name);
      spdlog::trace("[hermes][PSQLSerializer] {}", create_table_query);
      try {
        tx.exec0(create_table_query);
        tx.commit();  // we can commit right away as we don't care about optimizing the initialization process
        spdlog::debug("[hermes][PSQLSerializer] Table '{}' created", table_name);
      } catch (const pqxx::sql_error& err) {
        if (err.sqlstate() != "42P07") {
          throw err;
        }
        spdlog::debug("[hermes][PSQLSerializer] Table '{}' already exists", table_name);
      }
    }

    // Create columns
    InitVisitor visitor(table_name, *m_connection);
    msg->ApplyVisitor(visitor);

    // Convert table to a hypertable, does nothing if the hypertable already exists. If the table with data already exists but without hypertable, it will take a few minutes to migrate existing data (depending on the size).
    {
      pqxx::work tx(*m_connection);
      auto create_hypertable_query = fmt::format("SELECT create_hypertable('{}', 'time', if_not_exists => TRUE, migrate_data => TRUE);", table_name);
      spdlog::trace("[hermes][PSQLSerializer] {}", create_hypertable_query);
      tx.query<std::string>(create_hypertable_query);
      tx.commit();
    }
  }

  void PSQLSerializer::Serialize(const Message *msg) {
    auto table_name = GetTableName(msg);

    std::map<std::string, std::string> columns;
    SerializeVisitor visitor(columns);
    msg->ApplyVisitor(visitor);

    // Add 'time' as that field could not be found
    if (columns.find("time") == columns.end()) {
      columns["time"] = fmt::format("'{}'", ISO8601(std::chrono::system_clock::now()));
    }

    std::string column_names{};
    std::string column_values{};
    for (auto column : columns) {
      if (!column_names.empty()) {
        column_names += ", ";
        column_values += ", ";
      }
      column_names += fmt::format("\"{}\"", column.first);
      column_values += column.second;
    }

    auto query = fmt::format("INSERT INTO {} ({}) VALUES ({});", table_name, column_names, column_values);
    if (!m_transaction) {
      m_transaction = std::make_unique<PSQLSerializer::Transaction>(*m_connection);
    }
    spdlog::trace("[hermes][PSQLSerializer] {}", query);
    m_transaction->tx.exec0(query);
  }

  void PSQLSerializer::Finalize(const Message *msg) {
    if (!m_transaction) {
      return;
    }
    spdlog::trace("[hermes][PSQLSerializer] Commit transaction");
    m_transaction->tx.commit();
    m_transaction.reset();
  }

  void PSQLSerializer::Send(const Message *msg) {
    if (!m_transaction) {
      spdlog::debug("[hermes][PSQLSerializer] Trying to send an empty transaction");
      return;
    }
    spdlog::trace("[hermes][PSQLSerializer] Commit transaction");
    m_transaction->tx.commit();
    m_transaction.reset();
  }

  PSQLSerializer* Message::AddPSQLSerializer(std::shared_ptr<pqxx::connection> conn) {
    auto* serializer = AddSerializer(new PSQLSerializer(std::move(conn)));
    return dynamic_cast<PSQLSerializer*>(serializer);
  }
}  // namespace hermes
