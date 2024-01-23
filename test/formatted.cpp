#include <iostream>
#include <regex>
#include <sstream>

#include <json.h>

bool t01() {
  jsonio::json_obj obj;
  obj["a"] = 3;
  obj["b"] = false;
  jsonio::json_obj obj2;
  obj2["cc"] = "some text";
  obj["c"] = obj2;
  std::ostringstream os;
  os << std::noskipws << obj;
  if (os.str() != "{\n\t\"a\": 3,\n\t\"b\": false,\n\t\"c\":\n\t"
                  "{\n\t\t\"cc\": \"some text\"\n\t}\n}") {
    std::cerr << __FUNCTION__ << std::endl;
    return false;
  }
  return true;
}

bool t02() {
  jsonio::json_arr arr;
  arr.emplace_back("a");
  arr.emplace_back(1L);
  std::ostringstream os;
  os << std::noskipws << arr;
  if (os.str() != "[\n\t\"a\",\n\t1\n]") {
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
