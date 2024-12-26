#include <gtest/gtest.h>
#include <iostream>

#include <fmt/format.h>

#include "hermes/hermes.h"
#include "hermes/version.h"

struct Obj {
  int i;
  double d;
  std::string str;
  std::chrono::time_point<std::chrono::system_clock> time;
};

TEST(Hermes, Basic) {
  using namespace hermes;
  Obj o;
  o.i = 2;
  o.d = 3.14989796584651681;
  o.str = "coucou";
  o.time = std::chrono::system_clock::now();

  Message m("MSG1", "Le premier message");


  m.AddField<int>("Nb", "mps", "Un entier", &o.i);
  m.AddField<double>("pi", "km", "Un double", &o.d);
  m.AddField<std::string>("name", "-", "Une chaine", &o.str);
  m.AddField("time", "s", "A time", &o.time);

  Message m2("MSG2", "Un second message");

  int i = -23;
  double d = -1.414;
  float f = 1.397e12;

  m2.AddField<double>("double2", "A", "Second double", &d);
  m.AddField<float>("float", "A", "Un float", &f);

  // Adding two standard serializers
  CSVSerializer* csvSerializer = m.AddCSVSerializer("test_csv_serializer.csv");
  csvSerializer->SetDelimiter(" ; ");

  // Adding a vector
  std::vector<double> vector;
  vector.reserve(10);

  double val = 0;
  double dt = 0.2;
  for (int idx=0; idx<10; ++idx) {
    vector.push_back(val);
    val += dt;
//        std::cout << val;
  }

  // Essai pour renseigner dans un message
  std::string fieldame;
  for (int idx=0; idx<vector.size(); ++idx) {
    fieldame = fmt::format("x_{:d}", idx);
//        std::cout << fieldame << std::endl;
    m.AddField(fieldame, "C", "", &vector[idx]);

  }
  m.AddPrintSerializer();

  // Initializing the message
  m.Initialize();
  m.Send();

  // Using the message serialization
  for (int idx=0; idx<10; ++idx) {
    m.Serialize();
    m.Send();
  }
}

TEST(Hermes, CSVFileWriting) {

  using namespace hermes;

  Message m("MSG", "My message");

  double one = 1;
  double two = 2;

  m.AddField("one", "-", "First", &one);
  m.AddField("two", "-", "Second", &two);

  auto serializer = m.AddCSVSerializer("test.csv");
  serializer->SetDelimiter(";");

  m.Initialize();

  int counter = 1;
  for (int idx = 0; idx<100; ++idx) {
    m.Serialize();

    if (counter == 10) {
      m.Send();
      counter = 1;
    }

    counter++;
  }

  m.Finalize();

}
