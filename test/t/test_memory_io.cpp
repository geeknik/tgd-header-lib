
#include <catch.hpp>

#include <tgd_header/buffer_source.hpp>
#include <tgd_header/string_sink.hpp>

#include <string>

TEST_CASE("Write to string_sink and read from buffer_source") {
    const char data[] = "this is some test data\n";
    const auto data_size = sizeof(data) - 1;
    const std::size_t padding_size = 2;

    tgd_header::buffer out_buffer{data, data_size};

    std::string data_str;

    {
        tgd_header::string_sink sink{data_str};
        sink.padding(padding_size);
        sink.write(out_buffer);
    }

    tgd_header::buffer data_buffer{data_str.data(), data_str.size()};
    tgd_header::buffer_source source{data_buffer};

    source.skip(padding_size);

    const auto in_buffer = source.read(data_size);
    REQUIRE(in_buffer);
    REQUIRE(in_buffer.size() == out_buffer.size());
    REQUIRE(std::equal(in_buffer.begin(), in_buffer.end(), out_buffer.begin()));

    const auto in_buffer2 = source.read(1);
    REQUIRE_FALSE(in_buffer2);
}

