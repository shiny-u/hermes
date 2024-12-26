#include "hermes/serializer/csv_serializer.h"

#include <fstream>

#include <fmt/format.h>

#include "hermes/serialization_visitor.h"
#include "hermes/message.h"
#include "hermes/chrono.h"

namespace hermes {
  class CSVSerializer::Impl {
   public:
    explicit Impl(std::string csvFile) : m_CSVFile(std::move(csvFile)) {}

    std::string m_delimiter = ";";
    fmt::memory_buffer m_buffer;

    std::string m_CSVFile;
    std::ofstream m_file;

    bool c_IsInitialized = false;
  };

  class CSVSerializer::InitVisitor : public SerializationVisitor<CSVSerializer> {

    inline void visit(FieldBase *field) {
      fmt::format_to(std::back_inserter(m_serializer->m_impl->m_buffer), "{}{}",
                     field->GetName(), m_serializer->m_impl->m_delimiter);
    }

   public:
    explicit InitVisitor(CSVSerializer *serializer) :
        SerializationVisitor<CSVSerializer>(serializer) {}

    void visit(const Field<int> *field) override { visit((FieldBase *) field); }

    void visit(const Field<float> *field) override { visit((FieldBase *) field); }

    void visit(const Field<double> *field) override { visit((FieldBase *) field); }

    void visit(const Field<bool> *field) override { visit((FieldBase *) field); }

    void visit(const Field<std::string> *field) override { visit((FieldBase *) field); }

    void visit(const Field<Eigen::Matrix<double, 3, 1>> *field) override {
      auto name = field->GetName();
      fmt::format_to(std::back_inserter(m_serializer->m_impl->m_buffer), "{}{}{}",
                     name + "_0" + m_serializer->m_impl->m_delimiter,
                     name + "_1" + m_serializer->m_impl->m_delimiter,
                     name + "_2" + m_serializer->m_impl->m_delimiter); // FIXME : permettre de fixer des regles d'indicage...
    }

    void visit(const Field<std::vector<double>> *field) override {
      auto n = field->GetData().size();
      auto name = field->GetName();
      for (int i = 0; i < n; i++) {
        fmt::format_to(std::back_inserter(m_serializer->m_impl->m_buffer), "{}_{}{}",
                       name, i, m_serializer->m_impl->m_delimiter);
      }
    }

    void visit(const Field<std::function<double()>> *field) override { visit((FieldBase *) field); }

    void visit(const Field<std::chrono::time_point<std::chrono::system_clock>>* field) override { visit((FieldBase *) field); }

  };

  class CSVSerializer::UnitLineVisitor : public SerializationVisitor<CSVSerializer> {

    inline void visit(FieldBase *field) {
      fmt::format_to(std::back_inserter(m_serializer->m_impl->m_buffer), "{}{}",
                     field->GetUnit(), m_serializer->m_impl->m_delimiter);
    }

   public:
    explicit UnitLineVisitor(CSVSerializer *serializer) :
        SerializationVisitor<CSVSerializer>(serializer) {}

    void visit(const Field<int> *field) override { visit((FieldBase *) field); }

    void visit(const Field<float> *field) override { visit((FieldBase *) field); }

    void visit(const Field<double> *field) override { visit((FieldBase *) field); }

    void visit(const Field<bool> *field) override { visit((FieldBase *) field); }

    void visit(const Field<std::string> *field) override { visit((FieldBase *) field); }

    void visit(const Field<Eigen::Matrix<double, 3, 1>> *field) override {
      std::string unit_ = field->GetUnit() + m_serializer->m_impl->m_delimiter;
      fmt::format_to(std::back_inserter(m_serializer->m_impl->m_buffer), "{}{}{}", unit_, unit_, unit_);
    }

    void visit(const Field<std::vector<double>> *field) override {
      auto n = field->GetData().size();
      auto unit_ = field->GetUnit();
      for (int i = 0; i < n; i++) {
        fmt::format_to(std::back_inserter(m_serializer->m_impl->m_buffer), "{}{}",
                       unit_, m_serializer->m_impl->m_delimiter);
      }
    }

    void visit(const Field<std::function<double()>> *field) override { visit((FieldBase *) field); }

    void visit(const Field<std::chrono::time_point<std::chrono::system_clock>>* field) override {
      visit((FieldBase *) field);
    }
  };

  class CSVSerializer::SerializeVisitor : public SerializationVisitor<CSVSerializer> {

   public:
    explicit SerializeVisitor(CSVSerializer *serializer) :
        SerializationVisitor<CSVSerializer>(serializer) {}

