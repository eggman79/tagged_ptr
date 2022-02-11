[![Build Status](https://travis-ci.com/eggman79/flag_ptr.svg?branch=main)](https://travis-ci.com/eggman79/flag_ptr)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/d1942e0dcae448959c9a7bc0da909d26)](https://www.codacy.com/gh/eggman79/flag_ptr/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=eggman79/flag_ptr&amp;utm_campaign=Badge_Grade)
# flag_ptr
very simple single header-only c++ library for simple reading or writing flags on the youngest unused bits of pointer.

## Examples
```C++
#include <string>
#include <flag_ptr/flag_ptr.hpp>

int main(int argc, char** argv) {
  using namespace eggman79;
  enum class Color {Red, Green, Blue};
  
  auto str = make_flag_ptr<std::string, flags<flag<Color, 2>, flag<bool, 1>>>("string");
  
  str.set_flag<0>(Color::Blue);
  str.set_flag<1>(true);
  
  if (*str == "string" && str.get_flag<0>() == Color::Blue && str.get_flag<1>()) {
    std::cout << "ok" << std::endl;
  }
}
```
## Testing
```bash
git clone https://github.com/eggman79/flag_ptr
cd flag_ptr
git submodule update --init
mkdir build
cd build
cmake .. -DFLAG_PTR_BUILD_TESTS
make
make test
```

## Integration

[`flag_ptr.hpp`](https://github.com/eggman79/flag_ptr/blob/main/include/flag_ptr/flag_ptr.hpp) is the single required file. You need to add

```cpp
#include <flag_ptr/flag_ptr.hpp>

using namespace eggman79;
```

### CMake

You can also use the `flag_ptr::flag_ptr` interface target in CMake.  This target populates the appropriate usage requirements for `INTERFACE_INCLUDE_DIRECTORIES` to point to the appropriate include directories and `INTERFACE_COMPILE_FEATURES` for the necessary C++20 flags.

#### External
Build and install library:

```bash
git clone https://github.com/eggman79/flag_ptr
cd flag_ptr
mkdir build
cd build
cmake ..
make 
make install # if you are not su then: sudo make install
```

To use this library from a CMake project, you can locate it directly with `find_package()` and use the namespaced imported target from the generated package configuration:

```cmake
# CMakeLists.txt
find_package(flag_ptr REQUIRED)
...
add_library(foo ...)
...
target_link_libraries(foo PRIVATE flag_ptr::flag_ptr)
```


#### Embedded

To embed the library directly into an existing CMake project:

```bash
mkdir thirdparty # or another arbitrary folder name
git submodule add https://github.com/eggman79/flag_ptr thirdparty/flag_ptr

```
Call add_subdirectory() and target_link_libraries() in your CMakeLists.txt file:

```cmake

add_subdirectory(thirdparty/flag_ptr)
...
add_library(foo ...)
...
target_link_libraries(foo PRIVATE flag_ptr::flag_ptr)
```

