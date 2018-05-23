
#include <catch.hpp>

#include <tgd_header/buffer.hpp>

#include <array>
#include <cstring>
#include <string>

TEST_CASE("Empty buffer") {
    tgd_header::buffer b;

    REQUIRE_FALSE(b);
    REQUIRE_FALSE(b.managed());
    REQUIRE(b.size() == 0);
    REQUIRE(b.data() == nullptr);
    b.clear();
    REQUIRE_FALSE(b);
}

TEST_CASE("Non-managed buffer") {
    char a;
    tgd_header::buffer b{&a, 1};

    REQUIRE(b);
    REQUIRE_FALSE(b.managed());
    REQUIRE(&a == b.data());
    REQUIRE(b.size() == 1);
    b.clear();
    REQUIRE_FALSE(b);
}

TEST_CASE("Non-managed buffer (explicit)") {
    char a;
    tgd_header::buffer b{&a, 1, false};

    REQUIRE(b);
    REQUIRE_FALSE(b.managed());
    REQUIRE(&a == b.data());
    REQUIRE(b.size() == 1);
}

TEST_CASE("Managed pre-allocated buffer") {
    char* a = new char[20];
    tgd_header::buffer b{a, 20, true};

    REQUIRE(b);
    REQUIRE(b.managed());
    REQUIRE(a == b.data());
    REQUIRE(b.size() == 20);
    b.clear();
    REQUIRE_FALSE(b);
}

TEST_CASE("Managed buffer from std::array") {
    std::array<char, 10> data{"abc"};
    tgd_header::buffer b{data};

    REQUIRE(b);
    REQUIRE_FALSE(b.managed());
    REQUIRE(b.size() == 10);
    b.clear();
    REQUIRE_FALSE(b);
}

TEST_CASE("Managed buffer with existing data") {
    std::string some_data{"1234567890abc"};
    auto b = tgd_header::buffer{some_data.data(), some_data.size()}.copy();

    REQUIRE(b);
    REQUIRE(b.managed());
    REQUIRE(b.size() == 13);
    REQUIRE(b.data() != some_data.data());
    REQUIRE(!std::strncmp(b.data(), some_data.data(), some_data.size()));
    b.clear();
    REQUIRE_FALSE(b);
}

