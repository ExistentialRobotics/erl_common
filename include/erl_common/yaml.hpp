#pragma once

#include <yaml-cpp/yaml.h>

#include <fstream>
#include <optional>
#include <memory>
#include <filesystem>

#include "assert.hpp"
#include "eigen.hpp"
#include "opencv.hpp"

// https://yaml.org/spec/1.2.2/
// https://www.cloudbees.com/blog/yaml-tutorial-everything-you-need-get-started

namespace erl::common {

    struct YamlableBase {
        virtual ~YamlableBase() = default;

        virtual void
        FromYamlNode(const YAML::Node& node) = 0;

        inline void
        FromYamlString(const std::string& yaml_string) {
            auto node = YAML::Load(yaml_string);
            FromYamlNode(node);
        }

        [[nodiscard]] virtual YAML::Node
        AsYamlNode() const = 0;

        [[nodiscard]] virtual std::string
        AsYamlString() const = 0;

        inline void
        FromYamlFile(const std::string& yaml_file) {
            auto node = YAML::LoadFile(yaml_file);
            FromYamlNode(node);
        }

        inline void
        AsYamlFile(const std::string& yaml_file) const {
            std::ofstream ofs(yaml_file);
            if (!ofs.is_open()) { throw std::runtime_error("Failed to open file: " + yaml_file); }
            YAML::Emitter emitter(ofs);
            emitter << AsYamlNode();
        }
    };

    template<typename T>
    struct Yamlable : public YamlableBase {

        inline void
        FromYamlNode(const YAML::Node& node) override {
            YAML::convert<T>::decode(node, *static_cast<T*>(this));
        }

        [[nodiscard]] inline YAML::Node
        AsYamlNode() const override {
            return YAML::convert<T>::encode(*static_cast<const T*>(this));
        }

        [[nodiscard]] inline std::string
        AsYamlString() const override {
            return YAML::Dump(AsYamlNode());
        }
    };

    /**
     * @brief Override the Yamlable interface when the class is derived from a Yamlable class.
     * @tparam Base The base class that is derived from Yamlable<Base>.
     * @tparam T The derived class that is derived from Base.
     */
    template<typename Base, typename T>
    struct OverrideYamlable : public Base {
        inline void
        FromYamlNode(const YAML::Node& node) override {
            YAML::convert<T>::decode(node, *static_cast<T*>(this));
        }

        [[nodiscard]] inline YAML::Node
        AsYamlNode() const override {
            return YAML::convert<T>::encode(*static_cast<const T*>(this));
        }
    };
}  // namespace erl::common

namespace YAML {
    template<typename T, int Rows = Eigen::Dynamic, int Cols = Eigen::Dynamic, int Order = Eigen::ColMajor>
    struct ConvertEigenMatrix {
        inline static Node
        encode(const Eigen::Matrix<T, Rows, Cols, Order>& rhs) {
            Node node(NodeType::Sequence);
            int rows = Rows == Eigen::Dynamic ? rhs.rows() : Rows;
            int cols = Cols == Eigen::Dynamic ? rhs.cols() : Cols;

            if (Order == Eigen::RowMajor) {
                for (int i = 0; i < rows; ++i) {
                    Node row_node(NodeType::Sequence);
                    for (int j = 0; j < cols; ++j) { row_node.push_back(rhs(i, j)); }
                    node.push_back(row_node);
                }
            } else {
                for (int j = 0; j < cols; ++j) {
                    Node col_node(NodeType::Sequence);
                    for (int i = 0; i < rows; ++i) { col_node.push_back(rhs(i, j)); }
                    node.push_back(col_node);
                }
            }

            return node;
        }

        inline static bool
        decode(const Node& node, Eigen::Matrix<T, Rows, Cols, Order>& rhs) {
            if (node.IsNull() && (Rows == Eigen::Dynamic || Cols == Eigen::Dynamic)) { return true; }
            if (!node.IsSequence()) { return false; }
            if (!node[0].IsSequence()) { return false; }

            if (Order == Eigen::RowMajor) {
                int rows = Rows == Eigen::Dynamic ? node.size() : Rows;
                int cols = Cols == Eigen::Dynamic ? node[0].size() : Cols;
                rhs.resize(rows, cols);
                ERL_DEBUG_ASSERT(rows == int(node.size()), "expecting rows: %d, get node.size(): %lu", rows, node.size());
                for (int i = 0; i < rows; ++i) {
                    ERL_DEBUG_ASSERT(cols == int(node[i].size()), "expecting cols: %d, get node[0].size(): %lu", cols, node[i].size());
                    auto& kRowNode = node[i];
                    for (int j = 0; j < cols; ++j) { rhs(i, j) = kRowNode[j].as<T>(); }
                }
            } else {
                int cols = Cols == Eigen::Dynamic ? node.size() : Cols;
                int rows = Rows == Eigen::Dynamic ? node[0].size() : Rows;
                rhs.resize(rows, cols);
                ERL_DEBUG_ASSERT(cols == int(node.size()), "expecting cols: %d, get node.size(): %lu", cols, node.size());
                for (int j = 0; j < cols; ++j) {
                    ERL_DEBUG_ASSERT(rows == int(node[j].size()), "expecting rows: %d, get node[0].size(): %lu", rows, node[j].size());
                    auto& kColNode = node[j];
                    for (int i = 0; i < rows; ++i) { rhs(i, j) = kColNode[i].as<T>(); }
                }
            }

            return true;
        }
    };

