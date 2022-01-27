#include <iostream>
#include <regex>
#include <sstream>

#include <json.h>

bool t01()
{
    auto text1 = "1234";
    jsonio::json json1;
    std::istringstream { text1 } >> json1;
    jsonio::json json2;
    auto text2 = "5678";
    std::istringstream { text2 } >> json2;
    json1.steal(json2, false);
    std::ostringstream os1;
    os1 << json1;
    if (!json1.completed() || json1.get_type() != jsonio::JsonType::J_LONG || os1.str() != text2)
    {
        std::cerr << __FUNCTION__ << std::endl;
        return false;
    }
    auto text3 = "9.101";
    jsonio::json json3;
    std::istringstream { text3 } >> json3;
    json1.steal(json3, false);
    std::ostringstream os2;
    os2 << json1;
    if (!json1.completed() || json1.get_type() != jsonio::JsonType::J_LONG || os2.str() != text2)
    {
        std::cerr << __FUNCTION__ << std::endl;
        return false;
    }
    return true;
}

bool t02()
{
    auto text1 = R"({"a": true, "b": true})";
    jsonio::json json1;
    std::istringstream { text1 } >> json1;
    jsonio::json json2;
    auto text2 = R"({"a": 0, "bb": false})";
    std::istringstream { text2 } >> json2;
    json1.steal(json2, true);
    std::ostringstream os;
    os << json1;
    auto arr = os.str();
    std::regex pattern{"\r|\n|\t"};
    std::string out;
    std::regex_replace(std::back_insert_iterator(out), arr.begin(), arr.end(), pattern, "");
    auto text3 = R"({"a": false,"b": true})";
    if (!json1.completed() || json1.get_type() != jsonio::JsonType::J_OBJECT || out != text3)
    {
        std::cerr << __FUNCTION__ << std::endl;
        return false;
    }
    return true;
}

bool t03()
{
    auto text1 = R"({"a": true, "b": [true]})";
    jsonio::json json1;
    std::istringstream { text1 } >> json1;
    jsonio::json json2;
    auto text2 = R"({"aa": false, "b": "1"})";
    std::istringstream { text2 } >> json2;
    json1.steal(json2, true);
    std::ostringstream os;
    os << json1;
    auto arr = os.str();
    std::regex pattern{"\r|\n|\t"};
    std::string out;
    std::regex_replace(std::back_insert_iterator(out), arr.begin(), arr.end(), pattern, "");
    auto text3 = R"({"a": true,"b":["1"]})";
    if (!json1.completed() || json1.get_type() != jsonio::JsonType::J_OBJECT || out != text3)
    {
        std::cerr << __FUNCTION__ << std::endl;
        return false;
    }
    return true;
}

bool t04()
{
    auto text1 = R"({"a": true, "b": "abc"})";
    jsonio::json json1;
    std::istringstream { text1 } >> json1;
    jsonio::json json2;
    auto text2 = R"({"aa": false, "b":[["1"]]})";
    std::istringstream { text2 } >> json2;
    json1.steal(json2, true);
    std::ostringstream os;
    os << json1;
    auto arr = os.str();
    std::regex pattern{"\r|\n|\t"};
    std::string out;
    std::regex_replace(std::back_insert_iterator(out), arr.begin(), arr.end(), pattern, "");
    auto text3 = R"({"a": true,"b": "1"})";
    if (!json1.completed() || json1.get_type() != jsonio::JsonType::J_OBJECT || out != text3)
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
        t02() &&
        t03() &&
        t04() &&
        true)
    {
        return 0;
    }
    return -1;
}
