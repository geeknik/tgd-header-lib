#ifndef TGD_HEADER_FILE_SINK_HPP
#define TGD_HEADER_FILE_SINK_HPP

/*****************************************************************************

tgd_header - Encoding and decoding the Tiled Geographic Data Common Header.

This file is from https://github.com/mapbox/tgd-header-lib where you can find
more documentation.

*****************************************************************************/

/**
 * @file file_sink.hpp
 *
 * @brief Contains the file_sink class.
 */

#include "buffer.hpp"
#include "encoding.hpp"
#include "file.hpp"

#include <cassert>
#include <cstdint>
#include <cstring>
#include <fcntl.h>
#include <stdexcept>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <system_error>
#include <unistd.h>

namespace tgd_header {

    class file_sink : public detail::file {

        static int open_file_or_stdout(const std::string& filename) {
            if (filename.empty() || filename == "-") {
                return 1;
            }

            return open_file(filename, O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC, 0644); // NOLINT (hicpp-signed-bitwise)
        }

        void write_impl(const char* data, std::size_t size) const {
            const auto write_length = ::write(fd(), data, size);

            if (static_cast<std::uint64_t>(write_length) != size) {
                throw std::system_error{errno, std::system_category(), std::string{"Error writing to file: "} + std::strerror(errno)};
            }
        }

    public:

        explicit file_sink(const std::string& filename) :
            file(open_file_or_stdout(filename)) {
        }

        void write(const buffer& buffer) const {
            write_impl(buffer.data(), buffer.size());
        }

        void padding(std::size_t size) const {
            assert(size < detail::align_bytes);

            static const char pad[detail::align_bytes] = {0};

            write_impl(pad, size);
        }

    }; // file_sink

} // namespace tgd_header

#endif // TGD_HEADER_FILE_SINK_HPP
