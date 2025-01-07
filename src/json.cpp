#include <algorithm>
#include <cmath>
#include <iomanip>
#include <limits>
#include <memory>

#include "json.h"

jsonio::json::json() noexcept : flags_{PHASE_START} {}

jsonio::json::json(const jsonio::json &source) noexcept { *this = source; }

jsonio::json::json(jsonio::json &&source) noexcept {
  *this = std::move(source);
}

jsonio::json::json(const void *null_value) { *this = null_value; }

jsonio::json::json(const std::string &string_value) { *this = string_value; }

jsonio::json::json(std::string &&string_value) {
  *this = std::move(string_value);
}

jsonio::json::json(const char *string_value) { *this = string_value; }

jsonio::json::json(std::string_view string_value) { *this = string_value; }

jsonio::json::json(const long &long_value) { *this = long_value; }

jsonio::json::json(const int &int_value) { *this = int_value; }

jsonio::json::json(const std::size_t &size_t_value) { *this = size_t_value; }

jsonio::json::json(const double &double_value) { *this = double_value; }

jsonio::json::json(const bool &bool_value) { *this = bool_value; }

jsonio::json::json(const std::vector<std::byte> &binary_value) {
  *this = binary_value;
}

jsonio::json::json(std::vector<std::byte> &&binary_value) {
  *this = std::move(binary_value);
}

jsonio::json::json(const json_arr &json_array_value) {
  *this = json_array_value;
}

jsonio::json::json(json_arr &&json_array_value) {
  *this = std::move(json_array_value);
}

jsonio::json::json(const json_obj &json_object_value) {
  *this = json_object_value;
}

jsonio::json::json(json_obj &&json_object_value) {
  *this = std::move(json_object_value);
}

jsonio::json &jsonio::json::operator=(const jsonio::json &source) noexcept {
  if (this != &source) {
    PARENT_TYPE::operator=(*(PARENT_TYPE *)&source);
    flags_ = source.flags_;
    buffer_ = source.buffer_;
  }
  return *this;
}

jsonio::json &jsonio::json::operator=(jsonio::json &&source) noexcept {
  if (this != &source) {
    PARENT_TYPE::operator=(std::move(*(PARENT_TYPE *)&source));
    flags_ = source.flags_;
    source.flags_ = PHASE_START;
    buffer_ = std::move(source.buffer_);
    source.buffer_ = {};
  }
  return *this;
}

jsonio::json &jsonio::json::operator=(const void *null_value) {
  PARENT_TYPE::operator=(PARENT_TYPE{});
  flags_ = PHASE_COMPLETED;
  return *this;
}

jsonio::json &jsonio::json::operator=(const std::string &string_value) {
  PARENT_TYPE::operator=(json_string{string_value});
  flags_ = PHASE_COMPLETED;
  return *this;
}

jsonio::json &jsonio::json::operator=(std::string &&string_value) {
  PARENT_TYPE::operator=(json_string{std::move(string_value)});
  flags_ = PHASE_COMPLETED;
  return *this;
}

jsonio::json &jsonio::json::operator=(const char *string_value) {
  PARENT_TYPE::operator=(json_string{string_value});
  flags_ = PHASE_COMPLETED;
  return *this;
}

jsonio::json &jsonio::json::operator=(std::string_view string_value) {
  PARENT_TYPE::operator=(json_string{string_value});
  flags_ = PHASE_COMPLETED;
  return *this;
}

jsonio::json &jsonio::json::operator=(const long &long_value) {
  PARENT_TYPE::operator=(long_value);
  flags_ = PHASE_COMPLETED;
  return *this;
}

jsonio::json &jsonio::json::operator=(const int &int_value) {
  PARENT_TYPE::operator=(static_cast<long>(int_value));
  flags_ = PHASE_COMPLETED;
  return *this;
}

jsonio::json &jsonio::json::operator=(const std::size_t &size_t_value) {
  PARENT_TYPE::operator=(static_cast<long>(size_t_value));
  flags_ = PHASE_COMPLETED;
  return *this;
}

