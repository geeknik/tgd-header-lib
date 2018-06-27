
#include <catch.hpp>

#include <tgd_header/file_sink.hpp>
#include <tgd_header/file_source.hpp>
#include <tgd_header/mmap_source.hpp>

#include <algorithm>
#include <type_traits>

static_assert(!std::is_copy_constructible<tgd_header::file_source>(), "file_source should not be copy constructible");
static_assert(!std::is_copy_assignable<tgd_header::file_source>(), "file_source should not be copy constructible");

static_assert(!std::is_copy_constructible<tgd_header::mmap_source>(), "mmap_source should not be copy constructible");
static_assert(!std::is_copy_assignable<tgd_header::mmap_source>(), "mmap_source should not be copy constructible");

static_assert(!std::is_copy_constructible<tgd_header::file_sink>(), "file_sink should not be copy constructible");
static_assert(!std::is_copy_assignable<tgd_header::file_sink>(), "file_sink should not be copy constructible");

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
    REQUIRE(in_buffer);
    REQUIRE(in_buffer.size() == out_buffer.size());
    REQUIRE(std::equal(in_buffer.begin(), in_buffer.end(), out_buffer.begin()));

    const auto end_buffer = source.read(1);
    REQUIRE_FALSE(end_buffer);

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

    SECTION("move construct/swap source") {
        tgd_header::file_source source2{std::move(source)};
        using std::swap;
        swap(source2, source);
    }

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

    REQUIRE_THROWS_AS(source.skip(1000), const std::range_error&);

    source.skip(padding_size);

    SECTION("move construct/swap source") {
        tgd_header::mmap_source source2{std::move(source)};
        using std::swap;
        swap(source2, source);
    }

    SECTION("move construct/move assign source") {
        tgd_header::mmap_source source2{std::move(source)};
        source = std::move(source2);
    }

    const auto in_buffer = source.read(data_size);
    REQUIRE(in_buffer);
    REQUIRE(in_buffer.size() == out_buffer.size());
    REQUIRE(std::equal(in_buffer.begin(), in_buffer.end(), out_buffer.begin()));

    const auto end_buffer = source.read(1);
    REQUIRE_FALSE(end_buffer);

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

TEST_CASE("File sink stdout (empty)") {
    tgd_header::file_sink sink{""};
    REQUIRE(sink.fd() == 1);
}

TEST_CASE("File sink stdout (-)") {
    tgd_header::file_sink sink{"-"};
    REQUIRE(sink.fd() == 1);
}

TEST_CASE("Calling close() on closed file") {
    const auto filename = "test_file_4";

    tgd_header::file_sink sink{filename};
    close(sink.fd()); // you should never do this
    REQUIRE_THROWS_AS(sink.close(), const std::system_error&);

    unlink(filename);
}

TEST_CASE("Destructor must not throw") {
    const auto filename = "test_file_4";

    {
        tgd_header::file_sink sink{filename};
        close(sink.fd()); // you should never do this
    }
    // destructor of sink called here, must not throw

    unlink(filename);
}

TEST_CASE("Calling file_size() on closed file") {
    const auto filename = "test_file_5";

    tgd_header::file_sink sink{filename};
    close(sink.fd()); // you should never do this
    REQUIRE_THROWS_AS(sink.file_size(), const std::system_error&);

    unlink(filename);
}

TEST_CASE("Throw when trying to open nonexistent file") {
    const auto filename = "file_does_not_exist";

    REQUIRE_THROWS_AS(tgd_header::file_source{filename}, const std::system_error&);
}

TEST_CASE("Throw when there is an error writing to a file") {
    const auto filename = "test_file_6";

    tgd_header::file_sink sink{filename};
    sink.padding(2);
    close(sink.fd()); // you should never do this
    REQUIRE_THROWS_AS(sink.padding(2), const std::system_error&);

    unlink(filename);
}

TEST_CASE("Throw when there is an error seeking in a file") {
    const auto filename = "test_file_7";

    tgd_header::file_sink sink{filename};
    sink.padding(5);
    sink.close();

    tgd_header::file_source source{filename};
    close(source.fd()); // you should never do this
    REQUIRE_THROWS_AS(source.skip(5), const std::system_error&);

    unlink(filename);
}

TEST_CASE("Mmapping zero-length file fails") {
    const auto filename = "test_file_8";

    tgd_header::file_sink sink{filename};
    sink.close();

    // XXX this should fail (length is zero), but doesn't for some reason
    // tgd_header::mmap_source source{filename};

    unlink(filename);
}