    template<>
    struct convert<Eigen::Matrix2i> : public ConvertEigenMatrix<int, 2, 2> {};

    template<>
    struct convert<Eigen::Matrix2f> : public ConvertEigenMatrix<float, 2, 2> {};

    template<>
    struct convert<Eigen::Matrix2d> : public ConvertEigenMatrix<double, 2, 2> {};

    template<>
    struct convert<Eigen::Matrix2Xi> : public ConvertEigenMatrix<int, 2, Eigen::Dynamic> {};

    template<>
    struct convert<Eigen::Matrix2Xf> : public ConvertEigenMatrix<float, 2, Eigen::Dynamic> {};

    template<>
    struct convert<Eigen::Matrix2Xd> : public ConvertEigenMatrix<double, 2, Eigen::Dynamic> {};

    template<>
    struct convert<Eigen::MatrixX2i> : public ConvertEigenMatrix<int, Eigen::Dynamic, 2> {};

    template<>
    struct convert<Eigen::MatrixX2f> : public ConvertEigenMatrix<float, Eigen::Dynamic, 2> {};

    template<>
    struct convert<Eigen::MatrixX2d> : public ConvertEigenMatrix<double, Eigen::Dynamic, 2> {};

    template<>
    struct convert<Eigen::Matrix3i> : public ConvertEigenMatrix<int, 3, 3> {};

    template<>
    struct convert<Eigen::Matrix3f> : public ConvertEigenMatrix<float, 3, 3> {};

    template<>
    struct convert<Eigen::Matrix3d> : public ConvertEigenMatrix<double, 3, 3> {};

    template<>
    struct convert<Eigen::Matrix3Xi> : public ConvertEigenMatrix<int, 3, Eigen::Dynamic> {};

    template<>
    struct convert<Eigen::Matrix3Xf> : public ConvertEigenMatrix<float, 3, Eigen::Dynamic> {};

    template<>
    struct convert<Eigen::Matrix3Xd> : public ConvertEigenMatrix<double, 3, Eigen::Dynamic> {};

    template<>
    struct convert<Eigen::MatrixX3i> : public ConvertEigenMatrix<int, Eigen::Dynamic, 3> {};

    template<>
    struct convert<Eigen::MatrixX3f> : public ConvertEigenMatrix<float, Eigen::Dynamic, 3> {};

    template<>
    struct convert<Eigen::MatrixX3d> : public ConvertEigenMatrix<double, Eigen::Dynamic, 3> {};

    template<>
    struct convert<Eigen::Matrix4i> : public ConvertEigenMatrix<int, 4, 4> {};

    template<>
    struct convert<Eigen::Matrix4f> : public ConvertEigenMatrix<float, 4, 4> {};

    template<>
    struct convert<Eigen::Matrix4d> : public ConvertEigenMatrix<double, 4, 4> {};

    template<>
    struct convert<Eigen::Matrix4Xi> : public ConvertEigenMatrix<int, 4, Eigen::Dynamic> {};

    template<>
    struct convert<Eigen::Matrix4Xf> : public ConvertEigenMatrix<float, 4, Eigen::Dynamic> {};

    template<>
    struct convert<Eigen::Matrix4Xd> : public ConvertEigenMatrix<double, 4, Eigen::Dynamic> {};

    template<>
    struct convert<Eigen::MatrixX4i> : public ConvertEigenMatrix<int, Eigen::Dynamic, 4> {};

    template<>
    struct convert<Eigen::MatrixX4f> : public ConvertEigenMatrix<float, Eigen::Dynamic, 4> {};

    template<>
    struct convert<Eigen::MatrixX4d> : public ConvertEigenMatrix<double, Eigen::Dynamic, 4> {};

    template<typename T, int Size = Eigen::Dynamic>
    struct ConvertEigenVector {
        inline static Node
        encode(const Eigen::Vector<T, Size>& rhs) {
            Node node(NodeType::Sequence);
            if (Size == Eigen::Dynamic) {
                for (int i = 0; i < rhs.size(); ++i) { node.push_back(rhs[i]); }
            } else {
                for (int i = 0; i < Size; ++i) { node.push_back(rhs[i]); }
            }
            return node;
        }

        inline static bool
        decode(const Node& node, Eigen::Vector<T, Size>& rhs) {
            if (!node.IsSequence()) { return false; }
            if (Size == Eigen::Dynamic) {
                rhs.resize(node.size());
                for (int i = 0; i < rhs.size(); ++i) { rhs[i] = node[i].as<T>(); }
            } else {
                for (int i = 0; i < Size; ++i) { rhs[i] = node[i].as<T>(); }
            }
            return true;
        }
    };