jsonio::json &jsonio::json::operator=(const double &double_value) {
  PARENT_TYPE::operator=(double_value);
  flags_ = PHASE_COMPLETED;
  return *this;
}

jsonio::json &jsonio::json::operator=(const bool &bool_value) {
  PARENT_TYPE::operator=(bool_value);
  flags_ = PHASE_COMPLETED;
  return *this;
}

jsonio::json &
jsonio::json::operator=(const std::vector<std::byte> &binary_value) {
  PARENT_TYPE::operator=(binary_value);
  flags_ = PHASE_COMPLETED;
  return *this;
}

jsonio::json &jsonio::json::operator=(std::vector<std::byte> &&binary_value) {
  PARENT_TYPE::operator=(std::move(binary_value));
  flags_ = PHASE_COMPLETED;
  return *this;
}

jsonio::json &
jsonio::json::operator=(const jsonio::json_arr &json_array_value) {
  PARENT_TYPE::operator=(json_array_value);
  flags_ = PHASE_COMPLETED;
  return *this;
}

jsonio::json &jsonio::json::operator=(jsonio::json_arr &&json_array_value) {
  PARENT_TYPE::operator=(std::move(json_array_value));
  flags_ = PHASE_COMPLETED;
  return *this;
}

jsonio::json &
jsonio::json::operator=(const jsonio::json_obj &json_object_value) {
  PARENT_TYPE::operator=(json_object_value);
  flags_ = PHASE_COMPLETED;
  return *this;
}

jsonio::json &jsonio::json::operator=(jsonio::json_obj &&json_object_value) {
  PARENT_TYPE::operator=(std::move(json_object_value));
  flags_ = PHASE_COMPLETED;
  return *this;
}

jsonio::json::~json() noexcept {}

