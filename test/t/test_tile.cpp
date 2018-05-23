
#include <catch.hpp>

#include <tgd_header/tile.hpp>

TEST_CASE("Default tile") {
    tgd_header::tile_address t;

    REQUIRE(t.zoom() == 0);
    REQUIRE(t.x() == 0);
    REQUIRE(t.y() == 0);
}

TEST_CASE("Defined tile") {
    tgd_header::tile_address t{10, 5, 3};

    REQUIRE(t.zoom() == 10);
    REQUIRE(t.x() == 5);
    REQUIRE(t.y() == 3);
}

TEST_CASE("Tile comparison") {
    tgd_header::tile_address t1{10, 5, 3};
    tgd_header::tile_address t2{10, 5, 4};
    tgd_header::tile_address t3{10, 5, 3};
    tgd_header::tile_address t4{11, 5, 3};

    REQUIRE_FALSE(t1 == t2);
    REQUIRE(t1 == t3);
    REQUIRE_FALSE(t1 == t4);
    REQUIRE_FALSE(t2 == t3);
    REQUIRE_FALSE(t2 == t4);
    REQUIRE_FALSE(t3 == t4);

    REQUIRE(t1 != t2);
    REQUIRE_FALSE(t1 != t3);
    REQUIRE(t1 != t4);
    REQUIRE(t2 != t3);
    REQUIRE(t2 != t4);
    REQUIRE(t3 != t4);
}


