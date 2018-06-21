#ifndef TGD_HEADER_LAYER_HPP
#define TGD_HEADER_LAYER_HPP

/*****************************************************************************

tgd_header - Encoding and decoding the Tiled Geographic Data Common Header.

This file is from https://github.com/mapbox/tgd-header-lib where you can find
more documentation.

*****************************************************************************/

/**
 * @file layer.hpp
 *
 * @brief Contains the layer class.
 */

#include <tgd_header/buffer.hpp>
#include <tgd_header/encoding.hpp>
#include <tgd_header/exceptions.hpp>
#include <tgd_header/tile.hpp>
#include <tgd_header/types.hpp>

#include <zlib.h>

#include <algorithm>
#include <array>
#include <cassert>
#include <cstring>
#include <limits>
#include <string>
#include <type_traits>

namespace tgd_header {

    // customization point for different output schemes
    template <typename TOutput>
    void append_data(TOutput& output, const char* data, std::size_t size) {
        output.append(data, size);
    }

    class layer {

        // XXX make sure that when this is set, there is always a zero-byte
        // at the end.
        buffer m_name{};

        buffer m_content{};
        buffer m_wire_content{};

        // XXX can we make sure that these are not too long internally, so
        // the user doesn't have to?
        name_length_type m_name_length = 0;
        content_length_type m_content_length = 0;
        content_length_type m_wire_content_length = 0;

        tile_address m_tile{};

        layer_content_type m_content_type = layer_content_type::unknown;

        layer_compression_type m_compression_type = layer_compression_type::uncompressed;

        bool m_valid = false;

        static void check_magic(const char* data) {
            if (data[0] != 'T' ||
                data[1] != 'G' ||
                data[2] != 'D' ||
                data[3] != '0') {
                throw format_error{"magic error"};
            }
        }

        static constexpr const size_t max_name_length = 1000; // XXX needs to be decided in the specification

        // XXX shall we check that the content didn't get bigger and then use
        // uncompressed data instead?
        void encode_zlib() {
            assert(m_content.size() < std::numeric_limits<unsigned long>::max());
            unsigned long output_size = ::compressBound(static_cast<unsigned long>(m_content_length)); // NOLINT(google-runtime-int)

            mutable_buffer output{output_size};

            const auto result = ::compress(
                reinterpret_cast<unsigned char*>(output.data()), // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
                &output_size,
                reinterpret_cast<const unsigned char*>(m_content.data()), // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
                static_cast<unsigned long>(m_content_length) // NOLINT(google-runtime-int)
            );

            if (result != Z_OK) {
                throw zlib_error{std::string{"failed to compress data: "} + zError(result)};
            }

            m_wire_content_length = output_size;
            m_wire_content = buffer{output};
        }

        void decode_zlib() {
            unsigned long raw_size = m_content_length; // NOLINT(google-runtime-int)

            mutable_buffer mb{m_content_length};

            const auto result = ::uncompress(
                reinterpret_cast<unsigned char*>(mb.data()), // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
                &raw_size,
                reinterpret_cast<const unsigned char*>(m_wire_content.data()), // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
                m_wire_content.size()
            );

            if (result != Z_OK) {
                throw zlib_error{std::string{"failed to uncompress data: "} + zError(result)};
            }

            if (raw_size != m_content_length) {
                throw format_error{"wrong original size on compressed data"};
            }

            m_content = buffer{mb};
        }

        std::array<char, detail::header_size> serialize_header() {
            std::array<char, detail::header_size> header{{'T', 'G', 'D', '0'}};

            detail::set(m_content_type, &header[detail::offset::content_type]);
            detail::set(m_name_length, &header[detail::offset::name_length]);
            detail::set(m_compression_type, &header[detail::offset::compression_type]);
            m_tile.serialize(header);
            detail::set(m_content_length, &header[detail::offset::original_length]);
            detail::set(m_wire_content_length, &header[detail::offset::content_length]);

            return header;
        }

    public:

        layer() = default;

        layer(const char* data, std::uint64_t size) {
            if (size < detail::header_size) {
                throw format_error{"incomplete header"};
            }

            check_magic(data);

            detail::get(data + detail::offset::content_type, &m_content_type);

            m_tile = tile_address{data};

            detail::get(data + detail::offset::name_length, &m_name_length);
            if (m_name_length > max_name_length) {
                throw format_error{"name too long"};
            }

            detail::get(data + detail::offset::compression_type, &m_compression_type);
            detail::get(data + detail::offset::original_length, &m_content_length);
            detail::get(data + detail::offset::content_length, &m_wire_content_length);

            m_valid = true;
        }