void jsonio::json::steal(const json &source, bool convert) {
  switch (type()) {
  case JsonType::J_NULL:
    switch (source.type()) {
    case JsonType::J_NULL:
      break;
    case JsonType::J_ARRAY:
      if (convert) {
        if (source.get_array().size() == 1) {
          steal(source.get_array()[0], convert);
        }
      }
      break;
    }
    break;
  case JsonType::J_STRING:
    switch (source.type()) {
    case JsonType::J_NULL:
      if (convert) {
        get_string() = "";
      }
      break;
    case JsonType::J_STRING:
      get_string() = source.get_string();
      break;
    case JsonType::J_LONG:
      if (convert) {
        get_string() = std::to_string(source.get_long());
      }
      break;
    case JsonType::J_DOUBLE:
      if (convert) {
        get_string() = std::to_string(source.get_double());
      }
      break;
    case JsonType::J_BOOL:
      if (convert) {
        get_string() = source.get_bool() ? "true" : "false";
      }
      break;
    case JsonType::J_ARRAY:
      if (convert) {
        if (source.get_array().size() == 1) {
          steal(source.get_array()[0], convert);
        }
      }
      break;
    }
    break;
  case JsonType::J_LONG:
    switch (source.type()) {
    case JsonType::J_STRING:
      if (convert) {
        char *progress;
        auto d = std::strtod(source.get_string().c_str(), &progress);
        if (*progress == *source.get_string().end()) {
          if (d == std::round(d)) {
            if (source.get_string() == std::to_string(d)) {
              get_long() = d;
            }
          }
        }
      }
      break;
    case JsonType::J_LONG:
      get_long() = source.get_long();
      break;
    case JsonType::J_DOUBLE:
      if (convert) {
        if (source.get_double() == std::round(source.get_double())) {
          get_long() = static_cast<long>(source.get_double());
        }
      }
      break;
    case JsonType::J_BOOL:
      if (convert) {
        get_long() = source.get_bool() ? 1 : 0;
      }
      break;
    case JsonType::J_ARRAY:
      if (convert) {
        if (source.get_array().size() == 1) {
          steal(source.get_array()[0], convert);
        }
      }
      break;
    }
    break;
  case JsonType::J_DOUBLE:
    switch (source.type()) {
    case JsonType::J_STRING:
      if (convert) {
        char *progress;
        auto d = std::strtod(source.get_string().c_str(), &progress);
        if (*progress == *source.get_string().end()) {
          if (source.get_string() == std::to_string(d)) {
            get_double() = d;
          }
        }
      }
      break;
    case JsonType::J_LONG:
      if (convert) {
        get_double() = source.get_long();
      }
      break;
    case JsonType::J_DOUBLE:
      get_double() = source.get_double();
      break;
    case JsonType::J_BOOL:
      if (convert) {
        get_double() = source.get_bool() ? 1.0 : 0.0;
      }
      break;
    case JsonType::J_ARRAY:
      if (convert) {
        if (source.get_array().size() == 1) {
          steal(source.get_array()[0], convert);
        }
      }
      break;
    }
    break;
  case JsonType::J_BOOL:
    switch (source.type()) {
    case JsonType::J_STRING:
      if (convert) {
        std::string l_source;
        std::transform(source.get_string().begin(), source.get_string().end(),
                       std::back_inserter(l_source),
                       [](auto c) { return std::tolower(c); });
        if (l_source == "0" || l_source == "false" || l_source == "no" ||
            l_source == "off") {
          get_bool() = false;
        } else if (l_source == "1" || l_source == "true" || l_source == "yes" ||
                   l_source == "on") {
          get_bool() = true;
        }
      }
      break;
    case JsonType::J_LONG:
      if (convert) {
        if (source.get_long() == 0) {
          get_bool() = false;
        } else if (source.get_long() == 1) {
          get_bool() = true;
        }
      }
      break;
    case JsonType::J_DOUBLE:
      if (convert) {
        if (source.get_double() == 0.0) {
          get_bool() = false;
        } else if (source.get_double() == 1.0) {
          get_bool() = true;
        }
      }
      break;
    case JsonType::J_BOOL:
      get_bool() = source.get_bool();
      break;
    case JsonType::J_ARRAY:
      if (convert) {
        if (source.get_array().size() == 1) {
          steal(source.get_array()[0], convert);
        }
      }
      break;
    }
    break;
  case JsonType::J_BINARY:
    switch (source.type()) {
    case JsonType::J_BINARY:
      get_binary() = source.get_binary();
      break;
    case JsonType::J_ARRAY:
      if (convert) {
        if (source.get_array().size() == 1) {
          steal(source.get_array()[0], convert);
        }
      }
      break;
    }
    break;
  case JsonType::J_ARRAY:
    switch (source.type()) {
    case JsonType::J_NULL:
    case JsonType::J_STRING:
    case JsonType::J_LONG:
    case JsonType::J_DOUBLE:
    case JsonType::J_BOOL:
    case JsonType::J_OBJECT:
      if (convert) {
        get_array().clear();
        get_array().push_back(source);
      }
      break;
    case JsonType::J_ARRAY:
      get_array() = source.get_array();
      break;
    }
    break;
  case JsonType::J_OBJECT:
    switch (source.type()) {
    case JsonType::J_OBJECT:
      get_object().steal(source.get_object(), convert);
      break;
    case JsonType::J_ARRAY:
      if (convert) {
        if (source.get_array().size() == 1) {
          steal(source.get_array()[0], convert);
        }
      }
      break;
    }
    break;
  }
}

void jsonio::json::clear() {
  flags_ = PHASE_START;
  PARENT_TYPE::operator=(PARENT_TYPE{});
}

bool jsonio::json::operator==(const json &that) const {
  return *static_cast<const PARENT_TYPE *>(this) ==
         *static_cast<const PARENT_TYPE *>(&that);
}

bool jsonio::json::completed() const {
  return (flags_ & PHASE_COMPLETED) == PHASE_COMPLETED;
}

