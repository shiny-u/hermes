#ifndef HERMES_MESSAGE_H_
#define HERMES_MESSAGE_H_

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>
#include <iostream>

#include "hermes/field.h"
#ifdef HAS_DUNITS
#include <dunits/dunits.h>
#endif

namespace pqxx {
  class connection;
}  // namespace pqxx

namespace hermes {
  class FieldBase;

  class Serializer;
  class CSVSerializer;
  class PrintSerializer;
  class PSQLSerializer;

  class Message {
   protected:
    using VectorType = std::vector<std::unique_ptr<FieldBase>>;

   public:
    Message() = default;
    Message(const std::string &name, const std::string &description);
    void SetName(std::string name);
    std::string GetName() const;
    void SetDescription(std::string description);
    std::string GetDescription() const;
    void SetNameAndDescription(std::string name, std::string description);
    void SetLogLevel(unsigned int log_level);
    unsigned int GetLogLevel() const;
    void CheckUnits(bool val);
    void IsUnitCaseSensitive(bool val);

    template<class T>
    void AddField(std::string name,
                  std::string unit,
                  std::string description,
                  T *val,
                  unsigned int precision = 5,
                  unsigned int log_level = 0) {
      #ifdef HAS_DUNITS
      if (m_check_units) {
        if (!dunits::UnitsChecker::getInstance().is_valid_unit(unit, m_unit_is_case_sensitive)) {
          // TODO: peut-on traiter ca plutot avec une erreur au build time plutot qu'au runtime ?
          throw std::runtime_error("Unit " + unit + " is not recognized as a standard in dunits reference library.");
        }
      }
      #endif
      m_fields.emplace_back(std::make_unique<Field<T>>(name, unit, description, val, precision, log_level));
      m_mapper[name] = c_nbFields;
      c_nbFields++;
    }

    template<typename T>
    void AddField(std::string name,
                  std::string unit,
                  std::string description,
                  std::function<T()> func,
                  unsigned int precision = 5,
                  unsigned int log_level = 0) {
      #ifdef HAS_DUNITS
      if (m_check_units) {
        if (!dunits::UnitsChecker::getInstance().is_valid_unit(unit, m_unit_is_case_sensitive)) {
          // TODO: peut-on traiter ca plutot avec une erreur au build time plutot qu'au runtime ?
          throw std::runtime_error("Unit " + unit + " is not recognized as a standard in dunits reference library.");
        }
      }
      #endif
      m_fields.emplace_back(std::make_unique<Field<T>>(name, unit, description, func, precision, log_level));
      m_mapper[name] = c_nbFields;
      c_nbFields++;
    }

    template<class T>
    Field<T> *GetField(std::string fieldName) {
      return m_fields[m_mapper[fieldName]].get();
    }

    Serializer* AddSerializer(Serializer *serializer);
    void ApplyVisitor(Visitor &visitor) const ;
    void Initialize() const;
    virtual void Serialize() const;
    virtual void Finalize() const;
    virtual void Send() const;

    // Iterators definition over the container
    typedef VectorType::iterator iterator;
    iterator begin();
    iterator end();

    typedef VectorType::const_iterator const_iterator;
    const_iterator cbegin();
    const_iterator cend();

    // NOTE: Declared in serializer cpp file
    CSVSerializer *AddCSVSerializer(std::string CSVFile);
    PrintSerializer *AddPrintSerializer();
    PSQLSerializer *AddPSQLSerializer(std::shared_ptr<pqxx::connection> conn);

   protected:
    std::string m_name;
    std::string m_description;
    unsigned int m_log_level;

    bool m_check_units;
    bool m_unit_is_case_sensitive;

    VectorType m_fields;

    unsigned int c_nbFields = 0;
    std::unordered_map<std::string, unsigned int> m_mapper;

    std::vector<std::unique_ptr<Serializer>> m_serializers;
  };
}  // namespace hermes

#endif  // HERMES_MESSAGE_H_
