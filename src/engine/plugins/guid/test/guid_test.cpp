#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "guid.hpp"
#include "guid_generator.hpp"

TEST_CASE("Guid default construction") {
    Guid guid;
    CHECK(guid.IsEmpty());
    CHECK_FALSE(static_cast<bool>(guid));
}

TEST_CASE("Guid construction with values") {
    Guid guid(0x12345678, 0x9ABCDEF0);
    CHECK_FALSE(guid.IsEmpty());
    CHECK(guid.High() == 0x12345678);
    CHECK(guid.Low() == 0x9ABCDEF0);
}

TEST_CASE("Guid equality") {
    Guid a(100, 200);
    Guid b(100, 200);
    Guid c(100, 201);

    CHECK(a == b);
    CHECK_FALSE(a == c);
    CHECK(a != c);
}

TEST_CASE("Guid comparison") {
    Guid a(100, 200);
    Guid b(100, 201);
    Guid c(101, 100);

    CHECK(a < b);
    CHECK(a < c);
    CHECK(b < c);
    CHECK(c > a);
}

TEST_CASE("Guid string conversion") {
    SUBCASE("Empty guid returns empty string") {
        Guid guid;
        CHECK(guid.ToString().empty());
    }

    SUBCASE("Round-trip conversion") {
        Guid original(0x12345678AABBCCDD, 0x1122334455667788);
        std::string str = original.ToString();
        Guid parsed = Guid::FromString(str);

        CHECK(original == parsed);
    }

    SUBCASE("Parse standard UUID format") {
        std::string uuidStr = "550e8400-e29b-41d4-a716-446655440000";
        Guid guid = Guid::FromString(uuidStr);

        CHECK_FALSE(guid.IsEmpty());
        CHECK(guid.ToString() == uuidStr);
    }
}

TEST_CASE("Guid generation") {
    Guid a = Guid::Generate();
    Guid b = Guid::Generate();

    CHECK_FALSE(a.IsEmpty());
    CHECK_FALSE(b.IsEmpty());
    CHECK(a != b);
}

TEST_CASE("GuidGenerator generates unique guids") {
    Guid a = GuidGenerator::GenerateGuid();
    Guid b = GuidGenerator::GenerateGuid();

    CHECK_FALSE(a.IsEmpty());
    CHECK_FALSE(b.IsEmpty());
    CHECK(a != b);
}

TEST_CASE("Guid hash") {
    Guid a(100, 200);
    Guid b(100, 200);
    Guid c(100, 201);

    std::hash<Guid> hasher;
    CHECK(hasher(a) == hasher(b));
    CHECK(hasher(a) != hasher(c));
}
