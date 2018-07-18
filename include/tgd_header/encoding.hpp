#ifndef TGD_HEADER_ENCODING_HPP
#define TGD_HEADER_ENCODING_HPP

/*****************************************************************************

tgd_header - Encoding and decoding the Tiled Geographic Data Common Header.

This file is from https://github.com/mapbox/tgd-header-lib where you can find
more documentation.

*****************************************************************************/

/**
 * @file encoding.hpp
 *
 * @brief Contains helper functions for low-level encoding.
 */

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <type_traits>

#define TGD_HEADER_LITTLE_ENDIAN 1234
#define TGD_HEADER_BIG_ENDIAN    4321

// Find out which byte order the machine has.
#if defined(__BYTE_ORDER)
# if (__BYTE_ORDER == __LITTLE_ENDIAN)
#  define TGD_HEADER_BYTE_ORDER TGD_HEADER_LITTLE_ENDIAN
# endif
# if (__BYTE_ORDER == __BIG_ENDIAN)
#  define TGD_HEADER_BYTE_ORDER TGD_HEADER_BIG_ENDIAN
# endif
#else
// This probably isn't a very good default, but might do until we figure
// out something better.
# define TGD_HEADER_BYTE_ORDER TGD_HEADER_LITTLE_ENDIAN
#endif

// Check whether __builtin_bswap is available
#if defined(__GNUC__) || defined(__clang__)
# define TGD_HEADER_USE_BUILTIN_BSWAP
#endif

namespace tgd_header {

    namespace detail {

        inline uint16_t byteswap_impl(uint16_t value) noexcept {
#ifdef TGD_HEADER_USE_BUILTIN_BSWAP
            return __builtin_bswap16(value);
#else
            return ((value & 0xff00) >>  8) |
                   ((value & 0x00ff) <<  8)
#endif
        }

        inline uint32_t byteswap_impl(uint32_t value) noexcept {
#ifdef TGD_HEADER_USE_BUILTIN_BSWAP
            return __builtin_bswap32(value);
#else
            return ((value & 0xff000000) >> 24) |
                   ((value & 0x00ff0000) >>  8) |
                   ((value & 0x0000ff00) <<  8) |
                   ((value & 0x000000ff) << 24);
#endif
        }

        inline uint64_t byteswap_impl(uint64_t value) noexcept {
#ifdef TGD_HEADER_USE_BUILTIN_BSWAP
            return __builtin_bswap64(value);
#else
            return ((value & 0xff00000000000000ULL) >> 56) |
                   ((value & 0x00ff000000000000ULL) >> 40) |
                   ((value & 0x0000ff0000000000ULL) >> 24) |
                   ((value & 0x000000ff00000000ULL) >>  8) |
                   ((value & 0x00000000ff000000ULL) <<  8) |
                   ((value & 0x0000000000ff0000ULL) << 24) |
                   ((value & 0x000000000000ff00ULL) << 40) |
                   ((value & 0x00000000000000ffULL) << 56);
#endif
        }

        inline void byteswap_inplace(uint8_t* /*ptr*/) noexcept {
            // intentionally left blank
        }

        inline void byteswap_inplace(uint16_t* ptr) noexcept {
            *ptr = byteswap_impl(*ptr);
        }

        inline void byteswap_inplace(uint32_t* ptr) noexcept {
            *ptr = byteswap_impl(*ptr);
        }

        inline void byteswap_inplace(uint64_t* ptr) noexcept {
            *ptr = byteswap_impl(*ptr);
        }

        inline void byteswap_inplace(int32_t* ptr) noexcept {
            auto bptr = reinterpret_cast<uint32_t*>(ptr); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
            *bptr = byteswap_impl(*bptr);
        }

        inline void byteswap_inplace(int64_t* ptr) noexcept {
            auto bptr = reinterpret_cast<uint64_t*>(ptr); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
            *bptr = byteswap_impl(*bptr);
        }

        template <typename T, typename = typename std::enable_if<std::is_enum<T>::value>::type>
        void byteswap_inplace(T* ptr) noexcept {
            using ut = typename std::underlying_type<T>::type;
            byteswap_inplace(reinterpret_cast<ut*>(ptr)); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
        }

        constexpr const std::uint64_t align_bytes = 8;

        inline constexpr std::uint64_t padding(uint64_t size) noexcept {
            return (align_bytes - (size % align_bytes)) % align_bytes;
        }

        inline constexpr std::uint64_t padded_size(std::uint64_t size) noexcept {
            return (size + align_bytes - 1) & ~(align_bytes - 1);
        }

        /**
         * Read a value from the input and store it in the output variable.
         *
         * @tparam T Type of the value.
         * @param input Pointer into a buffer where the input data is. There
         *              must be at least sizeof(T) bytes in that buffer.
         * @param value Pointer to the location where the value should be stored.
         *
         * If necessary the byte order is taken into account.
         */
        template <typename T>
        void get(const char* input, T* value) noexcept {
            std::copy_n(input, sizeof(T), reinterpret_cast<char*>(value)); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
#if TGD_HEADER_BYTE_ORDER != TGD_HEADER_LITTLE_ENDIAN
            detail::byteswap_inplace(value);
#endif
        }

        /**
         * Write a value to the output buffer.
         *
         * If necessary the byte order is taken into account.
         */
        template <typename T>
        void set(T value, char* output) noexcept {
#if TGD_HEADER_BYTE_ORDER != TGD_HEADER_LITTLE_ENDIAN
            detail::byteswap_inplace(&value);
#endif
            std::copy_n(reinterpret_cast<const char*>(&value), sizeof(value), output); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
        }

        // XXX this is preliminary and needs to be optimized
        enum offset : std::size_t {
            content_type     =  4,
            name_length      =  8,
            compression_type = 10,
            tile             = 11,
            tile_zoom        = 11,
            tile_x           = 12,
            tile_y           = 16,
            original_length  = 24,
            content_length   = 28,
            end              = 32
        };

        constexpr const std::uint64_t header_size = offset::end;

    } // end namespace detail

} // namespace tgd_header

#undef TGD_HEADER_USE_BUILTIN_BSWAP
#undef TGD_HEADER_BYTE_ORDER
#undef TGD_HEADER_LITTLE_ENDIAN
#undef TGD_HEADER_BIG_ENDIAN

#endif // TGD_HEADER_ENCODING_HPP
