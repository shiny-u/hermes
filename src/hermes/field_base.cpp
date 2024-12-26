#include "hermes/field_base.h"

namespace hermes {

  FieldBase::FieldBase(std::string name,
                       std::string unit,
                       std::string description,
                       unsigned int precision,
                       unsigned int log_level) :
      m_name(std::move(name)),
      m_unit(std::move(unit)),
      m_description(std::move(description)),
      m_precision(precision),
      m_log_level(log_level) {}

  FieldBase::FieldBase(std::string name,
                       std::string description,
                       unsigned int precision,
                       unsigned int log_level) :
      FieldBase(std::move(name), "", std::move(description), precision, log_level) {}

  std::string FieldBase::GetName() const { return m_name; }

  std::string FieldBase::GetUnit() const { return m_unit; }

  std::string FieldBase::GetDescription() const { return m_description; }

  unsigned int FieldBase::precision() const { return m_precision; }

  unsigned int FieldBase::log_level() const { return m_log_level; }

}  // namespace hermes
