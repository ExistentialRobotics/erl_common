#include "erl_common/test_helper.hpp"
#include "erl_common/yaml.hpp"

#include <iostream>

namespace erl::test {

    struct SubSetting : common::Yamlable<SubSetting> {
        int c = 0;
        int d = 1;
    };

    struct Setting : common::Yamlable<Setting> {
        int a = 0;
        double b = 3.0;
        SubSetting sub_setting;
    };
}  // namespace erl::test

namespace YAML {

    template<>
    struct convert<erl::test::SubSetting> {
        static Node
        encode(const erl::test::SubSetting& rhs) {
            Node node;
            node["c"] = rhs.c;
            node["d"] = rhs.d;
            return node;
        }

        static bool
        decode(const Node& node, erl::test::SubSetting& rhs) {
            if (!node.IsMap()) { return false; }
            rhs.c = node["c"].as<int>();
            rhs.d = node["d"].as<int>();
            return true;
        }
    };

    template<>
    struct convert<erl::test::Setting> {
        static Node
        encode(const erl::test::Setting& rhs) {
            Node node;
            node["a"] = rhs.a;
            node["b"] = rhs.b;
            node["sub_setting"] = rhs.sub_setting;
            return node;
        }

        static bool
        decode(const Node& node, erl::test::Setting& rhs) {
            if (!node.IsMap()) { return false; }
            rhs.a = node["a"].as<int>();
            rhs.b = node["b"].as<double>();
            rhs.sub_setting = node["sub_setting"].as<erl::test::SubSetting>();
            return true;
        }
    };

    inline Emitter&
    operator<<(Emitter& out, const erl::test::SubSetting& sub_setting) {
        out << BeginMap;
        out << Key << "c" << Value << sub_setting.c;
        out << Key << "d" << Value << sub_setting.d;
        out << EndMap;
        return out;
    }

    inline Emitter&
    operator<<(Emitter& out, const erl::test::Setting& setting) {
        out << BeginMap;
        out << Key << "a" << Value << setting.a;
        out << Key << "b" << Value << setting.b;
        out << Key << "sub_setting" << Value << setting.sub_setting;
        out << EndMap;
        return out;
    }
}  // namespace YAML

TEST(YamlTest, FromYamlString) {
    erl::test::Setting setting;
    ASSERT_EQ(setting.a, 0);
    ASSERT_EQ(setting.b, 3.0);
    ASSERT_EQ(setting.sub_setting.c, 0);
    ASSERT_EQ(setting.sub_setting.d, 1);
    std::cout << setting << std::endl;

    ASSERT_TRUE(setting.FromYamlString(R"(
a: 10
b: 1.0
sub_setting:
    c: 2
    d: 3)"));

    std::cout << setting << std::endl;
    ASSERT_EQ(setting.a, 10);
    ASSERT_EQ(setting.b, 1.0);
    ASSERT_EQ(setting.sub_setting.c, 2);
    ASSERT_EQ(setting.sub_setting.d, 3);
}

TEST(YamlTest, EigenConversion) {
    Eigen::Vector3d v;
    v << 1, 2, 3;

    const YAML::Node node(v);
    std::cout << node << std::endl;
    ASSERT_STREQ(YAML::Dump(node).c_str(), "- 1\n- 2\n- 3");
}

enum class Color {
    kRed = 0,
    kBlue = 1,
};

template<>
struct YAML::convert<Color> {
    static Node
    encode(const Color& rhs) {
        Node node;
        switch (rhs) {
            case Color::kRed:
                node = "red";
                break;
            case Color::kBlue:
                node = "blue";
                break;
            default:
                throw std::runtime_error("Unknown color");
        }
        return node;
    }

    static bool
    decode(const Node& node, Color& rhs) {
        auto color_str = node.as<std::string>();
        if (color_str == "red") {
            rhs = Color::kRed;
        } else if (color_str == "blue") {
            rhs = Color::kBlue;
        } else {
            return false;
        }
        return true;
    }
};

TEST(YamlTest, EnumConversion) {

    std::cout << type_name(Color::kRed) << std::endl;
    auto color = static_cast<Color>(0);
    std::cout << type_name(color) << std::endl;

    YAML::Node node;
    node["color"] = Color::kRed;  // does not work unless we define struct convert<Color>
    std::cout << node << std::endl;
}
