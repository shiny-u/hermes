#include <gtest/gtest.h>
#include <iostream>

#include "hermes/hermes.h"

bool g_not_a_serializer_called = false;

namespace hermes {
  class NotASerializer : public Serializer {
   public:
    ~NotASerializer() {
      g_not_a_serializer_called = true;
    }
    void Initialize(const Message *msg) override {};
    void Serialize(const Message *msg) override {};
    void Finalize(const Message *msg) override {};
    void Send(const Message *msg) override {};
  };
}

TEST(Hermes, SerializerDestructor) {
  using namespace hermes;
  {
    // Create a unique pointer of serializer.
    // The destructor of NotASerializer should be called at the end of the
    // scope
    std::unique_ptr<Serializer> serializer(new NotASerializer());
    ASSERT_FALSE(g_not_a_serializer_called);
  }
  ASSERT_TRUE(g_not_a_serializer_called);
}
