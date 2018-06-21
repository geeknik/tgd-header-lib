#ifndef TGD_HEADER_EXCEPTIONS_HPP
#define TGD_HEADER_EXCEPTIONS_HPP

/*****************************************************************************

tgd_header - Encoding and decoding the Tiled Geographic Data Common Header.

This file is from https://github.com/mapbox/tgd-header-lib where you can find
more documentation.

*****************************************************************************/

/**
 * @file exceptions.hpp
 *
 * @brief Contains the exceptions used in this library.
 */

#include <stdexcept>

namespace tgd_header {

    /// Errors related to the common header format.
    struct format_error : public std::runtime_error {

        explicit format_error(const char* what) :
            runtime_error(what) {
        }

        explicit format_error(const std::string& what) :
            runtime_error(what) {
        }

    }; // format_error

    /// Errors originating in the zlib library.
    struct zlib_error : public std::runtime_error {

        explicit zlib_error(const std::string& what) :
            runtime_error(what) {
        }

    }; // zlib_error

} // namespace tgd_header

#endif // TGD_HEADER_EXCEPTIONS_HPP
