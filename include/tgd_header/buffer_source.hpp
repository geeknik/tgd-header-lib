#ifndef TGD_HEADER_BUFFER_SOURCE_HPP
#define TGD_HEADER_BUFFER_SOURCE_HPP

/*****************************************************************************

tgd_header - Encoding and decoding the Tiled Geographic Data Common Header.

This file is from https://github.com/mapbox/tgd-header-lib where you can find
more documentation.

*****************************************************************************/

/**
 * @file buffer_source.hpp
 *
 * @brief Contains the buffer_source class.
 */

#include <tgd_header/buffer.hpp>
#include <tgd_header/layer.hpp>

namespace tgd_header {

    class buffer_source {

        const buffer& m_data;
        std::uint64_t m_offset = 0;

    public:

        explicit buffer_source(const buffer& data) :
            m_data(data) {
        }

        buffer read(const std::uint64_t len) {
            if (m_offset + len > m_data.size()) {
                return buffer{};
            }
            buffer buffer{m_data.data() + m_offset, len};
            m_offset += len;
            return buffer;
        }

        void skip(const std::uint64_t len) {
            if (m_offset + len > m_data.size()) {
                throw std::range_error{"Out of range"};
            }
            m_offset += len;
        }

    }; // buffer_source

} // namespace tgd_header

#endif // TGD_HEADER_BUFFER_SOURCE_HPP
