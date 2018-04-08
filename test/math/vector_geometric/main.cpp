#include <math/math.hpp>
#include <unit_test/suite.hpp>

using namespace framework::math;

class geometric_function_tests : public framework::unit_test::suite
{
public:
    geometric_function_tests() : suite("geometric_function_tests")
    {
        add_test([this]() { length_function(); }, "length_function");
        add_test([this]() { distance_function(); }, "distance_function");
        add_test([this]() { dot_function(); }, "dot_function");
        add_test([this]() { cross_function(); }, "cross_function");
        add_test([this]() { normalize_function(); }, "normalize_function");
        add_test([this]() { faceforward_function(); }, "faceforward_function");
        add_test([this]() { reflection_function(); }, "reflection_function");
        add_test([this]() { refraction_function(); }, "refraction_function");

        v3f = {1.0f, 2.0f, 3.0f};
    }

private:
    void length_function()
    {
        TEST_ASSERT(almost_equal(length(v3f), 3.7416574954986572f), "Length function failed.");
    }

    void distance_function()
    {
        const vector3f v3f1 = {3.0f, 3.0f, 5.0f};
        const vector3f v3f2 = {2.0f, 1.0f, 2.0f};

        TEST_ASSERT(almost_equal(distance(v3f1, v3f2), length(v3f)), "Distance function failed.");
        TEST_ASSERT(almost_equal(distance(v3f1, v3f2), length(v3f)), "Distance function failed.");
    }

    void dot_function()
    {
        TEST_ASSERT(almost_equal(dot(v3f, v3f), 14.0f), "Dot function failed.");
        TEST_ASSERT(almost_equal(dot(vector2f{0.0f, 1.0f}, vector2f{1.0f, 0.0f}), 0.0f), "Dot function failed.");
    }

    void cross_function()
    {
        TEST_ASSERT(cross(vector3f(3.0f, 2.0f, 1.0f), v3f) == vector3f(4, -8, 4), "Cross function failed.");
    }

    void normalize_function()
    {
        TEST_ASSERT(almost_equal(length(normalize(v3f)), 1.0f, 1), "Normalize function failed.");
    }

    void faceforward_function()
    {
        TEST_ASSERT(faceforward(v3f, v3f, vector3f(1.0f, 2.0f, -3.0f)) == v3f, "Faceforward function failed.");
        TEST_ASSERT(faceforward(v3f, v3f, v3f) == -v3f, "Faceforward function failed.");
    }

    void reflection_function()
    {
        TEST_ASSERT(almost_equal(reflect(v3f, normalize(v3f)), -v3f, 1), "Reflection function failed.");
    }

    void refraction_function()
    {
        const vector3f result = refract(v3f, normalize(v3f), 1.0f);

        TEST_ASSERT(almost_equal(result, reflect(v3f, normalize(v3f))), "Refraction function failed.");
        TEST_ASSERT(almost_equal(refract(vector2f{0.0f, 1.0f}, vector2f{1.0f, 0.0f}, 2.0f), vector2f{0.0f}),
                    "Refraction function failed.");
    }

    vector3f v3f;
};

int main()
{
    return run_tests(geometric_function_tests());
}
