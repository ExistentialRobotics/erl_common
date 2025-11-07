#include "erl_common/enum_parse.hpp"
#include "erl_common/test_helper.hpp"
#include "erl_common/yaml.hpp"

#include <iostream>

enum class Color {
    kRed = 0,
    kBlue = 1,
};

ERL_REFLECT_ENUM_SCHEMA(
    Color,
    2,
    ERL_REFLECT_ENUM_MEMBER("red", Color::kRed),
    ERL_REFLECT_ENUM_MEMBER("blue", Color::kBlue));

ERL_PARSE_ENUM(Color, 2);

struct SubSetting : erl::common::Yamlable<SubSetting> {
    int c = 0;
    int d = 1;

    ERL_REFLECT_SCHEMA(
        SubSetting,
        ERL_REFLECT_MEMBER(SubSetting, c),
        ERL_REFLECT_MEMBER(SubSetting, d));
};

struct Setting : erl::common::Yamlable<Setting> {
    int a = 0;
    double b = 3.0;
    SubSetting sub_setting;
    std::shared_ptr<SubSetting> sub_setting_shared = std::make_shared<SubSetting>();
    Eigen::Vector2d v = {1.0, 2.0};
    Eigen::Matrix2d m = Eigen::Matrix2d::Identity();
    std::vector<float> vec_float;
    std::pair<long, int> p = {100, 200};
    Color color = Color::kRed;

    ERL_REFLECT_SCHEMA(
        Setting,
        ERL_REFLECT_MEMBER(Setting, a),
        ERL_REFLECT_MEMBER(Setting, b),
        ERL_REFLECT_MEMBER(Setting, sub_setting),
        ERL_REFLECT_MEMBER(Setting, sub_setting_shared),
        ERL_REFLECT_MEMBER(Setting, v),
        ERL_REFLECT_MEMBER(Setting, m),
        ERL_REFLECT_MEMBER(Setting, vec_float),
        ERL_REFLECT_MEMBER(Setting, p),
        ERL_REFLECT_MEMBER(Setting, color));
};

TEST(YamlTest, FromYamlString) {
    Setting setting;
    ASSERT_EQ(setting.a, 0);
    ASSERT_EQ(setting.b, 3.0);
    ASSERT_EQ(setting.sub_setting.c, 0);
    ASSERT_EQ(setting.sub_setting.d, 1);
    std::cout << setting << std::endl;

    const std::string &yaml_str = R"(
a: 10
b: 1.0
sub_setting:
    c: 2
    d: 3
sub_setting_shared:
    c: 4
    d: 5
v: [3.0, 4.0]
m: [[1.0, 2.0], [3.0, 4.0]]
vec_float: [1.0, 2.0, 4.0]
p: [500, 600]
color: blue)";

    const YAML::Node node(yaml_str);
    std::cout << node << std::endl;

    ASSERT_TRUE(setting.FromYamlString(yaml_str));

    std::cout << setting << std::endl;
    ASSERT_EQ(setting.a, 10);
    ASSERT_EQ(setting.b, 1.0);
    ASSERT_EQ(setting.sub_setting.c, 2);
    ASSERT_EQ(setting.sub_setting.d, 3);
    ASSERT_EQ(setting.sub_setting_shared->c, 4);
    ASSERT_EQ(setting.sub_setting_shared->d, 5);
    ASSERT_EQ(setting.v[0], 3.0);
    ASSERT_EQ(setting.v[1], 4.0);
    ASSERT_EQ(setting.m(0, 0), 1.0);
    ASSERT_EQ(setting.m(0, 1), 2.0);
    ASSERT_EQ(setting.m(1, 0), 3.0);
    ASSERT_EQ(setting.m(1, 1), 4.0);
    ASSERT_EQ(setting.vec_float.size(), 3);
    ASSERT_EQ(setting.vec_float[0], 1.0f);
    ASSERT_EQ(setting.vec_float[1], 2.0f);
    ASSERT_EQ(setting.vec_float[2], 4.0f);
    ASSERT_EQ(setting.p.first, 500);
    ASSERT_EQ(setting.p.second, 600);
    ASSERT_EQ(setting.color, Color::kBlue);

    std::cout << "`YAML::Node another_node = node` is shallow copy." << std::endl;
    YAML::Node node2 = setting.AsYamlNode();
    YAML::Node node3 = node2["sub_setting"];
    YAML::Node node4 = node3["c"];
    node4 = 5;                        // modify the node
    std::cout << node2 << std::endl;  // should print the modified value
}

