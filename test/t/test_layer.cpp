
#include <catch.hpp>

#include <tgd_header/buffer.hpp>
#include <tgd_header/buffer_source.hpp>
#include <tgd_header/layer.hpp>
#include <tgd_header/reader.hpp>
#include <tgd_header/string_sink.hpp>
#include <tgd_header/tile.hpp>
#include <tgd_header/types.hpp>

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

