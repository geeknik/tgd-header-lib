#ifndef TGD_HEADER_FILE_HPP
#define TGD_HEADER_FILE_HPP

/*****************************************************************************

tgd_header - Encoding and decoding the Tiled Geographic Data Common Header.

This file is from https://github.com/mapbox/tgd-header-lib where you can find
more documentation.

*****************************************************************************/

/**
 * @file file.hpp
 *
 * @brief Contains the file class.
 */

#include <tgd_header/file_utils.hpp>

#include <fcntl.h>
#include <stdexcept>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <system_error>
#include <unistd.h>

namespace tgd_header {

    namespace detail {

        class file {

            int m_fd = -1;

        public:

            explicit file(const char* name, int flags) :
                m_fd(::open(name, flags)) { // NOLINT(cppcoreguidelines-pro-type-vararg,hicpp-vararg)
                if (m_fd < 0) {
                    std::runtime_error{std::string{"Can't open file: "} + name};
                }
            }

            explicit file(const std::string& name, int flags) :
                file(name.c_str(), flags) {
            }

            explicit file(int fd) :
                m_fd(fd) {
                if (m_fd < 0) {
                    std::runtime_error{"Can't open file"};
                }
            }

            file(const file&) = delete;
            const file& operator=(const file&) = delete;

            file(file&& other) :
                m_fd(other.m_fd) {
                other.m_fd = -1;
            }

            const file& operator=(file&& other) {
                m_fd = other.m_fd;
                other.m_fd = -1;
                return *this;
            }

            ~file() noexcept {
                if (m_fd >= 2) {
                    ::close(m_fd);
                }
            }

            std::size_t file_size() const noexcept {
                return detail::file_size(m_fd);
            }

            int fd() const noexcept {
                return m_fd;
            }

        }; // class file

    } // namespace detail

} // namespace tgd_header

#endif // TGD_HEADER_FILE_HPP
