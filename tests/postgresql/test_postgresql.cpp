#include "gtest/gtest.h"

#include <thread>
#include <limits>

#include <spdlog/spdlog.h>
#include <pqxx/pqxx>

#include "hermes/hermes.h"

TEST(Hermes, PostgreSQL) {
  spdlog::set_level(spdlog::level::trace);

  struct MessageData {
    int i{};
    double d{};
    std::string str;
    bool bo{};
    std::chrono::time_point<std::chrono::system_clock> time;
  };
  MessageData data;
  hermes::Message m("MSG1", "Le premier message");

  m.AddField("Nb", "m/s", "Un entier", &data.i);
  m.AddField("this.is.a.field", "km", "Un double", &data.d);
  m.AddField("name", "--", "Une chaine", &data.str);
  m.AddField("is_something", "--", "", &data.bo);
  m.AddField("time", "--", "Generation timestamp", &data.time);


  std::shared_ptr<pqxx::connection> connection;
  try {
    connection = std::make_shared<pqxx::connection>("user=postgres password=password host=localhost dbname=database");
  } catch (const pqxx::failure& err) {
    spdlog::info("Connection failed ! {}", err.what());
    ASSERT_FALSE(true);
  }
  m.AddPSQLSerializer(connection);

  // Initialize
  m.Initialize();
  m.Send();

  // Using the message serialization
  for (int idx = 0; idx < 10; ++idx) {
    data.i += 1;
    data.d = static_cast<double>(data.i) / 100;
    data.str = fmt::format("Currend index is {}", idx);
    data.bo = idx < 4;
    data.time = std::chrono::system_clock::now();
    m.Serialize();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
  m.Send();
}

TEST(Hermes, TimeTypeError) {
  struct MessageData {
    int i{};
    double time{};
  };
  MessageData data;
  hermes::Message m("MSG2", "Le Second message");
  m.AddField("Nb", "m/s", "Un entier", &data.i);
  m.AddField("time", "s since epoch", "Generation timestamp", &data.time);

  std::shared_ptr<pqxx::connection> connection;
  try {
    connection = std::make_shared<pqxx::connection>("user=postgres password=password host=localhost dbname=database");
  } catch (const pqxx::failure& err) {
    spdlog::info("Connection failed ! {}", err.what());
    ASSERT_FALSE(true);
  }
  m.AddPSQLSerializer(connection);
  try {
    m.Initialize();
    ASSERT_FALSE(true);
  } catch (const std::runtime_error& err) {
    spdlog::info("{}", err.what());
  }
}

TEST(Hermes, NaNManagement) {
  struct MessageData {
    float f1 { 0.0f/0.0f };
    float f2 { std::numeric_limits<float>::quiet_NaN() };
    double d1 { 0.0/0.0 };
    double d2 { std::numeric_limits<double>::quiet_NaN() };
    std::function<double()> funct { [](){ return std::numeric_limits<double>::quiet_NaN();} };
  };

  MessageData data;
  hermes::Message m("MSG3", "Le troisième message");

  m.AddField("f1", "--", "0 divided by 0", &data.f1);
  m.AddField("f2", "--", "quiet nan", &data.f2);
  m.AddField("d1", "--", "0 divided by 0", &data.d1);
  m.AddField("d2", "--", "quiet nan", &data.d2);
  m.AddField("funct", "--", "function returning quiet nan", &data.funct);

  std::shared_ptr<pqxx::connection> connection;
  try {
    connection = std::make_shared<pqxx::connection>("user=postgres password=password host=localhost dbname=database");
  } catch (const pqxx::failure& err) {
    FAIL() << "Connection failed! " << err.what();
  }
  m.AddPSQLSerializer(connection);

  // Initialize
  m.Initialize();
  m.Serialize();
  m.Send();

  // Use pgAdmin (or command line) to inspect data in the DB, you should see NaN values in all columns of 'MSG3' table
}