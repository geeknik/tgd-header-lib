
# Tiled Geographic Data Common Header Library

THIS IS WORK IN PROGRESS. EVERYTHING IN HERE IS SUBJECT TO CHANGE!

This library is part of a suite of libraries handling different aspects of
Tiled Geographic Data. It handles encoding and decoding of the common
header fields.

This is a C++11 header-only library. It should work with any modern C++
compiler.


## Use

Simply include the header files from the `include` directory.


## Dependencies

The [zlib](https://www.zlib.net/) library is needed. (Debian/Ubuntu users
install package `zlib1g-dev`, Fedora/CentOS/openSUSE users install
`zlib-devel`.)


## Tests

To build and run the tests:

```
mkdir build
cd build
cmake ..
make
ctest
```

## Examples

Some example programs are provided in the `examples` directory. They are built
together with the tests. See the beginning of those files for some usage
instructions.


## Author

Jochen Topf (jochen@topf.org)