std::size_t jsonio::json::read(std::istream &is,
                               const std::string &delimiters) {
  if ((flags_ & PHASE_COMPLETED) == PHASE_COMPLETED) {
    flags_ = PHASE_START;
  }
  if ((flags_ & PHASE_COMPLETED) == PHASE_START) {
    char source;
    while (is >> source) {
      if (std::isspace(source)) {
        continue;
      } else if (source == ']') {
        if (auto delimiter = delimiters.find(source);
            delimiter != std::string::npos) {
          flags_ |= PHASE_COMPLETED;
          flags_ |= EMPTY_VALUE;
          return delimiter;
        } else {
          is.setstate(std::ios_base::iostate::_S_badbit);
        }
      } else if (source == '{') {
        flags_ &= ~PHASE_COMPLETED;
        flags_ |= PHASE_OBJECT;
        PARENT_TYPE::operator=(json_obj{json_obj::SKIP_PREFIX});
      } else if (source == '[') {
        flags_ &= ~PHASE_COMPLETED;
        flags_ |= PHASE_ARRAY;
        PARENT_TYPE::operator=(json_arr{json_arr::SKIP_PREFIX});
      } else if (source == '\"') {
        flags_ &= ~PHASE_COMPLETED;
        flags_ |= PHASE_STRING;
        PARENT_TYPE::operator=(json_string{});
      } else if (source == null_[0]) {
        flags_ &= ~PHASE_COMPLETED;
        flags_ |= PHASE_NULL;
      } else if (source == true_[0]) {
        flags_ &= ~PHASE_COMPLETED;
        flags_ |= PHASE_TRUE;
      } else if (source == false_[0]) {
        flags_ &= ~PHASE_COMPLETED;
        flags_ |= PHASE_FALSE;
      } else if (source == octet_[0]) {
        flags_ &= ~PHASE_COMPLETED;
        flags_ |= PHASE_OCTET_LITERAL;
      } else if (source == base64_[0]) {
        flags_ &= ~PHASE_COMPLETED;
        flags_ |= PHASE_BASE64_LITERAl;
      } else if (source == '.') {
        flags_ &= ~PHASE_COMPLETED;
        flags_ |= PHASE_FLOAT;
        buffer_.append(1, source);
      } else if ((source >= '0' && source <= '9') || source == '+' ||
                 source == '-') {
        flags_ &= ~PHASE_COMPLETED;
        flags_ |= PHASE_INT;
        buffer_.append(1, source);
      } else {
        is.setstate(std::ios_base::iostate::_S_badbit);
        return std::string::npos;
      }
      break;
    }
  }
  switch (flags_ & PHASE_COMPLETED) {
  case PHASE_OBJECT:
    std::get<json_obj>(**this).read(is);
    if (is.good()) {
      flags_ &= ~PHASE_COMPLETED;
      flags_ |= PHASE_DELIMITER;
      return read_delimiter(is, delimiters);
    }
    break;
  case PHASE_ARRAY:
    std::get<json_arr>(**this).read(is);
    if (is.good()) {
      flags_ &= ~PHASE_COMPLETED;
      flags_ |= PHASE_DELIMITER;
      return read_delimiter(is, delimiters);
    }
    break;
  case PHASE_STRING:
    std::get<json_string>(**this).read(is);
    if (is.good()) {
      flags_ &= ~PHASE_COMPLETED;
      flags_ |= PHASE_DELIMITER;
      return read_delimiter(is, delimiters);
    }
    break;
  case PHASE_NULL:
    read_literal(is, null_);
    if (is.good()) {
      PARENT_TYPE::operator=({});
      flags_ &= ~PHASE_COMPLETED;
      flags_ |= PHASE_DELIMITER;
      return read_delimiter(is, delimiters);
    }
    break;
  case PHASE_TRUE:
    read_literal(is, true_);
    if (is.good()) {
      PARENT_TYPE::operator=(true);
      flags_ &= ~PHASE_COMPLETED;
      flags_ |= PHASE_DELIMITER;
      return read_delimiter(is, delimiters);
    }
    break;
  case PHASE_FALSE:
    read_literal(is, false_);
    if (is.good()) {
      PARENT_TYPE::operator=(false);
      flags_ &= ~PHASE_COMPLETED;
      flags_ |= PHASE_DELIMITER;
      return read_delimiter(is, delimiters);
    }
    break;
  case PHASE_BASE64_LITERAl:
    read_literal(is, base64_);
    if (is.good()) {
      PARENT_TYPE::operator=(std::vector<std::byte>{});
      flags_ &= ~PHASE_COMPLETED;
      flags_ |= PHASE_BASE64_DATA;
      return read_base64_data(is, delimiters);
    }
    break;
  case PHASE_BASE64_DATA:
    return read_base64_data(is, delimiters);
    break;
  case PHASE_OCTET_LITERAL:
    read_literal(is, octet_);
    if (is.good()) {
      PARENT_TYPE::operator=(std::vector<std::byte>{});
      flags_ &= ~PHASE_COMPLETED;
      flags_ |= PHASE_OCTET_SIZE;
      return read_octet_size(is, delimiters);
    }
    break;
  case PHASE_OCTET_SIZE:
    return read_octet_size(is, delimiters);
    break;
  case PHASE_OCTET_DATA:
    return read_octet_data(is, delimiters);
    break;
  case PHASE_INT:
    return read_int(is, delimiters);
    break;
  case PHASE_FLOAT:
    return read_float(is, delimiters);
    break;
  case PHASE_DELIMITER:
    return read_delimiter(is, delimiters);
    break;
  }
  return std::string::npos;
}

