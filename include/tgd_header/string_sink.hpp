#ifndef TGD_HEADER_STRING_SINK_HPP
#define TGD_HEADER_STRING_SINK_HPP

/*****************************************************************************

tgd_header - Encoding and decoding the Tiled Geographic Data Common Header.

This file is from https://github.com/mapbox/tgd-header-lib where you can find
more documentation.

*****************************************************************************/

/**
 * @file string_sink.hpp
 *
 * @brief Contains the string_sink class.
 */

#include "buffer.hpp"

#include <cstdint>
#include <string>

namespace tgd_header {

    class string_sink {

        std::string& m_data;

    public:

        explicit string_sink(std::string& data) :
            m_data(data) {
        }

        void write(const buffer& buffer) {
            m_data.append(buffer.data(), buffer.size());
        }

        void padding(std::size_t size) {
            m_data.append(size, '\0');
        }

    }; // string_sink

} // namespace tgd_header

#endif // TGD_HEADER_STRING_SINK_HPP
