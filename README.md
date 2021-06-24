[![Build Status](https://travis-ci.com/eggman79/flag_ptr.svg?branch=main)](https://travis-ci.com/eggman79/flag_ptr)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/d1942e0dcae448959c9a7bc0da909d26)](https://www.codacy.com/gh/eggman79/flag_ptr/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=eggman79/flag_ptr&amp;utm_campaign=Badge_Grade)
# flag_ptr
very simple single header-only c++ library for simple reading or writing flags on the youngest unused bits of pointer.

## examples
```C++
#include <string>
#include <flag_ptr/flag_ptr.hpp>

int main(int argc, char** argv) {
  using namespace eggman79;
  enum Color {Red, Green, Blue};
  
  auto str = make_flag_ptr<std::string, flags<flag<Color, 2>, flag<bool, 1>>>("string");
  
  str.set_flag<0>(Color::Blue);
  str.set_flag<1>(true);
  
  if (*str == "string" && str.get_flag<0>() == Color::Blue && str.get_flag<1>()) {
    std::cout << "ok" << std::endl;
  }
}
```