void jsonio::json::read_literal(std::istream &is, std::string_view literal) {
  char source;
  while (is >> source) {
    if (source == literal[buffer_.size() + 1]) {
      if (buffer_.size() + 2 == literal.size()) {
        buffer_.clear();
        break;
      } else {
        buffer_.append(1, source);
      }
    } else {
      is.setstate(std::ios::iostate::_S_badbit);
      break;
    }
  }
}

std::size_t jsonio::json::read_base64_data(std::istream &is,
                                           const std::string &delimiters) {
  char source;
  while (is >> source) {
    if (std::isspace(source)) {
      continue;
    } else if (auto delimiter = delimiters.find(source);
               delimiter != std::string::npos) {
      if (buffer_.size() == 0) {
        flags_ |= PHASE_COMPLETED;
      } else {
        is.setstate(std::ios::iostate::_S_badbit);
      }
      return delimiter;
    } else if (buffer_.append(1, source); buffer_.size() == 4) {
      if (buffer_[3] != '=') {
        get_binary().resize(get_binary().size() + 3);
        b64_decode_chunk(reinterpret_cast<const char(&)[4]>(*buffer_.data()),
                         reinterpret_cast<std::byte(&)[3]>(
                             get_binary().at(get_binary().size() - 3)));
        buffer_.clear();
      } else {
        if (buffer_[2] != '=') {
          buffer_[3] = 'A';
          std::byte dest[3];
          b64_decode_chunk(reinterpret_cast<const char(&)[4]>(*buffer_.data()),
                           dest);
          std::copy(dest, dest + 2, std::back_insert_iterator(get_binary()));
        } else {
          buffer_[2] = buffer_[3] = 'A';
          std::byte dest[3];
          b64_decode_chunk(reinterpret_cast<const char(&)[4]>(*buffer_.data()),
                           dest);
          std::copy(dest, dest + 1, std::back_insert_iterator(get_binary()));
        }
        buffer_.clear();
        flags_ &= ~PHASE_COMPLETED;
        flags_ |= PHASE_DELIMITER;
        return read_delimiter(is, delimiters);
      }
    }
  }
  if (delimiters == "\n") {
    if (buffer_.size() == 0) {
      flags_ |= PHASE_COMPLETED;
    } else {
      is.setstate(std::ios::iostate::_S_badbit);
    }
  }
  return std::string::npos;
}

