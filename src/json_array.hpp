#ifndef JSONIO_SRC_JSON_ARRAY_HPP
#define JSONIO_SRC_JSON_ARRAY_HPP

#include <iostream>
#include <memory>
#include <vector>

namespace jsonio {

template <class json> class json_array : public std::vector<json> {
private:
  using PARENT_TYPE = std::vector<json>;
  unsigned int flags_;
  std::unique_ptr<json> value_;

public:
  enum : unsigned int {
    PHASE_START = 0x0000,
    PHASE_VALUE = 0x0001,
    PHASE_COMPLETED = 0x0003,
    SKIP_PREFIX = 0x0004,
  };

public:
  json_array(PARENT_TYPE &&init) noexcept
      : PARENT_TYPE{std::move(init)}, flags_{PHASE_COMPLETED},
        value_{std::make_unique<json>()} {}

  json_array() noexcept
      : flags_{PHASE_COMPLETED}, value_{std::make_unique<json>()} {}

  json_array(const unsigned int flags) noexcept
      : flags_{flags}, value_{std::make_unique<json>()} {}

  json_array(const json_array &that) noexcept
      : value_{std::make_unique<json>()} {
    *this = that;
  }

  json_array(json_array &&that) noexcept : value_{std::make_unique<json>()} {
    *this = std::move(that);
  }

  json_array &operator=(const json_array &source) noexcept {
    if (this != &source) {
      PARENT_TYPE::operator=(*(PARENT_TYPE *)&source);
      flags_ = source.flags_;
      *value_ = *source.value_;
    }
    return *this;
  }

  json_array &operator=(json_array &&source) noexcept {
    if (this != &source) {
      PARENT_TYPE::operator=(std::move(*(PARENT_TYPE *)&source));
      flags_ = source.flags_;
      source.flags_ = PHASE_START;
      *value_ = std::move(*source.value_);
    }
    return *this;
  }

  bool completed() const {
    return (flags_ & PHASE_COMPLETED) == PHASE_COMPLETED;
  }

  json *at(std::size_t index) {
    return const_cast<json *>(static_cast<const json_array &>(*this).at(index));
  }

  const json *at(std::size_t index) const {
    if (index < PARENT_TYPE::size()) {
      return &PARENT_TYPE::at(index);
    }
    return nullptr;
  }

  void read(std::istream &is) {
    if ((flags_ & PHASE_COMPLETED) == PHASE_COMPLETED)
      flags_ = PHASE_START;
    if ((flags_ & PHASE_COMPLETED) == PHASE_START) {
      value_->flags_ = json::PHASE_START;
      if (flags_ & SKIP_PREFIX) {
        flags_ &= ~PHASE_COMPLETED;
        flags_ |= PHASE_VALUE;
      } else {
        char source;
        while (is >> source) {
          if (std::isspace(source)) {
            continue;
          } else if (source == '[') {
            flags_ &= ~PHASE_COMPLETED;
            flags_ |= PHASE_VALUE;
          } else {
            is.setstate(std::ios::iostate::_S_badbit);
          }
          break;
        }
      }
    }
    if ((flags_ & PHASE_COMPLETED) == PHASE_VALUE) {
      const std::string delimiters = ",]";
      for (;;) {
        if (auto delimiter = value_->read(is, delimiters);
            delimiter != std::string::npos) {
          if ((value_->flags_ & json::EMPTY_VALUE) != json::EMPTY_VALUE) {
            PARENT_TYPE::push_back(std::move(*value_));
          } else if (PARENT_TYPE::size() != 0) {
            is.setstate(std::ios::iostate::_S_badbit);
            break;
          }
          if (delimiters[delimiter] == ']') {
            flags_ |= PHASE_COMPLETED;
            break;
          }
        } else {
          break;
        }
      }
    }
  }

  const void write(std::ostream &os, int indents, unsigned int flags) const {
    if (completed()) {
      if (flags & json::formatter::Format_Options::prettify_) {
        for (int i = 0; i < indents; ++i) {
          os << '\t';
        }
      }
      os << "[";
      bool comma = false;
      for (const auto &sub_JsonValue : *this) {
        if (sub_JsonValue.completed()) {
          if (comma) {
            os << ',';
          } else {
            comma = true;
          }
          if (flags & json::formatter::Format_Options::prettify_) {
            os << '\n';
            for (int i = 0; i < indents + 1; ++i) {
              os << '\t';
            }
          }
          sub_JsonValue.write(os, false, indents + 1);
        }
      }
      if (flags & json::formatter::Format_Options::prettify_) {
        os << '\n';
        for (int i = 0; i < indents; ++i) {
          os << '\t';
        }
      }
      os << "]";
    }
  }
};

} // namespace jsonio

#endif // JSONIO_SRC_JSON_ARRAY_HPP
