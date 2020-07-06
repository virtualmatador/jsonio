#ifndef JSONIO_SRC_JSON_H
#define JSONIO_SRC_JSON_H

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

class json_object;
class json_array;

using VARIANT_TYPE = std::variant
<
    void*,
    json_string,
    long,
    double,
    bool,
    std::unique_ptr<json_object>,
    std::unique_ptr<json_array>
>;

class json : public VARIANT_TYPE
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
        MASK_PHASE = 0x0007,
        EMPTY_VALUE = 0x0008;

private:
    size_t read(std::istream & is, const std::string & delimiters);
    void write(std::ostream & os, int indents) const;

public:
    json() noexcept;
    json(const json & source) noexcept;
    json(json && source) noexcept;

    json(const void* null_value);
    json(const std::string & string_value);
    json(std::string && string_value);
    json(const char* string_value);
    json(const long & long_value);
    json(const double & double_value);
    json(const bool & bool_value);
    json(const json_object & json_object_value);
    json(json_object && json_object_value);
    json(const json_array & json_array_value);
    json(json_array && json_array_value);

    json & operator=(const json & source) noexcept;
    json & operator=(json && source) noexcept;

    json & operator=(const void* null_value);
    json & operator=(const std::string & string_value);
    json & operator=(std::string && string_value);
    json & operator=(const char* string_value);
    json & operator=(const long & long_value);
    json & operator=(const double & double_value);
    json & operator=(const bool & bool_value);
    json & operator=(const json_object & json_object_value);
    json & operator=(json_object && json_object_value);
    json & operator=(const json_array & json_array_value);
    json & operator=(json_array && json_array_value);

    ~json() noexcept;

    bool completed() const;

    JsonType get_type() const;
    json & operator[](const std::string & key);
    const json & operator[](const std::string & key) const;
    json & operator[](size_t index);
    const json & operator[](size_t index) const;
    const json* get_value(const std::string & key) const;

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
    json_object & get_object();
    const json_object & get_object() const;
    json_array & get_array();
    const json_array & get_array() const;

public:
    friend class json_pair;
    friend class json_array;
    friend std::istream & operator>>(std::istream & is, json & target);
    friend std::ostream & operator<<(std::ostream & os, const json & source);
};
    
std::istream & operator>>(std::istream & is, json & target);
std::ostream & operator<<(std::ostream & os, const json & source);

} // namespace jsonio

#endif //JSONIO_SRC_JSON_H
