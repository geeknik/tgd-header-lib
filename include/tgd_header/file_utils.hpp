#ifndef TGD_HEADER_FILE_UTILS_HPP
#define TGD_HEADER_FILE_UTILS_HPP

/*****************************************************************************

tgd_header - Encoding and decoding the Tiled Geographic Data Common Header.

This file is from https://github.com/mapbox/tgd-header-lib where you can find
more documentation.

*****************************************************************************/

/**
 * @file file_utils.hpp
 *
 * @brief Contains utility functions for file access.
 */

#include <cstddef>
#include <sys/stat.h>
#include <sys/types.h>
#include <system_error>

#ifdef _WIN32
#  ifndef WIN32_LEAN_AND_MEAN
#   define WIN32_LEAN_AND_MEAN // Prevent winsock.h inclusion; avoid winsock2.h conflict
#  endif
# include <crtdbg.h>
# include <io.h>
# include <windows.h>
#endif

#ifndef _MSC_VER
# include <unistd.h>
#endif

namespace tgd_header {

    namespace detail {
#ifdef _MSC_VER
        // Disable parameter validation on Windows and reenable it
        // automatically when scope closes.
        // https://docs.microsoft.com/en-us/cpp/c-runtime-library/parameter-validation
        class disable_invalid_parameter_handler {

            static void invalid_parameter_handler(
                    const wchar_t* expression,
                    const wchar_t* function,
                    const wchar_t* file,
                    unsigned int line,
                    uintptr_t pReserved
                    ) {
                // do nothing
            }

            _invalid_parameter_handler old_handler;
            int old_report_mode;

        public:

            disable_invalid_parameter_handler() :
                old_handler(_set_thread_local_invalid_parameter_handler(invalid_parameter_handler)),
                old_report_mode(_CrtSetReportMode(_CRT_ASSERT, 0)) {
            }

            ~disable_invalid_parameter_handler() {
                _CrtSetReportMode(_CRT_ASSERT, old_report_mode);
                _set_thread_local_invalid_parameter_handler(old_handler);
            }

        }; // class disable_invalid_parameter_handler
#endif

        /**
         * Get file size.
         * This is a small wrapper around a system call.
         *
         * @param fd File descriptor
         * @returns file size
         * @throws std::system_error If system call failed
         */
        inline std::size_t file_size(int fd) {
#ifdef _MSC_VER
            // Windows implementation
            disable_invalid_parameter_handler diph;
            // https://msdn.microsoft.com/en-us/library/dfbc2kec.aspx
            const auto size = ::_filelengthi64(fd);
            if (size < 0) {
                throw std::system_error{errno, std::system_category(), "Could not get file size"};
            }
            return static_cast<std::size_t>(size);
#else
            // Unix implementation
            struct stat s; // NOLINT clang-tidy
            if (::fstat(fd, &s) != 0) {
                throw std::system_error{errno, std::system_category(), "Could not get file size"};
            }
            return static_cast<std::size_t>(s.st_size);
#endif
        }

    } // namespace detail

} // namespace tgd_header

#endif // TGD_HEADER_FILE_UTILS_HPP
