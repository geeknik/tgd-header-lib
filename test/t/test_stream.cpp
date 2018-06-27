#include <catch.hpp>

#include <tgd_header/stream.hpp>

#include <sstream>

TEST_CASE("Output content type") {
    const auto lct = tgd_header::layer_content_type::vt2;
    std::stringstream ss;
    ss << lct;
    REQUIRE(ss.str() == "vt2");
}

TEST_CASE("Output unknown content type") {
    const auto lct = static_cast<tgd_header::layer_content_type>(99);
    std::stringstream ss;
    ss << lct;
    REQUIRE(ss.str() == "[99]");
}

TEST_CASE("Output compression type: uncompressed") {
    const auto lct = tgd_header::layer_compression_type::uncompressed;
    std::stringstream ss;
    ss << lct;
    REQUIRE(ss.str() == "uncompressed");
}

TEST_CASE("Output compression type: zlib") {
    const auto lct = tgd_header::layer_compression_type::zlib;
    std::stringstream ss;
    ss << lct;
    REQUIRE(ss.str() == "zlib");
}

TEST_CASE("Output compression type: unknown") {
    const auto lct = static_cast<tgd_header::layer_compression_type>(42);
    std::stringstream ss;
    ss << lct;
    REQUIRE(ss.str() == "[42]");
}

