
#include <catch.hpp>

#include <tgd_header/file_sink.hpp>
#include <tgd_header/file_source.hpp>
#include <tgd_header/mmap_source.hpp>

#include <algorithm>

TEST_CASE("Write and read buffer") {
    const auto filename = "test_file_1";
    const char data[] = "this is some test data\n";
    const auto data_size = sizeof(data) - 1;

    tgd_header::buffer out_buffer{data, data_size};

    {
        tgd_header::file_sink sink{filename};
        REQUIRE(sink.fd() > 2);
        sink.write(out_buffer);
        sink.close();
    }

    tgd_header::file_source source{filename};
    REQUIRE(source.file_size() == data_size);

    const auto in_buffer = source.read(data_size);
    REQUIRE(in_buffer.size() == out_buffer.size());
    REQUIRE(std::equal(in_buffer.begin(), in_buffer.end(), out_buffer.begin()));

    unlink(filename);
}

TEST_CASE("Write and read buffer with padding and skip") {
    const auto filename = "test_file_2";
    const char data[] = "this is some test data\n";
    const auto data_size = sizeof(data) - 1;
    const std::size_t padding_size = 2;

    tgd_header::buffer out_buffer{data, data_size};

    {
        tgd_header::file_sink sink{filename};
        REQUIRE(sink.fd() > 2);
        sink.padding(padding_size);
        sink.write(out_buffer);
        sink.close();
    }

    tgd_header::file_source source{filename};
    REQUIRE(source.file_size() == data_size + padding_size);

    source.skip(padding_size);

    const auto in_buffer = source.read(data_size);
    REQUIRE(in_buffer.size() == out_buffer.size());
    REQUIRE(std::equal(in_buffer.begin(), in_buffer.end(), out_buffer.begin()));

    unlink(filename);
}

TEST_CASE("Write and read buffer using mmap with padding and skip") {
    const auto filename = "test_file_3";
    const char data[] = "this is some test data\n";
    const auto data_size = sizeof(data) - 1;
    const std::size_t padding_size = 2;

    tgd_header::buffer out_buffer{data, data_size};

    {
        tgd_header::file_sink sink{filename};
        REQUIRE(sink.fd() > 2);
        sink.padding(padding_size);
        sink.write(out_buffer);
        sink.close();
    }

    tgd_header::mmap_source source{filename};
    REQUIRE(source.file_size() == data_size + padding_size);

    source.skip(padding_size);

    const auto in_buffer = source.read(data_size);
    REQUIRE(in_buffer.size() == out_buffer.size());
    REQUIRE(std::equal(in_buffer.begin(), in_buffer.end(), out_buffer.begin()));

    unlink(filename);
}

TEST_CASE("File source stdin (empty)") {
    tgd_header::file_source source{""};
    REQUIRE(source.fd() == 0);
}

TEST_CASE("File source stdin (-)") {
    tgd_header::file_source source{"-"};
    REQUIRE(source.fd() == 0);
}

TEST_CASE("File sink stdin (empty)") {
    tgd_header::file_sink sink{""};
    REQUIRE(sink.fd() == 1);
}

TEST_CASE("File sink stdin (-)") {
    tgd_header::file_sink sink{"-"};
    REQUIRE(sink.fd() == 1);
}

