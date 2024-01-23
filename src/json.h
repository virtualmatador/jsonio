#ifndef JSONIO_SRC_JSON_H
#define JSONIO_SRC_JSON_H

#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <variant>

#include "json_array.hpp"
#include "json_object.hpp"
#include "json_string.h"

namespace jsonio {

template <class json>
using VARIANT_TYPE =
    std::optional<std::variant<json_string, long, double, bool,
                               json_array<json>, json_object<json>>>;

class json;

using json_obj = json_object<json>;
using json_arr = json_array<json>;

class json : public VARIANT_TYPE<json> {
private:
  using PARENT_TYPE = VARIANT_TYPE<json>;
  unsigned int flags_;
  std::string binary_;

private:
  static constexpr unsigned int PHASE_START = 0x0000, PHASE_ARRAY = 0x0001,
                                PHASE_OBJECT = 0x0002, PHASE_STRING = 0x0003,
                                PHASE_BINARY = 0x0004, PHASE_DELIMITER = 0x0005,
                                PHASE_COMPLETED = 0x0006, MASK_PHASE = 0x0007,
                                EMPTY_VALUE = 0x0008;

private:
  std::size_t read(std::istream &is, const std::string &delimiters);
  void write(std::ostream &os, int indents) const;

public:
  json() noexcept;
  json(const json &source) noexcept;
  json(json &&source) noexcept;

  json(const void *null_value);
  json(const std::string &string_value);
  json(std::string &&string_value);
  json(const char *string_value);
  json(const long &long_value);
  json(const int &int_value);
  json(const std::size_t &size_t_value);
  json(const double &double_value);
  json(const bool &bool_value);
  json(const json_obj &json_object_value);
  json(json_obj &&json_object_value);
  json(const json_arr &json_array_value);
  json(json_arr &&json_array_value);

  json &operator=(const json &source) noexcept;
  json &operator=(json &&source) noexcept;

  json &operator=(const void *null_value);
  json &operator=(const std::string &string_value);
  json &operator=(std::string &&string_value);
  json &operator=(const char *string_value);
  json &operator=(const long &long_value);
  json &operator=(const int &int_value);
  json &operator=(const std::size_t &size_t_value);
  json &operator=(const double &double_value);
  json &operator=(const bool &bool_value);
  json &operator=(const json_obj &json_object_value);
  json &operator=(json_obj &&json_object_value);
  json &operator=(const json_arr &json_array_value);
  json &operator=(json_arr &&json_array_value);

  ~json() noexcept;

  void steal(const json &source, bool convert);
  void clear();

  bool operator==(const json &that) const;
  bool completed() const;

  JsonType type() const;
  json &operator[](const std::string &key);
  const json &operator[](const std::string &key) const;
  json &operator[](std::size_t index);
  const json &operator[](std::size_t index) const;
  json *at(const std::string &key);
  const json *at(const std::string &key) const;

  bool is_null();
  std::string &get_string();
  const std::string &get_string() const;
  long &get_long();
  const long &get_long() const;
  double &get_double();
  const double &get_double() const;
  bool &get_bool();
  const bool &get_bool() const;
  json_obj &get_object();
  const json_obj &get_object() const;
  json_arr &get_array();
  const json_arr &get_array() const;
  float get_float() const;

public:
  friend class json_array<json>;
  friend class json_object<json>;
  friend std::istream &operator>>(std::istream &is, json &target);
  friend std::ostream &operator<<(std::ostream &os, const json &source);
};

std::istream &operator>>(std::istream &is, json &target);
std::ostream &operator<<(std::ostream &os, const json &source);

} // namespace jsonio

#endif // JSONIO_SRC_JSON_H
