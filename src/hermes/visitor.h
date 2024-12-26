#ifndef HERMES_VISITOR_H_
#define HERMES_VISITOR_H_

#include <vector>
#include <functional>
#include <chrono>
#include <Eigen/Core>

namespace hermes {
  template <typename T>
  class Field;
  
  struct Visitor {

    virtual void visit(const Field<int> *field) = 0;

    virtual void visit(const Field<float> *field) = 0;

    virtual void visit(const Field<double> *field) = 0;

    virtual void visit(const Field<bool> *field) = 0;

    virtual void visit(const Field<std::string> *field) = 0;

    virtual void visit(const Field<Eigen::Matrix<double, 3, 1>> *field) = 0;

    virtual void visit(const Field<std::vector<double>> *field) = 0;

    virtual void visit(const Field<std::function<double()>> *field) = 0;

    virtual void visit(const Field<std::chrono::time_point<std::chrono::system_clock>>* field) = 0;

  };
}  // namespace hermes

#endif  // HERMES_VISITOR_H_
