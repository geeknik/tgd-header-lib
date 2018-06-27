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

#include "buffer.hpp"
#include "file.hpp"

#include <cstddef>
#include <fcntl.h>
#include <stdexcept>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <system_error>
#include <unistd.h>

namespace tgd_header {

    /**
     * Source for a tgd_header::reader based on a file. The file is opened
     * on construction and closed on destruction (unless the filename is
     * empty or "-" in which case STDIN is used).
     *
     * Keeps track of where in the buffer we have been reading from.
     */
    class file_source : public detail::file {

        static int open_file_or_stdin(const std::string& filename) {
            if (filename.empty() || filename == "-") {
                return 0;
            }

            return open_file(filename, O_RDONLY | O_CLOEXEC); // NOLINT (hicpp-signed-bitwise)
        }

    public:

        /**
         * Construct file_source from contents of the specified file.
         *
         * @param filename Name of the input file. If empty or "-", STDIN
         *                 is used.
         */
        explicit file_source(const std::string& filename) :
            file(open_file_or_stdin(filename)) {
        }

        /**
         * Read exactly len bytes from the source and return the results.
         * If there aren't len bytes left in the source, an empty buffer
         * is returned.
         */
        buffer read(const std::size_t len) const {
            mutable_buffer mb{len};

            const auto read_length = ::read(fd(), mb.data(), mb.size());

            buffer buffer{std::move(mb)};
            if (read_length == 0) {
                buffer.clear();
                return buffer;
            }

            if (static_cast<std::size_t>(read_length) == len) {
                return buffer;
            }

            throw std::system_error{errno, std::system_category(), "Read error"};
        }

        void skip(const std::size_t len) const {
            const auto result = ::lseek(fd(), static_cast<off_t>(len), SEEK_CUR);
            if (result < 0) {
                throw std::system_error{errno, std::system_category(), "Seek error"};
            }
        }

    }; // file_source

} // namespace tgd_header

#endif // TGD_HEADER_FILE_SOURCE_HPP
