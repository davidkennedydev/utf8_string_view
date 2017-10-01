#include <iostream>

constexpr char Utf8ByteCompose(const int64_t unicode, size_t header_size) {
  unsigned char header = 0xfe << (8 - header_size);
  unsigned char payload = unicode & (0xff >> header_size);
  return header | payload;
}

constexpr char Utf8Encoding(const uint64_t codepoint, const size_t byte) {
  size_t code_units =
    codepoint < 0x80 ? 1 :
    codepoint < 0x800 ? 2 :
    codepoint < 0x10000 ? 3 : 4;

  if (byte > code_units)
    return 0;

  const uint64_t codepoint_cut = codepoint >> ((code_units - byte) * 6);

  size_t header_size = 2;

  if (byte == 1) {
    header_size = code_units;

    if (code_units > 1)
      ++header_size;
  }

  return Utf8ByteCompose(codepoint_cut, header_size);
}

#include <cstring>

struct char8_t {
  const char data[5];

  constexpr char8_t(const uint64_t unicode) :
    data {
      Utf8Encoding(unicode, 1),
      Utf8Encoding(unicode, 2),
      Utf8Encoding(unicode, 3),
      Utf8Encoding(unicode, 4),
      '\0'
    }
  {}

  bool operator==(const uint64_t unicode) const {
    const char data[5] {
      Utf8Encoding(unicode, 1),
      Utf8Encoding(unicode, 2),
      Utf8Encoding(unicode, 3),
      Utf8Encoding(unicode, 4),
      '\0'
    };
    return std::strcmp(this->data, data) == 0;
  }

  bool operator==(char8_t& ch) const {
    return std::strcmp(data, ch.data) == 0;
  }

  bool operator<(char8_t& ch) const {
    return std::strcmp(data, ch.data) < 0;
  }

  bool operator>(char8_t& ch) const {
    return std::strcmp(data, ch.data) > 0;
  }

  constexpr char8_t(const char* str) : data{str[0], str[1], str[2], str[3], '\0'} {}
};

std::ostream& operator <<(std::ostream& out, const char8_t& ch) {
  out << ch.data;
  return out;
}

#include <string_view>

template <typename T>
class base_utf8_string_iterator {
public:
  base_utf8_string_iterator (T& data, const size_t pos) : pos(pos), data(data) {}

  char8_t operator*();

  base_utf8_string_iterator& operator--();

  base_utf8_string_iterator operator--(int) {
    auto past = *this;
    --(*this);
    return past;
  }

  base_utf8_string_iterator& operator++();

  base_utf8_string_iterator operator++(int) {
    auto past = *this;
    ++(*this);
    return past;
  }

  base_utf8_string_iterator operator+(size_t i) const {
    auto it = *this;
    if (i > 0) 
      while(i--) ++it;
    else
      while(i++) --it;

    return it;
  }

  base_utf8_string_iterator operator-(size_t i) const {
    return (*this) + -i;
  }


  bool operator==(const base_utf8_string_iterator<T>& it) const {
    return this->pos == it.pos;
  }

  bool operator!=(const base_utf8_string_iterator<T>& it) const {
    return this->pos != it.pos;
  }

private:
  size_t pos;
  T& data;
};

typedef base_utf8_string_iterator<const std::string_view> const_ustring_iterator;

class utf8_string_view : public std::string_view {
  public:
    utf8_string_view(const char* str) : std::string_view(str) {
      code_points = CalculateLength(*this);
    }

    size_t length() const {
      return this->code_points;
    }

    static size_t CalculateLength(const std::string_view& str) {
      size_t code_points = 0;

      for (const char& character : str)
        if ((character & 0b11000000) != 0b10000000)
          ++code_points;
      return code_points;
    }

    const_ustring_iterator begin() const {
      return {*this, 0};
    }

    const_ustring_iterator end() const {
      return {*this, std::string_view::length()};
    }

    char8_t operator[](const size_t pos) const {
      return *(begin() + pos);
    }
  
    char8_t at(const size_t pos) const {
      if (pos >= code_points)
        throw std::out_of_range("");

      return (*this)[pos];
    }
  
  private:
    size_t code_points;
};

template <typename T>
char8_t base_utf8_string_iterator<T>::operator*() {
  if ((data[pos] & 0b10000000) == 0b00000000)
    return data[pos];
  const char character[] = {data[pos], data[pos + 1], data[pos + 2], data[pos + 3], '\0'};
  return character;
}

template <typename T>
base_utf8_string_iterator<T>& base_utf8_string_iterator<T>::operator++() {
  // offset by variable character lenght
  if ((data[pos] & 0b11100000) == 0b11000000)
    pos += 1;
  if ((data[pos] & 0b11110000) == 0b11100000)
    pos += 2;
  if ((data[pos] & 0b11111000) == 0b11110000)
    pos += 3;
  ++pos;

  return *this;
}

template <typename T>
base_utf8_string_iterator<T>& base_utf8_string_iterator<T>::operator--() {
  --pos;
  while((data[pos] & 0b11000000) == 0b10000000)
    --pos;

  return *this;
}
