#include "hermes/serializer/print_serializer.h"

#include <iostream>

#include <fmt/format.h>
#include <fmt/ostream.h>
#include <Eigen/Core>

#include "hermes/serialization_visitor.h"
#include "hermes/field.h"
#include "hermes/message.h"
#include "hermes/chrono.h"

namespace hermes {
  class PrintSerializer::Impl {
   public:
    fmt::memory_buffer buffer;
  };

  class PrintSerializer::InitVisitor : public SerializationVisitor<PrintSerializer> {

    inline void visit(FieldBase *field) {
      fmt::format_to(std::back_inserter(m_serializer->m_impl->buffer), "  * {:>10s} ({:^5s}) : {:50s}\n",
                     field->GetName(), field->GetUnit(), field->GetDescription());
    }

   public:
    explicit InitVisitor(PrintSerializer *serializer) : SerializationVisitor<PrintSerializer>(serializer) {}

    void visit(const Field<int> *field) override { visit((FieldBase *) field); }

    void visit(const Field<float> *field) override { visit((FieldBase *) field); }

    void visit(const Field<double> *field) override { visit((FieldBase *) field); }

    void visit(const Field<bool> *field) override { visit((FieldBase *) field); }

    void visit(const Field<std::string> *field) override { visit((FieldBase *) field); }

    void visit(const Field<Eigen::Matrix<double, 3, 1>> *field) override { visit((FieldBase *) field); }

    void visit(const Field<std::vector<double>> *field) override { visit((FieldBase *) field); }

    void visit(const Field<std::function<double()>> *field) override { visit((FieldBase *) field); }

    void visit(const Field<std::chrono::time_point<std::chrono::system_clock>>* field) override {
      visit((FieldBase *) field);
    }
  };

  class PrintSerializer::SerializeVisitor : public SerializationVisitor<PrintSerializer> {
   public:
    explicit SerializeVisitor(PrintSerializer *serializer) :
        SerializationVisitor<PrintSerializer>(serializer) {}

    void visit(const Field<int> *field) override {
      fmt::format_to(std::back_inserter(m_serializer->m_impl->buffer), "  * {:>10s} ({:^5s}) : {:d}\n",
                     field->GetName(), field->GetUnit(), field->GetData());
    }

    void visit(const Field<float> *field) override {
      fmt::format_to(std::back_inserter(m_serializer->m_impl->buffer), "  * {:>10s} ({:^5s}) : {:.12g}\n",
                     field->GetName(), field->GetUnit(), field->GetData());
    }

    void visit(const Field<double> *field) override {
      fmt::format_to(std::back_inserter(m_serializer->m_impl->buffer), "  * {:>10s} ({:^5s}) : {:.12g}\n",
                     field->GetName(), field->GetUnit(), field->GetData());
    }

    void visit(const Field<bool> *field) override {
      fmt::format_to(std::back_inserter(m_serializer->m_impl->buffer), "  * {:>10s} : {:d}\n",
                     field->GetName(), field->GetData());
    }

    void visit(const Field<std::string> *field) override {  // TODO: parametrer les formats...
      fmt::format_to(std::back_inserter(m_serializer->m_impl->buffer), "  * {:>10s} ({:^5s}) : {:s}\n",
                     field->GetName(), field->GetUnit(), field->GetData());
    }

    void visit(const Field<Eigen::Matrix<double, 3, 1>> *field) override {
      auto vector = field->GetData();
      fmt::format_to(std::back_inserter(m_serializer->m_impl->buffer),
                     "  * {:>10s} ({:^5s}) : {:.12g}\t{:.12g}\t{:.12g}\n",
                     field->GetName(), field->GetUnit(), vector[0], vector[1], vector[2]);
    }

    void visit(const Field<std::vector<double>> *field) override {
      auto vector = field->GetData();
      fmt::format_to(std::back_inserter(m_serializer->m_impl->buffer), "  * {:>10s} ({:^5s}) : ",
                     field->GetName(), field->GetUnit());
      for (const auto &element: vector) {
        fmt::format_to(std::back_inserter(m_serializer->m_impl->buffer), "{}{:.12g}\t",
                       m_serializer->m_impl->buffer.data(), element);
      }
      fmt::format_to(std::back_inserter(m_serializer->m_impl->buffer), "{}\n", m_serializer->m_impl->buffer.data());
    }

    void visit(const Field<std::function<double(void)>> *field) override {
      fmt::format_to(std::back_inserter(m_serializer->m_impl->buffer), "  * {:>10s} ({:^5s}) : {:.12g}\n",
                     field->GetName(), field->GetUnit(), (field->GetData())());
    }

    void visit(const Field<std::chrono::time_point<std::chrono::system_clock>>* field) override {
      fmt::format_to(std::back_inserter(m_serializer->m_impl->buffer), "  * {:>10s} ({:^5s}) : {:s}\n",
                     field->GetName(), field->GetUnit(), ISO8601(field->GetData()));
    }
  };

  void PrintSerializer::WriteHeader(const Message *msg) {
    fmt::format_to(std::back_inserter(m_impl->buffer), "\n[{}] : {}\n", msg->GetName(), msg->GetDescription());
  }


  PrintSerializer::PrintSerializer() :
    m_impl(std::make_unique<Impl>()),
    m_serializeVisitor(std::make_unique<SerializeVisitor>(this)) {}

  void PrintSerializer::Initialize(const Message *msg) {
    // Writing message header
    WriteHeader(msg);

    InitVisitor visitor(this);
    msg->ApplyVisitor(visitor);
  }

  void PrintSerializer::Serialize(const Message *msg) {
    WriteHeader(msg);
    msg->ApplyVisitor(*m_serializeVisitor);
  }

  void PrintSerializer::Finalize(const Message *msg) {}

  void PrintSerializer::Send(const Message *msg) {
    std::cout << fmt::to_string(m_impl->buffer);
    m_impl->buffer.clear();
  }

  PrintSerializer *Message::AddPrintSerializer() {
    auto *printSerializer = AddSerializer(new PrintSerializer());
    return dynamic_cast<PrintSerializer *>(printSerializer);
  }
}  // namespace hermes