std::size_t jsonio::json::read_octet_size(std::istream &is,
                                          const std::string &delimiters) {
  char source;
  while (is >> source) {
    if (std::isspace(source)) {
      continue;
    } else if (source == ';') {
      flags_ &= ~PHASE_COMPLETED;
      flags_ |= PHASE_OCTET_DATA;
      return read_octet_data(is, delimiters);
    } else {
      buffer_.append(1, source);
    }
  }
  return std::string::npos;
}

std::size_t jsonio::json::read_octet_data(std::istream &is,
                                          const std::string &delimiters) {
  char *end_ptr;
  std::size_t size = strtol(buffer_.c_str(), &end_ptr, 0);
  get_binary().reserve(size);
  if (*end_ptr == '\0') {
    while (is.good()) {
      auto pre_size = get_binary().size();
      get_binary().resize(size);
      auto read_size =
          is.readsome(reinterpret_cast<char *>(get_binary().data() + pre_size),
                      size - pre_size);
      get_binary().resize(pre_size + read_size);
      if (read_size == 0) {
        is.setstate(std::ios::iostate::_S_eofbit);
      } else if (get_binary().size() == size) {
        buffer_.clear();
        flags_ &= ~PHASE_COMPLETED;
        flags_ |= PHASE_DELIMITER;
        return read_delimiter(is, delimiters);
      }
    }
  } else {
    is.setstate(std::ios::iostate::_S_badbit);
  }
  return std::string::npos;
}

std::size_t jsonio::json::read_int(std::istream &is,
                                   const std::string &delimiters) {
  char source;
  while (is >> source) {
    if (std::isspace(source)) {
      continue;
    } else if (auto delimiter = delimiters.find(source);
               delimiter != std::string::npos) {
      convert_int(is);
      return delimiter;
    } else if (buffer_.append(1, source); buffer_.back() == '.') {
      flags_ &= ~PHASE_COMPLETED;
      flags_ |= PHASE_FLOAT;
      return read_float(is, delimiters);
    }
  }
  if (delimiters == "\n") {
    convert_int(is);
  }
  return std::string::npos;
}

std::size_t jsonio::json::read_float(std::istream &is,
                                     const std::string &delimiters) {
  char source;
  while (is >> source) {
    if (std::isspace(source)) {
      continue;
    } else if (auto delimiter = delimiters.find(source);
               delimiter != std::string::npos) {
      convert_float(is);
      return delimiter;
    } else {
      buffer_.append(1, source);
    }
  }
  if (delimiters == "\n") {
    convert_float(is);
  }
  return std::string::npos;
}

std::size_t jsonio::json::read_delimiter(std::istream &is,
                                         const std::string &delimiters) {
  if (delimiters == "\n") {
    flags_ |= PHASE_COMPLETED;
  } else {
    char source;
    while (is >> source) {
      if (std::isspace(source)) {
        continue;
      } else if (auto delimiter = delimiters.find(source);
                 delimiter != std::string::npos) {
        flags_ |= PHASE_COMPLETED;
        return delimiter;
      } else {
        is.setstate(std::ios::iostate::_S_badbit);
        break;
      }
    }
  }
  return std::string::npos;
}

void jsonio::json::convert_int(std::istream &is) {
  char *end_ptr;
  PARENT_TYPE::operator=(strtol(buffer_.c_str(), &end_ptr, 0));
  if (*end_ptr == '\0') {
    buffer_.clear();
    flags_ |= PHASE_COMPLETED;
  } else {
    is.setstate(std::ios::iostate::_S_badbit);
  }
}

void jsonio::json::convert_float(std::istream &is) {
  char *end_ptr;
  PARENT_TYPE::operator=(strtod(buffer_.c_str(), &end_ptr));
  if (*end_ptr == '\0') {
    buffer_.clear();
    flags_ |= PHASE_COMPLETED;
  } else {
    is.setstate(std::ios::iostate::_S_badbit);
  }
}

