#ifndef HERMES_FIELD_H_
#define HERMES_FIELD_H_

#include <string>
#include <system_error>

#include "hermes/visitor.h"
#include "hermes/field_base.h"

namespace hermes {
  /*
   * Implementation class for Fields. It is templated with the type of data it points to.
   */
  template<class T>
  class Field : public FieldBase {

   private:
    std::function<T()> m_getData;

   public:
    Field(std::string name,
          std::string unit,
          std::string description,
          T *data,
          unsigned int precision,
          unsigned int log_level)
        : m_getData([data]() { return *data; }), FieldBase(name, unit, description, precision, log_level) {
      if (log_level > HERMES_MAX_LOG_LEVEL) {
        throw std::runtime_error("Maximum log level allowable in hermes is "
                                 + std::to_string(HERMES_MAX_LOG_LEVEL)
                                 + ". Requested is "
                                 + std::to_string(log_level));
      }
    }

    Field(std::string name,
          std::string unit,
          std::string description,
          std::function<T()> data,
          unsigned int precision,
          unsigned int log_level)
        : m_getData([data]() { return data(); }), FieldBase(name, unit, description, precision, log_level) {
      if (log_level > HERMES_MAX_LOG_LEVEL) {
        throw std::runtime_error("Maximum log level allowable in hermes is "
                                 + std::to_string(HERMES_MAX_LOG_LEVEL)
                                 + ". Requested is "
                                 + std::to_string(log_level));
      }
    }

    void Accept(Visitor &visitor) const override {
      visitor.visit(this);
    }

    const T GetData() const {
      return m_getData();
    }

  };
}  // namespace hermes

#endif  // HERMES_FIELD_H_