    template<>
    struct convert<Eigen::VectorXd> : public ConvertEigenVector<double, Eigen::Dynamic> {};

    template<>
    struct convert<Eigen::Vector2d> : public ConvertEigenVector<double, 2> {};

    template<>
    struct convert<Eigen::Vector3d> : public ConvertEigenVector<double, 3> {};

    template<>
    struct convert<Eigen::Vector4d> : public ConvertEigenVector<double, 4> {};

    template<>
    struct convert<Eigen::VectorXf> : public ConvertEigenVector<float, Eigen::Dynamic> {};

    template<>
    struct convert<Eigen::Vector2f> : public ConvertEigenVector<float, 2> {};

    template<>
    struct convert<Eigen::Vector3f> : public ConvertEigenVector<float, 3> {};

    template<>
    struct convert<Eigen::Vector4f> : public ConvertEigenVector<float, 4> {};

    template<>
    struct convert<Eigen::VectorXi> : public ConvertEigenVector<int, Eigen::Dynamic> {};

    template<>
    struct convert<Eigen::Vector2i> : public ConvertEigenVector<int, 2> {};

    template<>
    struct convert<Eigen::Vector3i> : public ConvertEigenVector<int, 3> {};

    template<>
    struct convert<Eigen::Vector4i> : public ConvertEigenVector<int, 4> {};

    template<typename T>
    struct convert<std::optional<T>> {
        inline static Node
        encode(const std::optional<T>& rhs) {
            if (rhs) {
                return convert<T>::encode(*rhs);
            } else {
                return Node(NodeType::Null);
            }
        }

        inline static bool
        decode(const Node& node, std::optional<T>& rhs) {
            if (node.Type() != NodeType::Null) {
                T value;
                if (convert<T>::decode(node, value)) {
                    rhs = value;
                    return true;
                } else {
                    return false;
                }
            } else {
                rhs = std::nullopt;
                return true;
            }
        }
    };

    template<typename T>
    struct convert<std::shared_ptr<T>> {
        inline static Node
        encode(const std::shared_ptr<T>& rhs) {
            if (rhs == nullptr) { return Node(NodeType::Null); }
            return convert<T>::encode(*rhs);
        }

        inline static bool
        decode(const Node& node, std::shared_ptr<T>& rhs) {
            ERL_DEBUG_ASSERT(rhs == nullptr, "rhs should not be nullptr.");
            auto value = std::make_shared<T>();
            if (convert<T>::decode(node, *value)) {
                rhs = value;
                return true;
            } else {
                return false;
            }
        }
    };

    template<typename T>
    struct convert<std::unique_ptr<T>> {
        inline static Node
        encode(const std::unique_ptr<T>& rhs) {
            if (rhs == nullptr) { return Node(NodeType::Null); }
            return convert<T>::encode(*rhs);
        }

        inline static bool
        decode(const Node& node, std::unique_ptr<T>& rhs) {
            ERL_DEBUG_ASSERT(rhs == nullptr, "rhs should not be nullptr.");
            auto value = std::make_unique<T>();
            if (convert<T>::decode(node, *value)) {
                rhs = std::move(value);
                return true;
            } else {
                return false;
            }
        }
    };

    template<typename KeyType, typename ValueType>
    struct convert<std::unordered_map<KeyType, ValueType>> {
        inline static Node
        encode(const std::unordered_map<KeyType, ValueType>& rhs) {
            Node node(NodeType::Map);
            for (const auto& [key, value]: rhs) { node[convert<KeyType>::encode(key)] = convert<ValueType>::encode(value); }
            return node;
        }

        inline static bool
        decode(const Node& node, std::unordered_map<KeyType, ValueType>& rhs) {
            if (!node.IsMap()) { return false; }
            for (auto it = node.begin(); it != node.end(); ++it) {
                KeyType key;
                ValueType value;
                if (convert<KeyType>::decode(it->first, key) && convert<ValueType>::decode(it->second, value)) {
                    rhs[key] = value;
                } else {
                    return false;
                }
            }
            return true;
        }
    };

    template<>
    struct convert<cv::Scalar> {
        inline static Node
        encode(const cv::Scalar& rhs) {
            Node node(NodeType::Sequence);
            node.push_back(rhs[0]);
            node.push_back(rhs[1]);
            node.push_back(rhs[2]);
            node.push_back(rhs[3]);
            return node;
        }

        inline static bool
        decode(const Node& node, cv::Scalar& rhs) {
            if (!node.IsSequence()) { return false; }
            rhs[0] = node[0].as<double>();
            rhs[1] = node[1].as<double>();
            rhs[2] = node[2].as<double>();
            rhs[3] = node[3].as<double>();
            return true;
        }
    };
}  // namespace YAML

template<typename T>
inline std::enable_if_t<std::is_base_of_v<erl::common::YamlableBase, T>, std::ostream&>
operator<<(std::ostream& out, const T& yaml) {
    out << yaml.AsYamlString();
    return out;
}