void jsonio::json::write(std::ostream &os, bool separate, int indents,
                         unsigned int flags) const {
  if (completed()) {
    switch (type()) {
    case JsonType::J_NULL:
      if (flags & formatter::Format_Options::prettify_) {
        if (separate) {
          os << " ";
        }
      }
      os << null_;
      break;
    case JsonType::J_STRING:
      if (flags & formatter::Format_Options::prettify_) {
        if (separate) {
          os << " ";
        }
      }
      os << '\"';
      std::get<json_string>(**this).write(os);
      os << '\"';
      break;
    case JsonType::J_LONG:
      if (flags & formatter::Format_Options::prettify_) {
        if (separate) {
          os << " ";
        }
      }
      os << get_long();
      break;
    case JsonType::J_DOUBLE:
      if (flags & formatter::Format_Options::prettify_) {
        if (separate) {
          os << " ";
        }
      }
      os << std::setprecision(std::numeric_limits<double>::max_digits10)
         << get_double();
      break;
    case JsonType::J_BOOL:
      if (flags & formatter::Format_Options::prettify_) {
        if (separate) {
          os << " ";
        }
      }
      os << (get_bool() ? true_ : false_);
      break;
    case JsonType::J_BINARY:
      if (flags & formatter::Format_Options::prettify_) {
        if (separate) {
          os << " ";
        }
      }
      if ((flags & formatter::Format_Options::bytes_as_binary_)) {
        os << octet_ << get_binary().size() << ';';
        os.write(reinterpret_cast<const char *>(get_binary().data()),
                 get_binary().size());
      } else {
        os << base64_;
        std::size_t i = 0;
        char chunk[4];
        for (; i + 2 < get_binary().size(); i += 3) {
          b64_encode_chunk(reinterpret_cast<const std::byte(&)[3]>(
                               *(get_binary().data() + i)),
                           chunk);
          os.write(chunk, sizeof(chunk));
        }
        if (i == get_binary().size() - 1) {
          std::byte source[3]{get_binary().data()[i], std::byte{0},
                              std::byte{0}};
          b64_encode_chunk(source, chunk);
          chunk[2] = chunk[3] = '=';
          os.write(chunk, sizeof(chunk));
        } else if (i == get_binary().size() - 2) {
          std::byte source[3]{get_binary().data()[i],
                              get_binary().data()[i + 1], std::byte{0}};
          b64_encode_chunk(source, chunk);
          chunk[3] = '=';
          os.write(chunk, sizeof(chunk));
        }
      }
      break;
    case JsonType::J_ARRAY:
      if (flags & formatter::Format_Options::prettify_) {
        if (separate) {
          if (flags & formatter::Format_Options::new_line_bracket_) {
            os << "\n";
          } else {
            os << " ";
          }
        }
      }
      std::get<json_arr>(**this).write(os, indents, flags);
      break;
    case JsonType::J_OBJECT:
      if (flags & formatter::Format_Options::prettify_) {
        if (separate) {
          if (flags & formatter::Format_Options::new_line_bracket_) {
            os << "\n";
          } else {
            os << " ";
          }
        }
      }
      std::get<json_obj>(**this).write(os, indents, flags);
      break;
    }
  }
}

void jsonio::json::b64_encode_chunk(const std::byte (&source)[3],
                                    char (&dest)[4]) const {
  dest[0] = base64_chars_[static_cast<std::size_t>(source[0]) / 4];
  dest[1] = base64_chars_[(static_cast<std::size_t>(source[0]) % 4) * 16 +
                          static_cast<std::size_t>(source[1]) / 16];
  dest[2] = base64_chars_[(static_cast<std::size_t>(source[1]) % 16) * 4 +
                          static_cast<std::size_t>(source[2]) / 64];
  dest[3] = base64_chars_[static_cast<std::size_t>(source[2]) % 64];
}

