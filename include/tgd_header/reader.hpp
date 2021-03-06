#ifndef TGD_HEADER_READER_HPP
#define TGD_HEADER_READER_HPP

/*****************************************************************************

tgd_header - Encoding and decoding the Tiled Geographic Data Common Header.

This file is from https://github.com/mapbox/tgd-header-lib where you can find
more documentation.

*****************************************************************************/

/**
 * @file reader.hpp
 *
 * @brief Contains the reader class.
 */

#include "encoding.hpp"
#include "layer.hpp"

#include <cassert>

namespace tgd_header {

    template <typename TSource>
    class reader {

        TSource& m_source;
        layer m_layer{};
        bool m_content_is_read = false;

    public:

        explicit reader(TSource& source) :
            m_source(source) {
        }

        layer& next_layer() {
            if (m_layer && !m_content_is_read) {
                m_source.skip(detail::padded_size(m_layer.wire_content_length()));
            }
            m_content_is_read = false;
            const auto buffer = m_source.read(detail::header_size);

            if (buffer) {
                m_layer = layer{buffer};

                if (m_layer) {
                    m_layer.set_name_internal(m_source.read(detail::padded_size(m_layer.name_length() + 1)));
                }
            } else {
                m_layer = {};
            }

            return m_layer;
        }

        void read_content() {
            assert(m_layer && "You have to call next_layer() first");

            if (!m_content_is_read) {
                m_layer.set_wire_content(m_source.read(detail::padded_size(m_layer.wire_content_length())));
                m_content_is_read = true;
            }
        }

    }; // class reader

} // namespace tgd_header

#endif // TGD_HEADER_READER_HPP
