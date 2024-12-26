#ifndef HERMES_FIELD_BASE_H_
#define HERMES_FIELD_BASE_H_

#include <string>

namespace hermes {
  class Visitor;

  /*
   * Pure abstract class for field representation. This class is intended to be used as homogeneous element of a message
   * inside a std::vector<FieldBase> which is enclosed into the Message class.
   */
  class FieldBase {
   public:
    FieldBase(std::string name,
              std::string unit,
              std::string description,
              unsigned int precision,
              unsigned int log_level);

    FieldBase(std::string name,
              std::string description,
              unsigned int precision,
              unsigned int log_level);

    std::string GetName() const;
    std::string GetUnit() const;
    std::string GetDescription() const;
    unsigned int precision() const;
    unsigned int log_level() const;

    virtual void Accept(Visitor &visitor) const = 0;

   protected:
    std::string m_name;
    std::string m_unit;
    std::string m_description;
    unsigned int m_precision;
    unsigned int m_log_level;
  };
}  // namespace hermes

#endif  // HERMES_FIELD_BASE_H_
