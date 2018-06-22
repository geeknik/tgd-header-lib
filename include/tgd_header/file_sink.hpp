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
#include <fcntl.h>
#include <stdexcept>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <system_error>
#include <unistd.h>

namespace tgd_header {

    class file_sink : public detail::file {

        static int open_file(const std::string& filename) {
            if (filename.empty() || filename == "-") {
                return 1;
            }
            return ::open(filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC, 0644); // NOLINT (cppcoreguidelines-pro-type-vararg,hicpp-signed-bitwise,hicpp-vararg)
        }

    public:

        explicit file_sink(const std::string& filename) :
            file(open_file(filename)) {
        }

        void write(const buffer& buffer) {
            const auto write_length = ::write(fd(), buffer.data(), buffer.size());

            if (static_cast<std::uint64_t>(write_length) != buffer.size()) {
                throw std::system_error{errno, std::system_category(), "Write error"};
            }
        }

        void padding(std::uint64_t size) {
            assert(size < detail::align_bytes);

            static const char pad[detail::align_bytes] = {0};

            const auto write_length = ::write(fd(), pad, size);

            if (static_cast<std::uint64_t>(write_length) != size) {
                throw std::system_error{errno, std::system_category(), "Write error"};
            }
        }

    }; // file_sink

} // namespace tgd_header

#endif // TGD_HEADER_FILE_SINK_HPP
