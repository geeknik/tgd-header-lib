
#include <catch.hpp>

#include <tgd_header/buffer.hpp>
#include <tgd_header/buffer_source.hpp>
#include <tgd_header/layer.hpp>
#include <tgd_header/reader.hpp>
#include <tgd_header/string_sink.hpp>
#include <tgd_header/tile.hpp>
#include <tgd_header/types.hpp>

#include <array>
#include <cstring>
#include <string>

TEST_CASE("Encode and decode layer") {
    const char* name = "test";
    const char* content = "the quick brown fox jumps over the lazy dog";

    tgd_header::layer_compression_type ct;

    SECTION("without compression") {
        ct = tgd_header::layer_compression_type::uncompressed;
    }

    SECTION("with compression") {
        ct = tgd_header::layer_compression_type::zlib;
    }

    // set up layer
    tgd_header::layer layer;
    layer.set_content_type(tgd_header::layer_content_type::unknown);
    layer.set_compression_type(ct);
    layer.set_tile(tgd_header::tile_address{4, 3, 2});
    layer.set_name(name);
    layer.set_content(content, std::strlen(content) + 1);

    // encode layer
    std::string out;
    tgd_header::string_sink sink{out};
    layer.write(sink);

    // check encoded layer
    REQUIRE(out.size() >= 20 + std::strlen(name) + std::strlen(content));
    REQUIRE(!std::strncmp(out.data(), "TGD0", 4));

    // decode layer again and check it
    tgd_header::buffer b{out.data(), out.size()};
    tgd_header::buffer_source source{b};
    tgd_header::reader<tgd_header::buffer_source> reader{source};
    auto& new_layer = reader.next_layer();

    REQUIRE(new_layer);
    REQUIRE(new_layer.content_type() == tgd_header::layer_content_type::unknown);
    REQUIRE(new_layer.compression_type() == ct);
    REQUIRE(new_layer.tile() == tgd_header::tile_address(4, 3, 2));
    REQUIRE(new_layer.name_length() == std::strlen(name) + 1);
    REQUIRE(!std::strncmp(new_layer.name().data(), name, std::strlen(name)));

    reader.read_content();
    new_layer.decode_content();
    REQUIRE(new_layer.content_length() == std::strlen(content) + 1);
    REQUIRE(!std::strcmp(new_layer.content().data(), content));
}

bool buffer_is_equal(const tgd_header::buffer& buf, const char* str) noexcept {
    if (buf.size() != std::strlen(str) + 1) {
        return false;
    }
    return !std::strcmp(buf.data(), str);
}

TEST_CASE("Set name using cstring") {
    const char* name = "some_name";

    tgd_header::layer layer;
    layer.set_name(name);
    REQUIRE(layer.name_length() == 10);
    REQUIRE(buffer_is_equal(layer.name(), "some_name"));
}

TEST_CASE("Set name using buffer") {
    const char* name = "another_name";
    tgd_header::buffer buffer{name, std::strlen(name) + 1};

    tgd_header::layer layer;
    layer.set_name(std::move(buffer));
    REQUIRE(layer.name_length() == 13);
    REQUIRE(buffer_is_equal(layer.name(), "another_name"));
}


TEST_CASE("Set name using ptr and length") {
    const char* name = "a_name";

    tgd_header::layer layer;
    layer.set_name(name, std::strlen(name) + 1);
    REQUIRE(layer.name_length() == 7);
    REQUIRE(buffer_is_equal(layer.name(), "a_name"));
}

TEST_CASE("Set content using buffer") {
    const char* content = "body";
    tgd_header::buffer buffer{content, std::strlen(content) + 1};

    tgd_header::layer layer;
    layer.set_content(std::move(buffer));
    REQUIRE(layer.content_length() == 5);
    REQUIRE(buffer_is_equal(layer.content(), "body"));
}


TEST_CASE("Set content using ptr and length") {
    const char content[] = "a_body";

    tgd_header::layer layer;
    layer.set_content(content, sizeof(content));
    REQUIRE(layer.content_length() == 7);
    REQUIRE(buffer_is_equal(layer.content(), "a_body"));
}

TEST_CASE("Decoding incomplete layer throws") {
    const std::string data = "abcdefghijkl";

    REQUIRE_THROWS_AS(tgd_header::layer{data}, const tgd_header::format_error&);
}

