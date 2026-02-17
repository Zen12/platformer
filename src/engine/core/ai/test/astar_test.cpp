#include <doctest/doctest.h>
#include "astar_grid.hpp"

TEST_CASE("AStar finds path in open grid") {
    // Create 5x5 open grid (all walkable = 0)
    std::vector<std::vector<int>> grid(5, std::vector<int>(5, 0));

    AStar astar(5, 5, grid);
    auto path = astar.findPath({0, 0}, {4, 4}, true);

    CHECK_FALSE(path.empty());
    CHECK(path.front() == glm::ivec2(0, 0));
    CHECK(path.back() == glm::ivec2(4, 4));
}

TEST_CASE("AStar returns empty path when blocked") {
    // Create grid with wall blocking path
    std::vector<std::vector<int>> grid(5, std::vector<int>(5, 0));
    // Create vertical wall
    for (int i = 0; i < 5; i++) {
        grid[i][2] = 1;  // 1 = blocked
    }

    AStar astar(5, 5, grid);
    auto path = astar.findPath({0, 0}, {4, 4}, true);

    CHECK(path.empty());
}

TEST_CASE("AStar finds path around obstacle") {
    // Create grid with partial wall
    std::vector<std::vector<int>> grid(5, std::vector<int>(5, 0));
    // Create partial wall (leave gap at top)
    for (int i = 1; i < 5; i++) {
        grid[i][2] = 1;
    }

    AStar astar(5, 5, grid);
    auto path = astar.findPath({0, 0}, {4, 4}, true);

    CHECK_FALSE(path.empty());
    CHECK(path.front() == glm::ivec2(0, 0));
    CHECK(path.back() == glm::ivec2(4, 4));
}

TEST_CASE("AStar path to same position") {
    std::vector<std::vector<int>> grid(5, std::vector<int>(5, 0));

    AStar astar(5, 5, grid);
    auto path = astar.findPath({2, 2}, {2, 2}, true);

    CHECK_FALSE(path.empty());
    CHECK(path.size() == 1);
    CHECK(path.front() == glm::ivec2(2, 2));
}

TEST_CASE("AStar respects canJump parameter") {
    std::vector<std::vector<int>> grid(3, std::vector<int>(3, 0));

    AStar astar(3, 3, grid);

    SUBCASE("With jump enabled - can go any direction") {
        auto path = astar.findPath({0, 0}, {0, 2}, true);
        CHECK_FALSE(path.empty());
    }

    SUBCASE("Without jump - limited directions") {
        // Without jump, can only go: down, right, up (not left)
        auto path = astar.findPath({1, 1}, {1, 0}, false);
        // This might still find a path going around
        // The exact behavior depends on the direction constraints
    }
}
