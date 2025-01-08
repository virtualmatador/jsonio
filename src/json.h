#ifndef JSONIO_SRC_JSON_H
#define JSONIO_SRC_JSON_H

#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <variant>

#include "json_array.hpp"
#include "json_object.hpp"
#include "json_string.h"

namespace jsonio {

enum class JsonType : std::size_t {
  J_NULL,
  J_STRING,
  J_LONG,
  J_DOUBLE,
  J_BOOL,
  J_BINARY,
  J_ARRAY,
  J_OBJECT,
};

template <class json>
using VARIANT_TYPE =
    std::variant<std::monostate, json_string, long, double, bool,
                 std::vector<std::byte>, json_array<json>, json_object<json>>;

class json;

using json_obj = json_object<json>;
using json_arr = json_array<json>;

class json : public VARIANT_TYPE<json> {
private:
  using PARENT_TYPE = VARIANT_TYPE<json>;
  unsigned int flags_;
  std::string buffer_;

  inline static const std::string_view null_{"null"};
  inline static const std::string_view true_{"true"};
  inline static const std::string_view false_{"false"};
  inline static const std::string_view octet_{"octet;"};
  inline static const std::string_view base64_{"base64;"};
  inline static const std::string_view base64_chars_{
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "abcdefghijklmnopqrstuvwxyz"
      "0123456789+/"};

public:
  class formatter {
  private:
    const json &object_;
    unsigned int flags_;

  private:
    formatter(const json &object) : object_{object}, flags_{0} {}
    void write(std::ostream &os) const { object_.write(os, false, 0, flags_); }

  public:
    enum Format_Options : unsigned int {
      prettify_ = 0x0001,
      new_line_bracket_ = 0x0002,
      sort_asc_ = 0x0004,
      sort_desc_ = 0x0008,
      bytes_as_binary_ = 0x0010,
    };

  public:
    formatter &prettify() {
      flags_ |= Format_Options::prettify_;
      return *this;
    }

    formatter &new_line_bracket() {
      flags_ |= Format_Options::new_line_bracket_;
      return *this;
    }

    formatter &sort(bool asc = true) {
      flags_ &= ~(Format_Options::sort_asc_ | Format_Options::sort_desc_);
      flags_ |= asc ? Format_Options::sort_asc_ : Format_Options::sort_desc_;
      return *this;
    }

    formatter &bytes_as_binary() {
      flags_ |= Format_Options::bytes_as_binary_;
      return *this;
    }

    formatter &reset() {
      flags_ = 0;
      return *this;
    }

  public:
    friend json;
    friend std::ostream &operator<<(std::ostream &os,
                                    const json::formatter &source);
  };

private:
  enum : unsigned int {
    PHASE_START = 0x0000,
    PHASE_ARRAY = 0x0001,
    PHASE_OBJECT = 0x0002,
    PHASE_STRING = 0x0003,
    PHASE_NULL = 0x0004,
    PHASE_TRUE = 0x0005,
    PHASE_FALSE = 0x0006,
    PHASE_BASE64_LITERAl = 0x0007,
    PHASE_BASE64_DATA = 0x0008,
    PHASE_OCTET_LITERAL = 0x0009,
    PHASE_OCTET_SIZE = 0x000A,
    PHASE_OCTET_DATA = 0x000B,
    PHASE_INT = 0x000C,
    PHASE_FLOAT = 0x000D,
    PHASE_DELIMITER = 0x000E,
    PHASE_COMPLETED = 0x000F,
    EMPTY_VALUE = 0x0010,
  };

private:
  std::size_t read(std::istream &is, const std::string &delimiters);
  void read_literal(std::istream &is, std::string_view literal);
  std::size_t read_base64_data(std::istream &is, const std::string &delimiters);
  std::size_t read_octet_size(std::istream &is, const std::string &delimiters);
  std::size_t read_octet_data(std::istream &is, const std::string &delimiters);
  std::size_t read_int(std::istream &is, const std::string &delimiters);
  std::size_t read_float(std::istream &is, const std::string &delimiters);
  std::size_t read_delimiter(std::istream &is, const std::string &delimiters);
  void convert_int(std::istream &is);
  void convert_float(std::istream &is);
  void write(std::ostream &os, bool separate, int indents,
             unsigned int flags = 0) const;
  void b64_encode_chunk(const std::byte (&source)[3], char (&dest)[4]) const;
  void b64_decode_chunk(const char (&source)[4], std::byte (&dest)[3]) const;

public:
  json() noexcept;
  json(const json &source) noexcept;
  json(json &&source) noexcept;

  json(const void *null_value);
  json(const std::string &string_value);
  json(std::string &&string_value);
  json(const char *string_value);
  json(std::string_view string_value);
  json(const long &long_value);
  json(const int &int_value);
  json(const std::size_t &size_t_value);
  json(const double &double_value);
  json(const bool &bool_value);
  json(const std::vector<std::byte> &binary_value);
  json(std::vector<std::byte> &&binary_value);
  json(const json_arr &json_array_value);
  json(json_arr &&json_array_value);
  json(const json_obj &json_object_value);
  json(json_obj &&json_object_value);

  json &operator=(const json &source) noexcept;
  json &operator=(json &&source) noexcept;

  json &operator=(const void *null_value);
  json &operator=(const std::string &string_value);
  json &operator=(std::string &&string_value);
  json &operator=(const char *string_value);
  json &operator=(std::string_view string_value);
  json &operator=(const long &long_value);
  json &operator=(const int &int_value);
  json &operator=(const std::size_t &size_t_value);
  json &operator=(const double &double_value);
  json &operator=(const bool &bool_value);
  json &operator=(const std::vector<std::byte> &binary_value);
  json &operator=(std::vector<std::byte> &&binary_value);
  json &operator=(const json_arr &json_array_value);
  json &operator=(json_arr &&json_array_value);
  json &operator=(const json_obj &json_object_value);
  json &operator=(json_obj &&json_object_value);

  ~json() noexcept;

  void steal(const json &source, bool convert);
  void clear();

  bool operator==(const json &that) const;
  bool completed() const;

  JsonType type() const;
  json &operator[](std::size_t index);
  const json &operator[](std::size_t index) const;
  json *at(std::size_t index);
  const json *at(std::size_t index) const;
  json &operator[](const std::string &key);
  const json &operator[](const std::string &key) const;
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
  std::vector<std::byte> &get_binary();
  const std::vector<std::byte> &get_binary() const;
  json_arr &get_array();
  const json_arr &get_array() const;
  json_obj &get_object();
  const json_obj &get_object() const;

  int get_int() const;
  int get_uint() const;
  float get_float() const;

  formatter format() const;

public:
  friend class json_array<json>;
  friend class json_object<json>;
  friend std::istream &operator>>(std::istream &is, json &target);
  friend std::ostream &operator<<(std::ostream &os, const json &source);
};

std::istream &operator>>(std::istream &is, json &target);
std::ostream &operator<<(std::ostream &os, const json &source);

std::ostream &operator<<(std::ostream &os, const json::formatter &source);

} // namespace jsonio

#endif // JSONIO_SRC_JSON_H
