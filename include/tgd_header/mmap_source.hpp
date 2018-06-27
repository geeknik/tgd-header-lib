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

#include <cstddef>
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

    /**
     * Source for a tgd_header::reader based on a file. The file is opened
     * and mmaped on construction and unmapped and closed on destruction.
     *
     * Keeps track of where in the buffer we have been reading from.
     */
    class mmap_source : public detail::file {

        std::size_t m_size = 0;
        char* m_mapping = nullptr;
        std::size_t m_offset = 0;

    public:

        /**
         * Construct mmap_source from contents of the specified file.
         *
         * @param filename Name of the input file.
         */
        explicit mmap_source(const std::string& filename) :
            file(open_file(filename, O_RDONLY | O_CLOEXEC)), // NOLINT(hicpp-signed-bitwise)
            m_size(file_size()),
            m_mapping(static_cast<char*>(::mmap(nullptr, m_size, PROT_READ, MAP_PRIVATE, fd(), 0))) {
            if (!m_mapping) {
                throw std::system_error{errno, std::system_category(), std::string{"Error mmapping file '"} + filename + "': "};
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

        ~mmap_source() noexcept {
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

        buffer read(const std::size_t len) {
            buffer buffer{m_mapping + m_offset, len};
            if (m_offset + len > m_size) {
                buffer.clear();
                return buffer;
            }
            m_offset += len;
            return buffer;
        }

        void skip(const std::size_t len) {
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
