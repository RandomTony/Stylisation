#include "gtest/gtest.h"

#include "lime.hpp"
using namespace lime;

class Point2dTest : public ::testing::Test {
protected:
    static const int nSimpleLoop = 100;
    static Random rng;

protected:
    Point2dTest() {}
    virtual ~Point2dTest() {}
};

Random Point2dTest::rng = Random((unsigned int)time(0));

TEST_F(Point2dTest, Sign) {
    EXPECT_EQ(lime::sign(10.0), 1);
    EXPECT_EQ(lime::sign(-10.0), -1);
    EXPECT_EQ(lime::sign(1.0e-12), 0);
    EXPECT_EQ(lime::sign(-1.0e-12), 0);
}

TEST_F(Point2dTest, DefaultConstructor) {
    const Point2d p;
    EXPECT_EQ(p.x, 0.0);
    EXPECT_EQ(p.y, 0.0);
}

TEST_F(Point2dTest, OperatorEq) {
    Point2d p(1.0, 2.0);
    Point2d q(2.0, 2.0);
    q = p;
    EXPECT_EQ(p, q);
    p.x = 2.0;
    EXPECT_NE(p, q);
}

TEST_F(Point2dTest, AddAndSubtract) {
    for (int i = 0; i < nSimpleLoop; i++) {
        const Point2d p(rng.nextReal(), rng.nextReal());
        const Point2d q(rng.nextReal(), rng.nextReal());
        EXPECT_EQ(p.x + q.x, (p + q).x) << "Add: x does not match";
        EXPECT_EQ(p.y + q.y, (p + q).y) << "Add: y does not match";
        EXPECT_EQ(p.x - q.x, (p - q).x) << "Subtract: x does not match";
        EXPECT_EQ(p.y - q.y, (p - q).y) << "Subtract: x does not match";
    }
}

TEST_F(Point2dTest, MultiplyAndDiv) {
    for (int i = 0; i < nSimpleLoop; i++) {
        const Point2d p(rng.nextReal(), rng.nextReal());
        const double d = rng.nextReal() + 1.0e-8;
        EXPECT_EQ(p.x * d, (p * d).x) << "Multiply: x does not match";
        EXPECT_EQ(p.y * d, (p * d).y) << "Multiply: y does not match";
        EXPECT_EQ(p.x / d, (p / d).x) << "Divide: x does not match";
        EXPECT_EQ(p.y / d, (p / d).y) << "Divide: y does not match";
    }
    Point2d p(1.0, 2.0);
    ASSERT_DEATH(p / 0.0, "") << "Divide: zero division";
}

TEST_F(Point2dTest, Norm) {
    for (int i = 0; i < nSimpleLoop; i++) {
        double dx = rng.nextReal();
        double dy = rng.nextReal();
        const Point2d p(dx, dy);
        EXPECT_EQ(p.norm(), hypot(dx, dy)) << "Norm: value does not match";
    }
}

TEST_F(Point2dTest, Normalize) {
    for (int i = 0; i < nSimpleLoop; i++) {
        double dx = rng.nextReal();
        double dy = rng.nextReal();
        const Point2d p(dx, dy);
        double norm = p.norm();
        EXPECT_EQ(p.normalize().x, dx / norm) << "Normalize: x does not match";
        EXPECT_EQ(p.normalize().y, dy / norm) << "Normalize: y does not match";
    }

    const Point2d p(0.0, 0.0);
    ASSERT_DEATH(p.normalize(), "") << "Normalize: zero division";
}

TEST_F(Point2dTest, DotAndDet) {
    for (int i = 0; i < nSimpleLoop; i++) {
        double dx1 = rng.nextReal();
        double dy1 = rng.nextReal();
        double dx2 = rng.nextReal();
        double dy2 = rng.nextReal();
        const Point2d p1(dx1, dy1);
        const Point2d p2(dx2, dy2);
        EXPECT_EQ(p1.dot(p2), dx1 * dx2 + dy1 * dy2) << "Dot: value does not match";
        EXPECT_EQ(p1.det(p2), dx1 * dy2 - dx2 * dy1) << "Det: value does not match";
    }
}
