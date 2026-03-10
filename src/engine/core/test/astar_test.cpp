#include <gtest/gtest.h>
#include "astar_grid.hpp"

TEST(AStar, FindsPathInOpenGrid) {
    std::vector<std::vector<int>> grid(5, std::vector<int>(5, 0));

    AStar astar(5, 5, grid);
    auto path = astar.findPath({0, 0}, {4, 4}, true);

    EXPECT_FALSE(path.empty());
    EXPECT_EQ(path.front(), glm::ivec2(0, 0));
    EXPECT_EQ(path.back(), glm::ivec2(4, 4));
}

TEST(AStar, ReturnsEmptyPathWhenBlocked) {
    std::vector<std::vector<int>> grid(5, std::vector<int>(5, 0));
    for (int i = 0; i < 5; i++) {
        grid[i][2] = 1;
    }

    AStar astar(5, 5, grid);
    auto path = astar.findPath({0, 0}, {4, 4}, true);

    EXPECT_TRUE(path.empty());
}

TEST(AStar, FindsPathAroundObstacle) {
    std::vector<std::vector<int>> grid(5, std::vector<int>(5, 0));
    for (int i = 1; i < 5; i++) {
        grid[i][2] = 1;
    }

    AStar astar(5, 5, grid);
    auto path = astar.findPath({0, 0}, {4, 4}, true);

    EXPECT_FALSE(path.empty());
    EXPECT_EQ(path.front(), glm::ivec2(0, 0));
    EXPECT_EQ(path.back(), glm::ivec2(4, 4));
}

TEST(AStar, PathToSamePosition) {
    std::vector<std::vector<int>> grid(5, std::vector<int>(5, 0));

    AStar astar(5, 5, grid);
    auto path = astar.findPath({2, 2}, {2, 2}, true);

    EXPECT_FALSE(path.empty());
    EXPECT_EQ(path.size(), 1);
    EXPECT_EQ(path.front(), glm::ivec2(2, 2));
}

TEST(AStar, WithJumpEnabled) {
    std::vector<std::vector<int>> grid(3, std::vector<int>(3, 0));
    AStar astar(3, 3, grid);

    auto path = astar.findPath({0, 0}, {0, 2}, true);
    EXPECT_FALSE(path.empty());
}

TEST(AStar, WithoutJump) {
    std::vector<std::vector<int>> grid(3, std::vector<int>(3, 0));
    AStar astar(3, 3, grid);

    auto path = astar.findPath({1, 1}, {1, 0}, false);
    // Without jump, can only go: down, right, up (not left)
    // This might still find a path going around
}
