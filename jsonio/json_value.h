#ifndef JSONIO_JSON_VALUE_H
#define JSONIO_JSON_VALUE_H

#include <iostream>
#include <memory>
#include <string>
#include <variant>

#include "json_string.h"

namespace jsonio
{

enum class JsonType : size_t
{
    J_NULL,
    J_STRING,
    J_LONG,
    J_DOUBLE,
    J_BOOL,
    J_OBJECT,
    J_ARRAY,
};

class json;
class json_array;

using VARIANT_TYPE = std::variant
<
    void*,
    json_string,
    long,
    double,
    bool,
    std::unique_ptr<json>,
    std::unique_ptr<json_array>
>;

class json_value : public VARIANT_TYPE
{
private:
    unsigned int flags_;
    std::string binary_;

private:
    static const unsigned int
        PHASE_START = 0x0000,
        PHASE_ARRAY = 0x0001,
        PHASE_OBJECT = 0x0002,
        PHASE_STRING = 0x0003,
        PHASE_BINARY = 0x0004,
        PHASE_DELIMITER = 0x0005,
        PHASE_COMPLETED = 0x0006,
        MASK_PHASE = 0x0007;

private:
    size_t read(std::istream & is, const std::string & delimiters);
    void write(std::ostream & os, int indents) const;

public:
    json_value() noexcept;
    json_value(const json_value & source) noexcept;
    json_value(json_value && source) noexcept;

    json_value(const void* null_value);
    json_value(const std::string & string_value);
    json_value(std::string && string_value);
    json_value(const char* string_value);
    json_value(const long & long_value);
    json_value(const double & double_value);
    json_value(const bool & bool_value);
    json_value(const json & json_object_value);
    json_value(json && json_object_value);
    json_value(const json_array & json_array_value);
    json_value(json_array && json_array_value);

    json_value & operator=(const json_value & source) noexcept;
    json_value & operator=(json_value && source) noexcept;

    json_value & operator=(const void* null_value);
    json_value & operator=(const std::string & string_value);
    json_value & operator=(std::string && string_value);
    json_value & operator=(const char* string_value);
    json_value & operator=(const long & long_value);
    json_value & operator=(const double & double_value);
    json_value & operator=(const bool & bool_value);
    json_value & operator=(const json & json_object_value);
    json_value & operator=(json && json_object_value);
    json_value & operator=(const json_array & json_array_value);
    json_value & operator=(json_array && json_array_value);

    ~json_value() noexcept;

    bool completed() const;

    JsonType get_type() const;
    json_value & operator[](const std::string & key);
    const json_value & operator[](const std::string & key) const;
    json_value & operator[](size_t index);
    const json_value & operator[](size_t index) const;

    void* & get_null();
    void* const & get_null() const;
    json_string & get_string();
    const json_string & get_string() const;
    long & get_long();
    const long & get_long() const;
    double & get_double();
    const double & get_double() const;
    bool & get_bool();
    const bool & get_bool() const;
    json & get_object();
    const json & get_object() const;
    json_array & get_array();
    const json_array & get_array() const;

public:
    friend class json_pair;
    friend class json_array;
    friend std::istream & operator>>(std::istream & is, json_value & target);
    friend std::ostream & operator<<(std::ostream & os, const json_value & source);
};
    
std::istream & operator>>(std::istream & is, json_value & target);
std::ostream & operator<<(std::ostream & os, const json_value & source);

} // namespace jsonio

#endif //JSONIO_JSON_VALUE_H
