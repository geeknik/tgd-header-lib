
#include <catch.hpp>

#include <tgd_header/buffer.hpp>

#include <array>
#include <cstring>
#include <iterator>
#include <string>
#include <type_traits>

static_assert(!std::is_copy_constructible<tgd_header::buffer>(), "buffer should not be copy constructible");
static_assert(!std::is_copy_assignable<tgd_header::buffer>(), "buffer should not be copy constructible");

TEST_CASE("Empty buffer") {
    tgd_header::buffer b;

    REQUIRE_FALSE(b);
    REQUIRE_FALSE(b.managed());
    REQUIRE(b.size() == 0);
    REQUIRE(b.data() == nullptr);
    REQUIRE(b.begin() == b.end());
    REQUIRE(b.cbegin() == b.cend());
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
    auto a = new char[20];
    tgd_header::buffer b{a, 20, true};

    REQUIRE(b);
    REQUIRE(b.managed());
    REQUIRE(a == b.data());
    REQUIRE(b.size() == 20);
    REQUIRE(std::distance(b.begin(), b.end()) == 20);
    REQUIRE(std::distance(b.cbegin(), b.cend()) == 20);
    b.clear();
    REQUIRE_FALSE(b);
}

TEST_CASE("Unmanaged buffer from std::array") {
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

TEST_CASE("Managed buffer created from mutable_buffer") {
    tgd_header::mutable_buffer mb{20};
    REQUIRE(mb.size() == 20);
    REQUIRE(mb.data() != nullptr);
    REQUIRE(std::distance(mb.begin(), mb.end()) == 20);
    REQUIRE(std::distance(mb.cbegin(), mb.cend()) == 20);

    std::array<char, 10> data{"abcd_abcd"};

    std::copy(data.begin(), data.end(), mb.begin());

    tgd_header::buffer b{std::move(mb)};
    REQUIRE(std::distance(b.begin(), b.end()) == 20);
    REQUIRE(std::distance(b.cbegin(), b.cend()) == 20);
    REQUIRE(mb.size() == 0); // NOLINT(bugprone-use-after-move) testing our own code here
    REQUIRE(mb.data() == nullptr); // NOLINT(bugprone-use-after-move) testing our own code here

    REQUIRE(b);
    REQUIRE(b.managed());
    REQUIRE(b.size() == 20);
    REQUIRE(b.data() == std::string{"abcd_abcd"});
}

TEST_CASE("Swap buffers") {
    std::array<char, 10> data1{"abc"};
    tgd_header::buffer b1{data1};
    REQUIRE_FALSE(b1.managed());
    REQUIRE(b1.size() == 10);

    std::array<char, 20> data2{"def"};
    tgd_header::buffer b2{data2};
    REQUIRE_FALSE(b2.managed());
    REQUIRE(b2.size() == 20);

    using std::swap;
    swap(b1, b2);
    REQUIRE(b1.size() == 20);
    REQUIRE(b2.size() == 10);
}

