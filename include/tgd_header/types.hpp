#ifndef TGD_HEADER_TYPES_HPP
#define TGD_HEADER_TYPES_HPP

/*****************************************************************************

tgd_header - Encoding and decoding the Tiled Geographic Data Common Header.

This file is from https://github.com/mapbox/tgd-header-lib where you can find
more documentation.

*****************************************************************************/

/**
 * @file types.hpp
 *
 * @brief Contains basic types used in the library.
 */

#include <cstdint>

namespace tgd_header {

    enum class layer_content_type : std::uint16_t {

        unknown = 0x00,

        vt2     = 0x02,
        vt3     = 0x03,

        raster  = 0x10,
        png     = 0x11,
        jpeg    = 0x12,
        tiff    = 0x13,

        other   = 0xffff

    }; // enum class layer_content_type

    enum class layer_compression_type : std::uint8_t {
        uncompressed = 0,
        zlib         = 1,
        other        = 0xff
    }; // enum class layer_compression_type

    using name_length_type = std::uint16_t;

    using content_length_type = std::uint64_t;

} // namespace tgd_header

#endif // TGD_HEADER_TYPES_HPP