void jsonio::json::b64_decode_chunk(const char (&source)[4],
                                    std::byte (&dest)[3]) const {
  dest[0] = static_cast<std::byte>(base64_chars_.find(source[0]) * 4 +
                                   base64_chars_.find(source[1]) / 16);
  dest[1] = static_cast<std::byte>((base64_chars_.find(source[1]) % 16) * 16 +
                                   base64_chars_.find(source[2]) / 4);
  dest[2] = static_cast<std::byte>((base64_chars_.find(source[2]) % 4) * 64 +
                                   base64_chars_.find(source[3]));
}

jsonio::JsonType jsonio::json::type() const {
  if (!has_value()) {
    return JsonType::J_NULL;
  }
  return static_cast<JsonType>((*this)->index());
}

jsonio::json &jsonio::json::operator[](std::size_t index) {
  return std::get<json_arr>(**this).operator[](index);
}

const jsonio::json &jsonio::json::operator[](std::size_t index) const {
  return std::get<json_arr>(**this).operator[](index);
}

jsonio::json *jsonio::json::at(std::size_t index) {
  return std::get<json_arr>(**this).at(index);
}

const jsonio::json *jsonio::json::at(std::size_t index) const {
  return std::get<json_arr>(**this).at(index);
}

jsonio::json &jsonio::json::operator[](const std::string &key) {
  return std::get<json_obj>(**this).operator[](key);
}

const jsonio::json &jsonio::json::operator[](const std::string &key) const {
  return std::get<json_obj>(**this).operator[](key);
}

jsonio::json *jsonio::json::at(const std::string &key) {
  return std::get<json_obj>(**this).at(key);
}

const jsonio::json *jsonio::json::at(const std::string &key) const {
  return std::get<json_obj>(**this).at(key);
}

bool jsonio::json::is_null() { return !has_value(); }

std::string &jsonio::json::get_string() {
  return const_cast<std::string &>(
      static_cast<const json &>(*this).get_string());
}

const std::string &jsonio::json::get_string() const {
  return std::get<json_string>(**this);
}

long &jsonio::json::get_long() {
  return const_cast<long &>(static_cast<const json &>(*this).get_long());
}

const long &jsonio::json::get_long() const { return std::get<long>(**this); }

double &jsonio::json::get_double() {
  return const_cast<double &>(static_cast<const json &>(*this).get_double());
}

const double &jsonio::json::get_double() const {
  return std::get<double>(**this);
}

bool &jsonio::json::get_bool() {
  return const_cast<bool &>(static_cast<const json &>(*this).get_bool());
}

const bool &jsonio::json::get_bool() const { return std::get<bool>(**this); }

std::vector<std::byte> &jsonio::json::get_binary() {
  return const_cast<std::vector<std::byte> &>(
      static_cast<const json &>(*this).get_binary());
}

const std::vector<std::byte> &jsonio::json::get_binary() const {
  return std::get<std::vector<std::byte>>(**this);
}

jsonio::json_arr &jsonio::json::get_array() {
  return const_cast<json_arr &>(static_cast<const json &>(*this).get_array());
}

const jsonio::json_arr &jsonio::json::get_array() const {
  return std::get<json_arr>(**this);
}

jsonio::json_obj &jsonio::json::get_object() {
  return const_cast<jsonio::json_obj &>(
      static_cast<const json &>(*this).get_object());
}

const jsonio::json_obj &jsonio::json::get_object() const {
  return std::get<json_obj>(**this);
}

int jsonio::json::get_int() const {
  return static_cast<int>(std::get<long>(**this));
}

int jsonio::json::get_uint() const {
  return static_cast<unsigned int>(std::get<long>(**this));
}

float jsonio::json::get_float() const {
  return static_cast<float>(std::get<double>(**this));
}

jsonio::json::formatter jsonio::json::format() const {
  return formatter(*this);
}

std::istream &jsonio::operator>>(std::istream &is, jsonio::json &target) {
  target.read(is, "\n");
  return is;
}

std::ostream &jsonio::operator<<(std::ostream &os, const jsonio::json &source) {
  source.write(os, false, 0);
  return os;
}

std::ostream &jsonio::operator<<(std::ostream &os,
                                 const jsonio::json::formatter &source) {
  source.write(os);
  return os;
}
