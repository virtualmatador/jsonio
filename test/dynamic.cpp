#include <iostream>
#include <regex>
#include <sstream>

#include <json.h>

bool t01()
{
    auto text = R"({"a":"b"})";
    jsonio::json json;
    std::istringstream is{text};
    is >> json;
    json.get_object().insert({"c", jsonio::json()});
    json["c"] = long(123);
    json.get_object().insert({"e", jsonio::json_object<jsonio::json>()});
    json["e"].get_object().insert({"f", true});
    json.get_object().insert({"g", jsonio::json()});
    json["g"] = jsonio::json_array<jsonio::json>();
    json["g"].get_array().push_back("h");
    json.get_object().insert({"i", jsonio::json()});
    json["i"] = jsonio::json_object<jsonio::json>();
    json["i"].get_object().insert({"j", 0.5});
    json.get_object().insert({"k", jsonio::json_array<jsonio::json>()});
    json["k"].get_array().push_back(jsonio::json_object<jsonio::json>());
    std::ostringstream os;
    os << json;
    auto arr = os.str();
    std::regex pattern{"\r|\n|\t| "};
    std::string out;
    std::regex_replace(std::back_insert_iterator(out), arr.begin(), arr.end(), pattern, "");
    auto expected = R"({"a":"b","c":123,"e":{"f":true},"g":["h"],"i":{"j":0.5},"k":[{}]})";
    if (!json.completed() || json.get_type() != jsonio::JsonType::J_OBJECT || out != expected)
    {
        std::cerr << __FUNCTION__ << std::endl;
        return false;
    }
    return true;
}

int main()
{
    if (
        t01() &&
        true)
    {
        return 0;
    }
    return -1;
}
