
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

static const char content[] = "the quick brown fox jumps over the lazy dog";

TEST_CASE("Encode and decode layer") {
    const char name[] = "test";

    auto ct = tgd_header::layer_compression_type::uncompressed;

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
    layer.set_content(content, sizeof(content));

    // encode layer
    std::string out;
    tgd_header::string_sink sink{out};
    layer.write(sink);

    // check encoded layer
    REQUIRE(out.size() >= 20 + sizeof(name) + sizeof(content));
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
    REQUIRE(new_layer.name_length() == sizeof(name) - 1);
    REQUIRE(!std::strcmp(new_layer.name(), name));

    reader.read_content();
    new_layer.decode_content();
    REQUIRE(new_layer.content_length() == sizeof(content));
    REQUIRE(!std::strcmp(new_layer.content().data(), content));
}

TEST_CASE("Set name using cstring") {
    const char* name = "some_name";

    tgd_header::layer layer;
    layer.set_name(name);
    REQUIRE(layer.name_length() == 9);
    REQUIRE(layer.has_name(name));

    REQUIRE_FALSE(layer.has_name("some_other_name"));
    REQUIRE_FALSE(layer.has_name("long_name"));
    std::string namestr{name};
    REQUIRE(layer.has_name(namestr));
    namestr[1] = 'a';
    REQUIRE_FALSE(layer.has_name(namestr));
    namestr.append("x");
    REQUIRE_FALSE(layer.has_name(namestr));
}

TEST_CASE("Set name using buffer") {
    const char name[] = "another_name";
    tgd_header::buffer buffer{name, sizeof(name)};

    tgd_header::layer layer;
    layer.set_name(std::move(buffer));
    REQUIRE(layer.name_length() == 12);
    REQUIRE(layer.has_name(name));
}

TEST_CASE("Set name using ptr and length") {
    const char name[] = "a_name";

    tgd_header::layer layer;
    layer.set_name(name, sizeof(name));
    REQUIRE(layer.name_length() == 6);
    REQUIRE(layer.has_name(name));
}

static bool buffer_is_equal(const tgd_header::buffer& buf, const char* str) noexcept {
    if (buf.size() != std::strlen(str) + 1) {
        return false;
    }
    return !std::strcmp(buf.data(), str);
}

TEST_CASE("Set content using buffer") {
    const char content[] = "body";
    tgd_header::buffer buffer{content, sizeof(content)};

    tgd_header::layer layer;
    layer.set_content(std::move(buffer));
    REQUIRE(layer.content_length() == 5);
    REQUIRE(buffer_is_equal(layer.content(), content));
}

TEST_CASE("Set content using ptr and length") {
    const char content[] = "a_body";

    tgd_header::layer layer;
    layer.set_content(content, sizeof(content));
    REQUIRE(layer.content_length() == 7);
    REQUIRE(buffer_is_equal(layer.content(), content));
}

TEST_CASE("Decoding incomplete layer throws") {
    const std::string data = "abcdefghijkl";

    REQUIRE_THROWS_AS(tgd_header::layer{data}, const tgd_header::format_error&);
}

static std::string create_test_layer() {

    // set up layer
    tgd_header::layer layer;
    layer.set_content_type(tgd_header::layer_content_type::unknown);
    layer.set_compression_type(tgd_header::layer_compression_type::zlib);
    layer.set_tile(tgd_header::tile_address{4, 3, 2});
    layer.set_name("test");
    layer.set_content(content, sizeof(content));

    // encode layer
    std::string out;
    tgd_header::string_sink sink{out};
    layer.write(sink);

    return out;
}

TEST_CASE("read_content() can be called multiple times") {
    const auto out = create_test_layer();

    // decode layer again and check it
    tgd_header::buffer b{out.data(), out.size()};
    tgd_header::buffer_source source{b};
    tgd_header::reader<tgd_header::buffer_source> reader{source};
    auto& new_layer = reader.next_layer();

    reader.read_content();
    reader.read_content();
    new_layer.decode_content();

    REQUIRE(new_layer.content_length() == sizeof(content));
    REQUIRE(!std::strcmp(new_layer.content().data(), content));
}

TEST_CASE("auto-skips content if read_content() is not called") {
    const auto out = create_test_layer();

    // decode layer again and check it
    tgd_header::buffer b{out.data(), out.size()};
    tgd_header::buffer_source source{b};
    tgd_header::reader<tgd_header::buffer_source> reader{source};

    REQUIRE(reader.next_layer());
    REQUIRE_FALSE(reader.next_layer());
}

TEST_CASE("Error in compressed content is detected") {
    auto out = create_test_layer();

    // corrupt the compressed data
    out[28] = '\0';

    // decode layer again and check it
    tgd_header::buffer b{out.data(), out.size()};
    tgd_header::buffer_source source{b};
    tgd_header::reader<tgd_header::buffer_source> reader{source};
    auto& new_layer = reader.next_layer();

    reader.read_content();
    REQUIRE_THROWS_AS(new_layer.decode_content(), const tgd_header::zlib_error&);
    REQUIRE_THROWS_WITH(new_layer.decode_content(), "failed to uncompress data: data error");
}

TEST_CASE("Length of compressed content is too large") {
    auto out = create_test_layer();

    // corrupt the original_length field
    tgd_header::detail::set<tgd_header::content_length_type>(static_cast<tgd_header::content_length_type>(sizeof(content) + 1), &out[tgd_header::detail::offset::original_length]);

    // decode layer again and check it
    tgd_header::buffer b{out.data(), out.size()};
    tgd_header::buffer_source source{b};
    tgd_header::reader<tgd_header::buffer_source> reader{source};
    auto& new_layer = reader.next_layer();

    reader.read_content();
    REQUIRE_THROWS_AS(new_layer.decode_content(), const tgd_header::format_error&);
    REQUIRE_THROWS_WITH(new_layer.decode_content(), "wrong original size on compressed data");
}

TEST_CASE("Length of compressed content is too small") {
    auto out = create_test_layer();

    // corrupt the original_length field
    tgd_header::detail::set<tgd_header::content_length_type>(static_cast<tgd_header::content_length_type>(sizeof(content) - 1), &out[tgd_header::detail::offset::original_length]);

    // decode layer again and check it
    tgd_header::buffer b{out.data(), out.size()};
    tgd_header::buffer_source source{b};
    tgd_header::reader<tgd_header::buffer_source> reader{source};
    auto& new_layer = reader.next_layer();

    reader.read_content();
    REQUIRE_THROWS_AS(new_layer.decode_content(), const tgd_header::zlib_error&);
    REQUIRE_THROWS_WITH(new_layer.decode_content(), "failed to uncompress data: buffer error");
}

