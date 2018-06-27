#ifndef TGD_HEADER_BUFFER_HPP
#define TGD_HEADER_BUFFER_HPP

/*****************************************************************************

tgd_header - Encoding and decoding the Tiled Geographic Data Common Header.

This file is from https://github.com/mapbox/tgd-header-lib where you can find
more documentation.

*****************************************************************************/

/**
 * @file buffer.hpp
 *
 * @brief Contains the buffer class.
 */

#include <algorithm>
#include <array>
#include <cstdint>
#include <memory>

namespace tgd_header {

    /**
     * A mutable_buffer is created by specifying the size the buffer should
     * have. It can then be filled with data. After that it will be turned
     * into a (non-mutable) buffer by instantiating a buffer with the
     * mutable_buffer as the only parameter on the constructor.
     *
     * The size of a mutable_buffer can not be changed after construction.
     * If you need a resizable buffer, use a std::string for instance.
     */
    class mutable_buffer {

        friend class buffer;

        std::unique_ptr<char[]> m_data = nullptr;
        std::size_t m_size;

        void release() noexcept {
            m_data.release();
            m_size = 0;
        }

    public:

        /**
         * Construct buffer with specified size. This will allocate the
         * required memory on the heap using new.
         */
        explicit mutable_buffer(std::size_t size) :
            m_size(size) {
            m_data.reset(new char[size]); // NOLINT(modernize-make-unique) (not available in C++11)
        }

        /// Return a pointer to the data in the buffer.
        char* data() const noexcept {
            return m_data.get();
        }

        /// Return the size of the buffer.
        std::size_t size() const noexcept {
            return m_size;
        }

        /// Iterator pointing to the beginning of the data.
        char* begin() noexcept {
            return m_data.get();
        }

        /// Iterator pointing to the end of the data.
        char* end() noexcept {
            return m_data.get() + m_size;
        }

        /// Iterator pointing to the beginning of the data.
        const char* cbegin() const noexcept {
            return m_data.get();
        }

        /// Iterator pointing to the end of the data.
        const char* cend() const noexcept {
            return m_data.get() + m_size;
        }

    }; // class mutable_buffer

    /**
     * A buffer is a piece of memory at some location and with a specified
     * size. Buffers can exist in three forms:
     *
     * * The empty buffer doesn't contain any memory at all. It is created
     *   using the default constructor or by using the clear() function on
     *   any existing buffer.
     * * The buffer can point to some pre-existing memory range that some
     *   other part of this library or your program allocated and still
     *   manages. You have to make sure to keep this memory around as long
     *   as this buffer is still used.
     * * The buffer can allocate memory itself or take over the memory
     *   management for some memory you give it access to. In this case
     *   the buffer will release the memory (using delete[]) when the clear()
     *   function is called or when it is destructed.
     */
    class buffer {

        const char* m_data = nullptr;
        std::size_t m_size = 0;
        bool m_managed = false;

    public:

        /// Construct an empty buffer.
        buffer() noexcept = default;

        /**
         * Construct a buffer from a std::array<char, N>. You must take care
         * that the array is available as long as the buffer is used.
         */
        template <std::size_t N>
        explicit buffer(const std::array<char, N>& array) :
            m_data(array.data()),
            m_size(array.size()) {
        }

        /**
         * Construct a buffer from a mutable buffer. Management of the
         * memory will be taken over from the mutable_buffer.
         */
        explicit buffer(mutable_buffer&& mb) :
            m_data(mb.data()),
            m_size(mb.size()),
            m_managed(true) {
            mb.release();
        }

        /**
         * Construct a buffer pointing to existing memory. The memory is
         * not managed by the buffer.
         */
        explicit buffer(const char* data, std::size_t size) noexcept :
            m_data(data),
            m_size(size) {
        }

        /**
         * Construct a buffer pointing to existing memory. The memory is
         * managed by the buffer if the manage flag is true.
         */
        explicit buffer(const char* data, std::size_t size, bool manage) noexcept :
            m_data(data),
            m_size(size),
            m_managed(manage) {
        }

        /**
         * Buffers can't be copied, because they might be managing a ressource.
         * Use the copy() function if you need a copy.
         */
        buffer(const buffer&) = delete;
        buffer& operator=(const buffer&) = delete;

        /// Buffers can be moved.
        buffer(buffer&& other) noexcept :
            m_data(other.m_data),
            m_size(other.m_size),
            m_managed(other.m_managed) {
            other.m_data = nullptr;
            other.m_size = 0;
            other.m_managed = false;
        }

        /// Buffers can be moved.
        buffer& operator=(buffer&& other) noexcept {
            swap(other);
            return *this;
        }

        /**
         * Destruct the buffer. If the buffer content is managed, release
         * the memory.
         */
        ~buffer() noexcept {
            clear();
        }

        /**
         * Return a copy of this buffer.
         *
         * The resulting buffer is always managed and contains a copy of the
         * data in this buffer.
         */
        buffer copy() const {
            mutable_buffer mbuffer{size()};
            std::copy_n(data(), size(), mbuffer.data());
            return buffer{std::move(mbuffer)};
        }

        /**
         * Clear the contents of the buffer. If the buffer content is
         * managed, release the memory.
         */
        void clear() noexcept {
            if (m_managed) {
                delete[] m_data;
                m_managed = false;
            }
            m_data = nullptr;
            m_size = 0;
        }

        /// Swap the contents of this buffer with the specified buffer.
        void swap(buffer& other) noexcept {
            using std::swap;
            swap(m_data, other.m_data);
            swap(m_size, other.m_size);
            swap(m_managed, other.m_managed);
        }

        /// Does this buffer contain some data.
        explicit operator bool() const noexcept {
            return m_data != nullptr;
        }

        /// Return pointer to buffer contents.
        const char* data() const noexcept {
            return m_data;
        }

        /// Returns true if the buffer manages the memory allocation.
        bool managed() const noexcept {
            return m_managed;
        }

        /// Return the size of the buffer contents.
        std::size_t size() const noexcept {
            return m_size;
        }

        /// Iterator pointing to the beginning of the data.
        const char* cbegin() const noexcept {
            return m_data;
        }

        /// Iterator pointing to the end of the data.
        const char* cend() const noexcept {
            return m_data + size();
        }

        /// Iterator pointing to the beginning of the data.
        const char* begin() const noexcept {
            return cbegin();
        }

        /// Iterator pointing to the end of the data.
        const char* end() const noexcept {
            return cend();
        }

    }; // class buffer

    inline void swap(buffer& a, buffer& b) noexcept {
        a.swap(b);
    }

} // namespace tgd_header

#endif // TGD_HEADER_BUFFER_HPP
