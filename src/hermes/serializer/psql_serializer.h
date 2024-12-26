#ifndef HERMES_SERIALIZER_PSQL_SERIALIZER_H_
#define HERMES_SERIALIZER_PSQL_SERIALIZER_H_

#include <memory>
#include "hermes/serializer.h"

namespace pqxx {
  class connection;
}  // namespace pqxx

namespace hermes {
  /*
   * PostgreSQL serializer.
   *
   * Table name is the serialized message name. Column names are the different fields name.
   * NB: the table and column names are converted to lower case
   *
   * Designed to work with timescale (https://www.timescale.com/), the serialized messages need a 'time' field.
   * If none found, the field is added and the time at insertion is used
   */
  class PSQLSerializer : public Serializer {
   public:
    explicit PSQLSerializer(std::shared_ptr<pqxx::connection> connection);

    void Initialize(const Message *msg) override;
    void Serialize(const Message *msg) override;
    void Finalize(const Message *msg) override;
    void Send(const Message *msg) override;

   private:
    static std::string GetTableName(const Message* msg);

   private:
    std::shared_ptr<pqxx::connection> m_connection;
    class Transaction;
    std::unique_ptr<Transaction> m_transaction;
  };
}  // namespace hermes

#endif  // HERMES_SERIALIZER_PSQL_SERIALIZER_H_
