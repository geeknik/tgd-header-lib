#ifndef TGD_HEADER_TILE_HPP
#define TGD_HEADER_TILE_HPP

/*****************************************************************************

tgd_header - Encoding and decoding the Tiled Geographic Data Common Header.

This file is from https://github.com/mapbox/tgd-header-lib where you can find
more documentation.

*****************************************************************************/

/**
 * @file tile.hpp
 *
 * @brief Contains the tile_address class.
 */

#include <tgd_header/encoding.hpp>
#include <tgd_header/exceptions.hpp>

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>

namespace tgd_header {

    class tile_address {

        std::uint32_t m_x = 0;
        std::uint32_t m_y = 0;
        std::uint8_t m_zoom = 0;

    public:

        constexpr tile_address() = default;

        constexpr explicit tile_address(std::uint8_t zoom, std::uint32_t x, std::uint32_t y) noexcept :
            m_x(x),
            m_y(y),
            m_zoom(zoom) {
        }

        explicit tile_address(const char* input) noexcept {
            detail::get(input + detail::offset::tile_zoom, &m_zoom);
            detail::get(input + detail::offset::tile_x, &m_x);
            detail::get(input + detail::offset::tile_y, &m_y);
        }

        constexpr std::uint8_t zoom() const noexcept {
            return m_zoom;
        }

        constexpr std::uint32_t x() const noexcept {
            return m_x;
        }

        constexpr std::uint32_t y() const noexcept {
            return m_y;
        }

        void serialize(std::array<char, detail::header_size>& header) const noexcept {
            detail::set(m_zoom, &header[detail::offset::tile_zoom]);
            detail::set(m_x, &header[detail::offset::tile_x]);
            detail::set(m_y, &header[detail::offset::tile_y]);
        }

    }; // class tile_address

    inline bool operator==(const tile_address& lhs, const tile_address& rhs) noexcept {
        return lhs.zoom() == rhs.zoom() && lhs.x() == rhs.x() && lhs.y() == rhs.y();
    }

    inline bool operator!=(const tile_address& lhs, const tile_address& rhs) noexcept {
        return !(lhs == rhs);
    }

} // namespace tgd_header

#endif // TGD_HEADER_TILE_HPP
