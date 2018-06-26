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

#include "buffer.hpp"
#include "file.hpp"

#include <cstdint>
#include <fcntl.h>
#include <stdexcept>
#include <string>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <system_error>
#include <unistd.h>
#include <utility>

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

        mmap_source(const mmap_source&) = delete;
        mmap_source& operator=(const mmap_source&) = delete;

        mmap_source(mmap_source&& other) noexcept :
            file(std::move(other)),
            m_size(other.m_size),
            m_mapping(other.m_mapping),
            m_offset(other.m_offset) {
            other.m_size = 0;
            other.m_mapping = nullptr;
        }

        mmap_source& operator=(mmap_source&& other) noexcept {
            swap(other);
            return *this;
        }

        ~mmap_source() {
            try {
                close();
            } catch (...) {
                // ignore errors so that the destructor can be noexcept
            }
        }

        /// Swap the contents of this mmap_source with the specified mmap_source.
        void swap(mmap_source& other) noexcept {
            using std::swap;
            swap(m_size, other.m_size);
            swap(m_mapping, other.m_mapping);
            swap(m_offset, other.m_offset);
        }

        void close() {
            if (m_mapping) {
                ::munmap(m_mapping, m_size);
            }
            file::close();
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

    inline void swap(mmap_source& a, mmap_source& b) noexcept {
        a.swap(b);
    }

} // namespace tgd_header

#endif // TGD_HEADER_MMAP_SOURCE_HPP
