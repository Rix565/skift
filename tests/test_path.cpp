#include <stdio.h>

#include <libsystem/Assert.h>
#include <libsystem/Path.h>

#define TEST(__func) void __func()

TEST(absolute_path_are_absolute)
{
    auto p = Path::parse("/home/user/local");

    assert(p.absolute());
}

TEST(absolute_path_are_no_relative)
{
    auto p = Path::parse("/home/user/local");

    assert(!p.relative());
}

TEST(relative_path_are_not_absolute)
{
    auto p = Path::parse("user/local");

    assert(!p.absolute());
}

TEST(relative_path_are_relative)
{
    auto p = Path::parse("user/local");

    assert(p.relative());
}

TEST(same_path_are_equals)
{
    auto p1 = Path::parse("/home/user/local");
    auto p2 = Path::parse("/home/user/local");

    assert(p1 == p2);
}

TEST(different_path_are_not_equals)
{
    auto p1 = Path::parse("/home/user/local/documents");
    auto p2 = Path::parse("/home/user/local");

    assert(p1 != p2);
}

TEST(a_path_and_its_string_representation_are_equals)
{
    auto p = Path::parse("/home/user/local");
    assert(p.string() == "/home/user/local");
}

TEST(path_is_equal_to_its_normalized_counter_part)
{
    auto p = Path::parse("/home/../home/././././user/local");
    p = p.normalized();

    assert(p.string() == "/home/user/local");
}

TEST(joined_path_is_equal_to_its_normalized_counter_part)
{
    auto p1 = Path::parse("/home/");
    auto p2 = Path::join(p1, "user/local");

    assert(p2.string() == "/home/user/local");
}

int main(int, char const *[])
{
    absolute_path_are_absolute();
    absolute_path_are_no_relative();
    relative_path_are_not_absolute();
    relative_path_are_relative();
    same_path_are_equals();
    different_path_are_not_equals();
    a_path_and_its_string_representation_are_equals();
    path_is_equal_to_its_normalized_counter_part();
    joined_path_is_equal_to_its_normalized_counter_part();

    return 0;
}
