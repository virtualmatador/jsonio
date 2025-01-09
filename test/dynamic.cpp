#include <iostream>
#include <regex>
#include <sstream>

#include <json.hpp>

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
  json["i"]["k"] = std::vector{std::byte{' '}, std::byte{' '}};
  json["l"] = jsonio::json_arr();
  json["l"].get_array().push_back(jsonio::json_obj());
  auto r = (std::ostringstream{} << json.format().sort()).str();
  auto expected =
      R"({"a":"b","c":123,"e":{"f":true},"g":["h"],"i":{"j":0.5,"k":base64;ICA=},"l":[{}]})";
  if (!json.completed() || json.type() != jsonio::JsonType::J_OBJECT ||
      r != expected) {
    std::cerr << __FUNCTION__ << std::endl;
    return false;
  }
  return true;
}

bool t02() {
  jsonio::json json{jsonio::json_obj{
      {{"Name", "Bob"},
       {"Age", 25},
       {"Wife", jsonio::json_obj{{{"Name", "Alice"}, {"Age", 25}}}}}}};
  auto r = (std::ostringstream{} << json.format().sort()).str();
  auto expected = R"({"Age":25,"Name":"Bob","Wife":{"Age":25,"Name":"Alice"}})";
  if (!json.completed() || r != expected) {
    std::cerr << __FUNCTION__ << std::endl;
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
