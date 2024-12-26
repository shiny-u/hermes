#include "hermes/message.h"
#include "hermes/serializer.h"

namespace hermes {

  Message::Message(const std::string &name, const std::string &description) :
      m_name(name),
      m_description(description),
      m_log_level(HERMES_MAX_LOG_LEVEL),
      m_check_units(true),
      m_unit_is_case_sensitive(true) {}

  void Message::SetName(std::string name) { m_name = std::move(name); }

  std::string Message::GetName() const { return m_name; }

  void Message::SetDescription(std::string description) { m_description = std::move(description); }

  std::string Message::GetDescription() const { return m_description; }

  void Message::SetNameAndDescription(std::string name, std::string description) {
    m_name = std::move(name);
    m_description = std::move(description);
  }

  void Message::SetLogLevel(unsigned int log_level) {
    if (log_level > HERMES_MAX_LOG_LEVEL) {
      throw std::runtime_error("Maximum log level allowable in hermes is "
                               + std::to_string(HERMES_MAX_LOG_LEVEL)
                               + ". Requested is "
                               + std::to_string(log_level));
    }
    m_log_level = log_level;
  }

  unsigned int Message::GetLogLevel() const { return m_log_level; }

  void Message::CheckUnits(bool val) { m_check_units = val; }

  void Message::IsUnitCaseSensitive(bool val) { m_unit_is_case_sensitive = val; }

  Serializer* Message::AddSerializer(Serializer *serializer) {
    m_serializers.push_back(std::unique_ptr<Serializer>(serializer));
    return m_serializers[m_serializers.size() - 1].get();
  }


  void Message::ApplyVisitor(Visitor &visitor) const {
    for (const auto &fieldUPtr: m_fields) {
      if (fieldUPtr->log_level() <= m_log_level) fieldUPtr->Accept(visitor);
    }
  }

  void Message::Initialize() const {
    for (const auto &serializer: m_serializers) {
      serializer->Initialize(this);
    }
  }

  void Message::Serialize() const {
    for (const auto &serializer: m_serializers) {
      serializer->Serialize(this);
    }
  }

  void Message::Finalize() const {
    for (const auto &serializer: m_serializers) {
      serializer->Finalize(this);
    }
  }

  void Message::Send() const {
    for (const auto &serializer: m_serializers) {
      serializer->Send(this);
    }
  }

  Message::iterator Message::begin() { return m_fields.begin(); }

  Message::iterator Message::end() { return m_fields.end(); }

  Message::const_iterator Message::cbegin() { return m_fields.cbegin(); }

  Message::const_iterator Message::cend() { return m_fields.cend(); }
}  // namespace hermes