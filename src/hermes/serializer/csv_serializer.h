#ifndef HERMES_SERIALIZER_CSV_SERIALIZER_H_
#define HERMES_SERIALIZER_CSV_SERIALIZER_H_

#include <memory>
#include <string>
#include "hermes/serializer.h"

namespace hermes {

    class CSVSerializer : public Serializer {

     private:
      class InitVisitor;
      class UnitLineVisitor;
      class SerializeVisitor;

      std::string GetCSVLine();

     public:

      explicit CSVSerializer(std::string CSVFile);
      void SetDelimiter(const std::string &delimiter);
      void Initialize(const Message *msg) override;
      void Serialize(const Message *msg) override;
      void Finalize(const Message *msg) override;
      void Send(const Message *msg) override;

     private:
      class Impl;
      std::unique_ptr<Impl> m_impl;

      std::unique_ptr<SerializeVisitor> m_serializeVisitor;

    };
}  // namespace hermes

#endif  // HERMES_SERIALIZER_CSV_SERIALIZER_H_
