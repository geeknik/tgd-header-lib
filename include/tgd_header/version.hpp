#ifndef TGD_HEADER_VERSION_HPP
#define TGD_HEADER_VERSION_HPP

/*****************************************************************************

tgd_header - Encoding and decoding the Tiled Geographic Data Common Header.

This file is from https://github.com/mapbox/tgd-header-lib where you can find
more documentation.

*****************************************************************************/

/**
 * @file version.hpp
 *
 * @brief Contains the version number macros for the library.
 */

/// The major version number
#define TGD_HEADER_VERSION_MAJOR 0

/// The minor version number
#define TGD_HEADER_VERSION_MINOR 0

/// The patch number
#define TGD_HEADER_VERSION_PATCH 1

/// The complete version number
#define TGD_HEADER_VERSION_CODE                                      \
    (TGD_HEADER_VERSION_MAJOR * 10000 + TGDLIB_VERSION_MINOR * 100 + \
     TGD_HEADER_VERSION_PATCH)

/// Version number as string
#define TGD_HEADER_VERSION_STRING "0.0.1"

#endif // TGD_HEADER_VERSION_HPP
