#ifndef HERMES_SERIALIZER_H_
#define HERMES_SERIALIZER_H_

namespace hermes {
  class Message;

  /*
   * Abstract base class for the serialization of messages. It must use visitor pattern to serialize following its
   * different stages
   */
  class Serializer {
   public:
    virtual ~Serializer() {}

    virtual void Initialize(const Message *msg) = 0;

    virtual void Serialize(const Message *msg) = 0;

    virtual void Finalize(const Message *msg) = 0;

    virtual void Send(const Message *msg) = 0;
  };
}  // namespace hermes
#endif  // HERMES_SERIALIZER_H_