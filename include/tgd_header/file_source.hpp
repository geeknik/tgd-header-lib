#ifndef TGD_HEADER_FILE_SOURCE_HPP
#define TGD_HEADER_FILE_SOURCE_HPP

/*****************************************************************************

tgd_header - Encoding and decoding the Tiled Geographic Data Common Header.

This file is from https://github.com/mapbox/tgd-header-lib where you can find
more documentation.

*****************************************************************************/

/**
 * @file file_source.hpp
 *
 * @brief Contains the file_source class.
 */

#include <tgd_header/file_utils.hpp>
#include <tgd_header/layer.hpp>

#include <fcntl.h>
#include <stdexcept>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <system_error>
#include <unistd.h>

//#include <iostream>

namespace tgd_header {

    class file_source {

        int m_fd = -1;

    public:

        explicit file_source(const std::string& filename) :
            m_fd(::open(filename.c_str(), O_RDONLY)) {
            if (m_fd < 0) {
                std::runtime_error{"Can't open file"};
            }
        }

        ~file_source() {
            if (m_fd >= 0) {
                ::close(m_fd);
            }
        }

        std::size_t file_size() const noexcept {
            return detail::file_size(m_fd);
        }

        buffer read(const std::size_t len) {
            mutable_buffer mb{len};

            const auto read_length = ::read(m_fd, mb.data(), mb.size());

            buffer buffer{mb};
            if (read_length == 0) {
                buffer.clear();
                return buffer;
            }

            if (static_cast<std::size_t>(read_length) == len) {
                return buffer;
            }

            throw std::system_error{errno, std::system_category(), "Read error"};
        }

        void skip(const std::size_t len) {
            const auto result = ::lseek(m_fd, static_cast<off_t>(len), SEEK_CUR);
            if (result < 0) {
                throw std::system_error{errno, std::system_category(), "Seek error"};
            }
        }

    }; // file_source

} // namespace tgd_header

#endif // TGD_HEADER_FILE_SOURCE_HPP
