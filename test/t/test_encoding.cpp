
#include <catch.hpp>

#include <tgd_header/encoding.hpp>
#include <tgd_header/tile.hpp>
#include <tgd_header/types.hpp>

static_assert(tgd_header::detail::offset::name_length % sizeof(tgd_header::name_length_type) == 0, "name_length field must be aligned properly");
static_assert(tgd_header::detail::offset::content_type % sizeof(tgd_header::layer_content_type) == 0, "content_type field must be aligned properly");
static_assert(tgd_header::detail::offset::compression_type % sizeof(tgd_header::layer_compression_type) == 0, "compression_type field must be aligned properly");
static_assert(tgd_header::detail::offset::original_length % sizeof(tgd_header::content_length_type) == 0, "original_length field must be aligned properly");
static_assert(tgd_header::detail::offset::content_length % sizeof(tgd_header::content_length_type) == 0, "content_length field must be aligned properly");

TEST_CASE("padding") {
    REQUIRE(tgd_header::detail::padding(0) == 0);
    REQUIRE(tgd_header::detail::padding(1) == 7);
    REQUIRE(tgd_header::detail::padding(2) == 6);
    REQUIRE(tgd_header::detail::padding(3) == 5);
    REQUIRE(tgd_header::detail::padding(4) == 4);
    REQUIRE(tgd_header::detail::padding(5) == 3);
    REQUIRE(tgd_header::detail::padding(6) == 2);
    REQUIRE(tgd_header::detail::padding(7) == 1);
    REQUIRE(tgd_header::detail::padding(8) == 0);
    REQUIRE(tgd_header::detail::padding(9) == 7);
}

TEST_CASE("padded_size") {
    REQUIRE(tgd_header::detail::padded_size(0) ==  0);
    REQUIRE(tgd_header::detail::padded_size(1) ==  8);
    REQUIRE(tgd_header::detail::padded_size(2) ==  8);
    REQUIRE(tgd_header::detail::padded_size(3) ==  8);
    REQUIRE(tgd_header::detail::padded_size(4) ==  8);
    REQUIRE(tgd_header::detail::padded_size(5) ==  8);
    REQUIRE(tgd_header::detail::padded_size(6) ==  8);
    REQUIRE(tgd_header::detail::padded_size(7) ==  8);
    REQUIRE(tgd_header::detail::padded_size(8) ==  8);
    REQUIRE(tgd_header::detail::padded_size(9) == 16);
}

