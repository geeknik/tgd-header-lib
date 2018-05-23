#ifndef TGD_HEADER_CONFIG_HPP
#define TGD_HEADER_CONFIG_HPP

/*****************************************************************************

tgd_header - Encoding and decoding the Tiled Geographic Data Common Header.

This file is from https://github.com/mapbox/tgd-header-lib where you can find
more documentation.

*****************************************************************************/

#include <cassert>

/**
 * @file config.hpp
 *
 * @brief Contains macro checks for different configurations.
 */

// Wrapper for assert() used for testing
#ifndef tgd_header_assert
# define tgd_header_assert(x) assert(x)
#endif

#endif // TGD_HEADER_CONFIG_HPP
