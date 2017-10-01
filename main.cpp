#include <iostream>
#include <string_view>
#include "utf8.h"

template <typename T>
void printString(const T& data) {
  std::cout << "Length: " << data.length() << "\t\t Data: " << data << std::endl;

  std::cout << "Each character iteration (foreach): \t";
  for (const auto& character : data)
    std::cout << character << ' ';
  std::cout << std::endl;

  std::cout << "Each character iteration (for index): \t";
  for (size_t i = 0; i < data.length(); ++i)
    std::cout << data[i] << ' ';

  std::cout << "\n\n" << std::flush;
}

int main(void) {
  std::cout << "One byte character:   " << char8_t(0x61) << std::endl;
  std::cout << "Two byte character:   " << char8_t(0xe7) << std::endl;
  std::cout << "Three byte character: " << char8_t(0x229e) << std::endl;
  std::cout << "Four byte character:  " << char8_t(0x1f836) << std::endl;
  std::cout << std::endl;
  /*
    One byte character:   a
    Two byte character:   ç
    Three byte character: ⊞
    Four byte character:  🠶
  */

  constexpr auto raw_text = "1🠶3";

  std::cout << "Common string view: " << std::endl;

  std::string_view text = raw_text;
  printString(text);
  /*
    Length: 6		 Data: 1🠶3
    Each character iteration (foreach):    1 � � � � 3 
    Each character iteration (for index):  1 � � � � 3 
  */

  std::cout << "UTF-8 string view: " << std::endl;

  utf8_string_view text2 = raw_text;
  printString(text2);
  /*
    Length: 3		 Data: 1🠶3
    Each character iteration (foreach):    1 🠶 3
    Each character iteration (for index):  1 🠶 3
  */

}