        explicit layer(const buffer& buffer) :
            layer(buffer.data(), buffer.size()) {
        }

        explicit layer(const std::string& data) :
            layer(data.data(), data.size()) {
        }

        operator bool() const noexcept {
            return m_valid;
        }

        layer_content_type content_type() const noexcept {
            return m_content_type;
        }

        void set_content_type(layer_content_type content) noexcept {
            m_content_type = content;
        }

        layer_compression_type compression_type() const noexcept {
            return m_compression_type;
        }

        void set_compression_type(layer_compression_type compression) noexcept {
            m_compression_type = compression;
        }

        tile_address tile() const noexcept {
            return m_tile;
        }

        void set_tile(tile_address t) noexcept {
            m_tile = t;
        }

        name_length_type name_length() const noexcept {
            return m_name_length;
        }

        const buffer& name() const noexcept {
            return m_name;
        }

        void set_name_internal(buffer&& buffer) {
            m_name = std::move(buffer);
        }

        void set_name(buffer&& buffer) {
            m_name_length = static_cast<name_length_type>(buffer.size());
            m_name = std::move(buffer);
        }

        void set_name(const char* name, std::size_t length) {
            m_name_length = static_cast<name_length_type>(length);
            m_name = buffer{name, length};
        }

        void set_name(const char* name) {
            const auto length = std::strlen(name) + 1;
            if (length >= std::numeric_limits<name_length_type>::max()) {
                throw format_error{"name too long"};
            }
            set_name(name, length);
        }

        content_length_type content_length() const noexcept {
            return m_content_length;
        }

        const buffer& content() const noexcept {
            return m_content;
        }

        void set_content(buffer&& buffer) {
            m_content_length = static_cast<content_length_type>(buffer.size());
            m_content = std::move(buffer);
        }

        void set_content(const char* content, std::size_t length) {
            m_content_length = static_cast<content_length_type>(length);
            m_content = buffer{content, length};
        }

        void set_content(const char* content) {
            const auto length = std::strlen(content) + 1;
            if (length >= std::numeric_limits<content_length_type>::max()) {
                throw format_error{"content too long"};
            }
            set_content(content, length);
        }

        content_length_type wire_content_length() const noexcept {
            return m_wire_content_length;
        }

        const buffer& wire_content() const noexcept {
            return m_wire_content;
        }

        void set_wire_content(buffer&& buffer) {
            m_wire_content = std::move(buffer);
        }

        // XXX it should be possible to do this magically in the background
        // when needed.
        void encode_content() {
            if (m_content && !m_wire_content) {
                switch (m_compression_type) {
                    case layer_compression_type::uncompressed:
                        m_wire_content_length = m_content_length;
                        m_wire_content = buffer{m_content.data(), m_content.size()};
                        break;
                    case layer_compression_type::zlib:
                        encode_zlib();
                        break;
                    default:
                        throw format_error{"Unknown compression type (" + std::to_string(static_cast<int>(m_compression_type)) + ")"};
                }
            }
        }

        // XXX it should be possible to do this magically in the background
        // when needed.
        void decode_content() {
            if (m_wire_content && !m_content) {
                switch (m_compression_type) {
                    case layer_compression_type::uncompressed:
                        m_content = buffer{m_wire_content.data(), m_wire_content_length};
                        break;
                    case layer_compression_type::zlib:
                        decode_zlib();
                        break;
                    default:
                        throw format_error{"Unknown compression type (" + std::to_string(static_cast<int>(m_compression_type)) + ")"};
                }
            }
        }

        template <typename TSink>
        std::size_t write(TSink& sink) {
            encode_content();

            const auto header = serialize_header();
            sink.write(buffer{header});

            sink.write(m_name);
            sink.padding(detail::padding(m_name.size()));

            sink.write(m_wire_content);
            sink.padding(detail::padding(m_wire_content.size()));

            return detail::header_size +
                   detail::padded_size(m_name.size()) +
                   detail::padded_size(m_content.size());
        }

    }; // class layer

} // namespace tgd_header

#endif // TGD_HEADER_LAYER_HPP
