#ifndef HERMES_SERIALIZATION_VISITOR_H_
#define HERMES_SERIALIZATION_VISITOR_H_

#include "hermes/visitor.h"

namespace hermes {
  template<class C>
  class SerializationVisitor : public Visitor {
   protected:
    C *m_serializer;

    explicit SerializationVisitor(C *serializer) : m_serializer(serializer) {}
  };
}  // namespace hermes

#endif  // HERMES_SERIALIZATION_VISITOR_H_
