#ifndef TGD_HEADER_STREAM_HPP
#define TGD_HEADER_STREAM_HPP

/*****************************************************************************

tgd_header - Encoding and decoding the Tiled Geographic Data Common Header.

This file is from https://github.com/mapbox/tgd-header-lib where you can find
more documentation.

*****************************************************************************/

/**
 * @file stream.hpp
 *
 * @brief Contains stream output operators for basic types used in the library.
 */

#include "tile.hpp"
#include "types.hpp"

#include <ostream>

namespace tgd_header {

    /**
     * Write a layer_content_type to an output stream. For known types, the
     * name is written out, for unknown types, the numerical code.
     */
    template <typename TChar, typename TTraits>
    inline std::basic_ostream<TChar, TTraits>& operator<<(std::basic_ostream<TChar, TTraits>& out, layer_content_type content_type) {
        switch (content_type) {
            case layer_content_type::unknown:
                out << "unknown";
                break;
            case layer_content_type::vt2:
                out << "vt2";
                break;
            case layer_content_type::vt3:
                out << "vt3";
                break;
            case layer_content_type::raster:
                out << "raster";
                break;
            case layer_content_type::png:
                out << "png";
                break;
            case layer_content_type::jpeg:
                out << "jpeg";
                break;
            case layer_content_type::tiff:
                out << "tiff";
                break;
            default:
                out << '[' << static_cast<int>(content_type) << ']';
        }
        return out;
    }

    template <typename TChar, typename TTraits>
    inline std::basic_ostream<TChar, TTraits>& operator<<(std::basic_ostream<TChar, TTraits>& out, layer_compression_type compression_type) {
        switch (compression_type) {
            case layer_compression_type::uncompressed:
                out << "uncompressed";
                break;
            case layer_compression_type::zlib:
                out << "zlib";
                break;
            default:
                out << '[' << static_cast<int>(compression_type) << ']';
        }
        return out;
    }

    /**
     * Write a tile address in the format "zoom/x/y" to the stream.
     */
    template <typename TChar, typename TTraits>
    inline std::basic_ostream<TChar, TTraits>& operator<<(std::basic_ostream<TChar, TTraits>& out, const tile_address& tile) {
        return out << static_cast<int>(tile.zoom()) << '/' << tile.x() << '/' << tile.y();
    }

} // namespace tgd_header

#endif // TGD_HEADER_STREAM_HPP
