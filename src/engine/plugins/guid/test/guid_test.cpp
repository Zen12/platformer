#include <gtest/gtest.h>
#include "guid.hpp"
#include "guid_generator.hpp"

TEST(Guid, DefaultConstruction) {
    Guid guid;
    EXPECT_TRUE(guid.IsEmpty());
    EXPECT_FALSE(static_cast<bool>(guid));
}

TEST(Guid, ConstructionWithValues) {
    Guid guid(0x12345678, 0x9ABCDEF0);
    EXPECT_FALSE(guid.IsEmpty());
    EXPECT_EQ(guid.High(), 0x12345678);
    EXPECT_EQ(guid.Low(), 0x9ABCDEF0);
}

TEST(Guid, Equality) {
    Guid a(100, 200);
    Guid b(100, 200);
    Guid c(100, 201);

    EXPECT_EQ(a, b);
    EXPECT_FALSE(a == c);
    EXPECT_NE(a, c);
}

TEST(Guid, Comparison) {
    Guid a(100, 200);
    Guid b(100, 201);
    Guid c(101, 100);

    EXPECT_LT(a, b);
    EXPECT_LT(a, c);
    EXPECT_LT(b, c);
    EXPECT_GT(c, a);
}

TEST(Guid, EmptyGuidReturnsEmptyString) {
    Guid guid;
    EXPECT_TRUE(guid.ToString().empty());
}

TEST(Guid, RoundTripConversion) {
    Guid original(0x12345678AABBCCDD, 0x1122334455667788);
    std::string str = original.ToString();
    Guid parsed = Guid::FromString(str);

    EXPECT_EQ(original, parsed);
}

TEST(Guid, ParseStandardUuidFormat) {
    std::string uuidStr = "550e8400-e29b-41d4-a716-446655440000";
    Guid guid = Guid::FromString(uuidStr);

    EXPECT_FALSE(guid.IsEmpty());
    EXPECT_EQ(guid.ToString(), uuidStr);
}

TEST(Guid, Generation) {
    Guid a = Guid::Generate();
    Guid b = Guid::Generate();

    EXPECT_FALSE(a.IsEmpty());
    EXPECT_FALSE(b.IsEmpty());
    EXPECT_NE(a, b);
}

TEST(GuidGenerator, GeneratesUniqueGuids) {
    Guid a = GuidGenerator::GenerateGuid();
    Guid b = GuidGenerator::GenerateGuid();

    EXPECT_FALSE(a.IsEmpty());
    EXPECT_FALSE(b.IsEmpty());
    EXPECT_NE(a, b);
}

TEST(Guid, Hash) {
    Guid a(100, 200);
    Guid b(100, 200);
    Guid c(100, 201);

    std::hash<Guid> hasher;
    EXPECT_EQ(hasher(a), hasher(b));
    EXPECT_NE(hasher(a), hasher(c));
}
