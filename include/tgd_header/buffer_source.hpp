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

#include "buffer.hpp"

#include <cstddef>
#include <stdexcept>

namespace tgd_header {

    /**
     * Source for a tgd_header::reader based on a tgd_header::buffer. The
     * buffer is taken by reference, it is not allowed to change as long
     * as the buffer_source is used.
     *
     * Keeps track of where in the buffer we have been reading from.
     */
    class buffer_source {

        const buffer& m_data;
        std::size_t m_offset = 0;

    public:

        /// Construct buffer_source from a buffer.
        explicit buffer_source(const buffer& data) :
            m_data(data) {
        }

        /**
         * Read exactly len bytes from the source and return the results.
         * If there aren't len bytes left in the source, an empty buffer
         * is returned.
         */
        buffer read(const std::size_t len) {
            buffer buffer{m_data.data() + m_offset, len};
            if (m_offset + len > m_data.size()) {
                buffer.clear();
                return buffer;
            }
            m_offset += len;
            return buffer;
        }

        /**
         * Skip exactly len bytes from the source.
         *
         * @throws std::range_error If there aren't len bytes left in the
         *                          source
         */
        void skip(const std::size_t len) {
            if (m_offset + len > m_data.size()) {
                throw std::range_error{"Out of range"};
            }
            m_offset += len;
        }

    }; // buffer_source

} // namespace tgd_header

#endif // TGD_HEADER_BUFFER_SOURCE_HPP
