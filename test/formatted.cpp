#include <iostream>
#include <regex>
#include <sstream>

#include <json.h>

bool t01() {
  jsonio::json obj = jsonio::json_obj{};
  obj["a"] = 3;
  obj["b"] = false;
  jsonio::json obj2 = jsonio::json_obj{};
  obj2["cc"] = "some text";
  obj["c"] = obj2;
  auto r = (std::ostringstream{}
            << obj.format().sort().prettify().new_line_bracket())
               .str();
  if (r != "{\n\t\"a\": 3,\n\t\"b\": false,\n\t\"c\":\n\t"
           "{\n\t\t\"cc\": \"some text\"\n\t}\n}") {
    std::cerr << __FUNCTION__ << std::endl;
    return false;
  }
  return true;
}

bool t02() {
  jsonio::json arr = jsonio::json_arr{};
  arr.get_array().emplace_back("a");
  arr.get_array().emplace_back(1L);
  auto r = (std::ostringstream{} << arr.format().prettify().new_line_bracket())
               .str();
  if (r != "[\n\t\"a\",\n\t1\n]") {
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
