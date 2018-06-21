#ifndef TGD_HEADER_MMAP_SOURCE_HPP
#define TGD_HEADER_MMAP_SOURCE_HPP

/*****************************************************************************

tgd_header - Encoding and decoding the Tiled Geographic Data Common Header.

This file is from https://github.com/mapbox/tgd-header-lib where you can find
more documentation.

*****************************************************************************/

/**
 * @file mmap_source.hpp
 *
 * @brief Contains the mmap_source class.
 */

#include <tgd_header/file.hpp>
#include <tgd_header/layer.hpp>

#include <fcntl.h>
#include <stdexcept>
#include <string>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <system_error>
#include <unistd.h>

namespace tgd_header {

    class mmap_source : public detail::file {

        std::size_t m_size = 0;
        char* m_mapping = nullptr;
        std::uint64_t m_offset = 0;

    public:

        explicit mmap_source(const std::string& filename) :
            file(filename, O_RDONLY | O_CLOEXEC) { // NOLINT(hicpp-signed-bitwise)
            m_size = file_size();
            m_mapping = static_cast<char*>(::mmap(nullptr, m_size, PROT_READ, MAP_PRIVATE, fd(), 0));
            if (!m_mapping) {
                std::runtime_error{std::string{"Can't mmap file: "} + filename};
            }
        }

        ~mmap_source() {
            if (m_mapping) {
                ::munmap(m_mapping, m_size);
            }
        }

        buffer read(const std::uint64_t len) {
            if (m_offset + len > m_size) {
                return buffer{};
            }
            buffer buffer{m_mapping + m_offset, len};
            m_offset += len;
            return buffer;
        }

        void skip(const std::uint64_t len) {
            if (m_offset + len > m_size) {
                throw std::range_error{"Out of range"};
            }
            m_offset += len;
        }

    }; // mmap_source

} // namespace tgd_header

#endif // TGD_HEADER_MMAP_SOURCE_HPP
