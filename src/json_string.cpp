#include "json_string.h"

jsonio::json_string::json_string() noexcept : flags_{PHASE_START} {}

jsonio::json_string::json_string(const std::string &text) noexcept {
  *this = text;
}

jsonio::json_string::json_string(std::string &&text) noexcept {
  *this = std::move(text);
}

jsonio::json_string::json_string(const char *text) noexcept { *this = text; }

jsonio::json_string::json_string(std::string_view text) noexcept {
  *this = text;
}

jsonio::json_string::json_string(const jsonio::json_string &source) noexcept {
  *this = source;
}

jsonio::json_string::json_string(jsonio::json_string &&source) noexcept {
  *this = std::move(source);
}

jsonio::json_string &
jsonio::json_string::operator=(const jsonio::json_string &source) noexcept {
  if (this != &source) {
    std::string::operator=(*(std::string *)&source);
    flags_ = source.flags_;
  }
  return *this;
}

jsonio::json_string &
jsonio::json_string::operator=(jsonio::json_string &&source) noexcept {
  if (this != &source) {
    std::string::operator=(std::move(*(std::string *)&source));
    flags_ = source.flags_;
    source.flags_ = PHASE_START;
  }
  return *this;
}

jsonio::json_string &
jsonio::json_string::operator=(const std::string &text) noexcept {
  flags_ = PHASE_COMPLETED;
  std::string::operator=(text);
  check_escape();
  return *this;
}

jsonio::json_string &
jsonio::json_string::operator=(std::string &&text) noexcept {
  flags_ = PHASE_COMPLETED;
  std::string::operator=(std::move(text));
  check_escape();
  return *this;
}

jsonio::json_string &jsonio::json_string::operator=(const char *text) noexcept {
  flags_ = PHASE_COMPLETED;
  std::string::operator=(text);
  check_escape();
  return *this;
}

jsonio::json_string &
jsonio::json_string::operator=(std::string_view text) noexcept {
  flags_ = PHASE_COMPLETED;
  std::string::operator=(text);
  check_escape();
  return *this;
}

jsonio::json_string::~json_string() noexcept {}

void jsonio::json_string::read(std::istream &is) {
  if ((flags_ & PHASE_COMPLETED) == PHASE_COMPLETED) {
    flags_ = PHASE_START;
  }
  if ((flags_ & PHASE_COMPLETED) == PHASE_START) {
    flags_ &= ~PHASE_COMPLETED;
    flags_ |= PHASE_TEXT;
    std::string::clear();
  }
  if ((flags_ & PHASE_COMPLETED) == PHASE_TEXT) {
    char source;
    while (is.get(source)) {
      if (flags_ & ESCAPING) {
        source = Unescape(source);
        if (source != '\0') {
          flags_ &= ~ESCAPING;
          flags_ |= ESCAPED;
          append(1, source);
        } else {
          is.setstate(std::ios::iostate::_S_badbit);
          break;
        }
      } else if (source != '\"') {
        if (source != '\\') {
          append(1, source);
        } else {
          flags_ |= ESCAPING;
        }
      } else {
        flags_ |= PHASE_COMPLETED;
        break;
      }
    }
  }
}

void jsonio::json_string::write(std::ostream &os) const {
  if (completed()) {
    if (flags_ & ESCAPED) {
      for (const char source : *this) {
        if (!os.good()) {
          break;
        }
        char escaped_source = Escape(source);
        if (escaped_source == '\0') {
          os << source;
        } else {
          os << '\\';
          os << escaped_source;
        }
      }
    } else {
      os << *((std::string *)this);
    }
  }
}

bool jsonio::json_string::completed() const {
  return (flags_ & PHASE_COMPLETED) == PHASE_COMPLETED;
}

void jsonio::json_string::check_escape() {
  for (auto source : *(std::string *)this) {
    if (Escape(source) != '\0') {
      flags_ |= ESCAPED;
      break;
    }
  }
}

char jsonio::json_string::Unescape(const char source) {
  switch (source) {
  case 'b':
    return '\b';
    break;
  case 'f':
    return '\f';
    break;
  case 'n':
    return '\n';
    break;
  case 'r':
    return '\r';
    break;
  case 't':
    return '\t';
    break;
  case '\"':
    return '\"';
    break;
  case '\\':
    return '\\';
    break;
  }
  return '\0';
}

char jsonio::json_string::Escape(const char source) {
  switch (source) {
  case '\b':
    return 'b';
    break;
  case '\f':
    return 'f';
    break;
  case '\n':
    return 'n';
    break;
  case '\r':
    return 'r';
    break;
  case '\t':
    return 't';
    break;
  case '\"':
    return '\"';
    break;
  case '\\':
    return '\\';
    break;
  }
  return '\0';
}