TEST(YamlTest, FromCommandLine) {
    Setting setting;

    std::cout << setting << std::endl;

    // {
    //     const char *argv[] = {"test", "--help"};
    //     std::cout << "FromCommandLine with 2 arguments: \n";
    //     for (auto arg: argv) { std::cout << arg << " "; }
    //     std::cout << std::endl;
    //     setting.FromCommandLine(2, argv);
    //     std::cout << setting << std::endl;
    // }

    {
        const char *argv[] = {
            "test", "--a",
            "10",   "--b",
            "1.0",  "--sub_setting.c",
            "2",    "--sub_setting.d",
            "3",    "--sub_setting_shared.c",
            "4",    "--v",
            "5.0",  "6.0",
            "--m",  "7.0",
            "8.0",  "9.0",
            "10.0", "--vec_float",
            "1.0",  "2.0",
            "4.0",  "--p.first",
            "500",  "--p.second",
            "600",  "--color",
            "blue",
        };
        int argc = sizeof(argv) / sizeof(argv[0]);
        std::cout << "FromCommandLine with " << argc << " arguments: \n";
        for (auto arg: argv) { std::cout << arg << " "; }
        std::cout << std::endl;
        setting.FromCommandLine(argc, argv);
        std::cout << setting << std::endl;
        ASSERT_EQ(setting.a, 10);
        ASSERT_EQ(setting.b, 1.0);
        ASSERT_EQ(setting.sub_setting.c, 2);
        ASSERT_EQ(setting.sub_setting.d, 3);
        ASSERT_EQ(setting.sub_setting_shared->c, 4);
        ASSERT_EQ(setting.v[0], 5.0);
        ASSERT_EQ(setting.v[1], 6.0);
        ASSERT_EQ(setting.m(0, 0), 7.0);
        ASSERT_EQ(setting.m(1, 0), 8.0);
        ASSERT_EQ(setting.m(0, 1), 9.0);
        ASSERT_EQ(setting.m(1, 1), 10.0);
        ASSERT_EQ(setting.vec_float.size(), 3);
        ASSERT_EQ(setting.vec_float[0], 1.0f);
        ASSERT_EQ(setting.vec_float[1], 2.0f);
        ASSERT_EQ(setting.vec_float[2], 4.0f);
        ASSERT_EQ(setting.p.first, 500);
        ASSERT_EQ(setting.p.second, 600);
        ASSERT_EQ(setting.color, Color::kBlue);
    }
}

TEST(YamlTest, EigenConversion) {
    Eigen::Vector3d v;
    v << 1, 2, 3;

    const YAML::Node node(v);
    std::cout << node << std::endl;
    ASSERT_STREQ(YAML::Dump(node).c_str(), "- 1\n- 2\n- 3");
}

TEST(YamlTest, EnumConversion) {

    std::cout << type_name(Color::kRed) << std::endl;
    auto color = static_cast<Color>(0);
    std::cout << type_name(color) << std::endl;

    YAML::Node node;
    node["color"] = Color::kRed;  // does not work unless we define struct convert<Color>
    std::cout << node << std::endl;
}

TEST(YamlTest, RecursiveLoad) {
    {
        Setting setting;
        setting.a = 42;
        setting.AsYamlFile("base.yaml");

        SubSetting sub_setting;
        sub_setting.d = 10;
        sub_setting.AsYamlFile("sub_setting_base.yaml");
    }

    {
        std::string yaml_str = R"(__base__: base.yaml
b: 2.5
sub_setting:
    c: 7
sub_setting_shared:
    __base__: sub_setting_base.yaml
    c: 20
color: blue
)";
        std::ofstream out("setting.yaml");
        out << yaml_str;
        out.close();
    }

    Setting setting;
    ASSERT_TRUE(setting.FromYamlFile("setting.yaml"));
    std::cout << setting << std::endl;
    ASSERT_EQ(setting.a, 42);                      // from base.yaml
    ASSERT_EQ(setting.b, 2.5);                     // overridden
    ASSERT_EQ(setting.sub_setting.c, 7);           // overridden
    ASSERT_EQ(setting.sub_setting.d, 1);           // from base.yaml
    ASSERT_EQ(setting.sub_setting_shared->c, 20);  // overridden
    ASSERT_EQ(setting.sub_setting_shared->d, 10);  // from sub_setting_base.yaml
    ASSERT_EQ(setting.color, Color::kBlue);        // overridden
}
