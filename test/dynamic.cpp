#include <iostream>
#include <regex>
#include <sstream>

#include <json.h>

bool t01() {
  auto text = R"({"a":"b"})";
  jsonio::json json;
  std::istringstream is{text};
  is >> json;
  json["c"] = 123;
  json["e"] = jsonio::json_obj();
  json["e"]["f"] = true;
  json["g"] = jsonio::json();
  json["g"] = jsonio::json_arr();
  json["g"].get_array().push_back("h");
  json["i"] = jsonio::json();
  json["i"] = jsonio::json_obj();
  json["i"]["j"] = 0.5;
  json["k"] = jsonio::json_arr();
  json["k"].get_array().push_back(jsonio::json_obj());
  std::ostringstream os;
  os << json;
  auto expected =
      R"({"a":"b","c":123,"e":{"f":true},"g":["h"],"i":{"j":0.5},"k":[{}]})";
  if (!json.completed() || json.type() != jsonio::JsonType::J_OBJECT ||
      os.str() != expected) {
    std::cerr << __FUNCTION__ << std::endl;
    std::cerr << os.str() << std::endl;
    return false;
  }
  return true;
}

bool t02() {
  jsonio::json_obj json{
      {{"Name", "Bob"},
       {"Age", 25},
       {"Wife", jsonio::json_obj{{{"Name", "Alice"}, {"Age", 25}}}}}};
  std::ostringstream os;
  os << json;
  auto expected = R"({"Name":"Bob","Age":25,"Wife":{"Name":"Alice","Age":25}})";
  if (!json.completed() || os.str() != expected) {
    std::cerr << __FUNCTION__ << std::endl;
    std::cerr << os.str() << std::endl;
    return false;
  }
  return true;
}

int main() {
  if (t01() && t02() && true) {
    return 0;
  }
  return -1;
}
