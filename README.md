# flag_ptr
single header-only c++ library for simple reading or writing flags on the youngest unused bits of pointer.

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
