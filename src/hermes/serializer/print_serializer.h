#ifndef HERMES_SERIALIZER_PRINT_SERIALIZER_H_
#define HERMES_SERIALIZER_PRINT_SERIALIZER_H_

#include <memory>
#include "hermes/serializer.h"

namespace hermes {
  class Message;

  class PrintSerializer : public Serializer {

   public:
    PrintSerializer();

    void Initialize(const Message *msg) override;
    void Serialize(const Message *msg) override;
    void Finalize(const Message *msg) override;
    void Send(const Message *msg) override;

   private:
    void WriteHeader(const Message *msg);

   private:
    class InitVisitor;
    class SerializeVisitor;

    class Impl;
    std::unique_ptr<Impl> m_impl;
    std::unique_ptr<SerializeVisitor> m_serializeVisitor;
  };
}  // namespace hermes

#endif  // HERMES_SERIALIZER_PRINT_SERIALIZER_H_