    void visit(const Field<int> *field) override {
      fmt::format_to(std::back_inserter(m_serializer->m_impl->m_buffer), "{:d}{:s}",
                     field->GetData(), m_serializer->m_impl->m_delimiter);
    }

    void visit(const Field<float> *field) override {
      fmt::format_to(std::back_inserter(m_serializer->m_impl->m_buffer), "{:.{}f}{:s}",
                     field->GetData(), field->precision(),
                     m_serializer->m_impl->m_delimiter);
    }

    void visit(const Field<double> *field) override {
      fmt::format_to(std::back_inserter(m_serializer->m_impl->m_buffer), "{:.{}f}{:s}",
                     field->GetData(), field->precision(),
                     m_serializer->m_impl->m_delimiter);
    }

    void visit(const Field<bool> *field) override {
      fmt::format_to(std::back_inserter(m_serializer->m_impl->m_buffer), "{:d}{:s}",
                     field->GetData(), m_serializer->m_impl->m_delimiter);
    }

    void visit(const Field<std::string> *field) override {
      fmt::format_to(std::back_inserter(m_serializer->m_impl->m_buffer), "{:s}{:s}",
                     field->GetData(), m_serializer->m_impl->m_delimiter);
    }

    void visit(const Field<Eigen::Matrix<double, 3, 1>> *field) override {
      auto precision = field->precision();
      auto vector = field->GetData();
      fmt::format_to(std::back_inserter(m_serializer->m_impl->m_buffer), "{:.{}f}{:s}{:.{}f}{:s}{:.{}f}{:s}",
                     vector[0], precision, m_serializer->m_impl->m_delimiter,
                     vector[1], precision, m_serializer->m_impl->m_delimiter,
                     vector[2], precision, m_serializer->m_impl->m_delimiter
      );
    }

    void visit(const Field<std::vector<double>> *field) override {
      auto precision = field->precision();
      auto vector = field->GetData();
      for (const auto &element: vector) {
        fmt::format_to(std::back_inserter(m_serializer->m_impl->m_buffer), "{:.{}f}{}",
                       element, precision, m_serializer->m_impl->m_delimiter);
      }
    }

    void visit(const Field<std::function<double()>> *field) override {
      fmt::format_to(std::back_inserter(m_serializer->m_impl->m_buffer), "{:.{}f}{:s}",
                     (field->GetData())(), field->precision(),
                     m_serializer->m_impl->m_delimiter);
    }

    void visit(const Field<std::chrono::time_point<std::chrono::system_clock>>* field) override {
      fmt::format_to(std::back_inserter(m_serializer->m_impl->m_buffer), "{:s}{:s}",
                     ISO8601(field->GetData()), m_serializer->m_impl->m_delimiter);
    }
  };

  std::string CSVSerializer::GetCSVLine() {
    // Adding the final line break
    return fmt::to_string(m_impl->m_buffer);
  }



  CSVSerializer::CSVSerializer(std::string CSVFile) :
    m_serializeVisitor(std::make_unique<SerializeVisitor>(this)),
    m_impl(std::make_unique<Impl>(std::move(CSVFile))) {}

  void CSVSerializer::SetDelimiter(const std::string &delimiter) { m_impl->m_delimiter = delimiter; }

  void CSVSerializer::Initialize(const Message *msg) {
    // Building the file name based on rule on message

    CSVSerializer::InitVisitor visitor(this);
    msg->ApplyVisitor(visitor);
    fmt::format_to(std::back_inserter(m_impl->m_buffer), "\n");

    CSVSerializer::UnitLineVisitor unitLineVisitor(this);
    msg->ApplyVisitor(unitLineVisitor);

  }

  void CSVSerializer::Serialize(const Message *msg) {
    fmt::format_to(std::back_inserter(m_impl->m_buffer), "\n");
    msg->ApplyVisitor(*m_serializeVisitor);
  }

  void CSVSerializer::Finalize(const Message *msg) {}

  void CSVSerializer::Send(const Message *msg) {
    if (m_impl->c_IsInitialized) {
      m_impl->m_file.open(m_impl->m_CSVFile, std::ios::app);
    } else {
      m_impl->m_file.open(m_impl->m_CSVFile, std::ios::trunc);
      m_impl->c_IsInitialized = true;
    }

    m_impl->m_file << GetCSVLine();

    m_impl->m_file.close();

    m_impl->m_buffer.clear();
  }

  CSVSerializer *Message::AddCSVSerializer(std::string CSVFile) {
    auto *csvSerializer = AddSerializer(new CSVSerializer(std::move(CSVFile)));
    return dynamic_cast<CSVSerializer *>(csvSerializer);
  }

}  // namespace hermes
