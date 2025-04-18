#pragma once

#include <iostream>
#include <cmath>
#include <memory>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <functional>

namespace autodiff {

    class Var {
        struct Node;
        using NodePtr = std::shared_ptr<Node>;

        // 计算图节点结构
        struct Node {
            double value;  // 节点值
            double grad;   // 梯度值
            std::vector<std::pair<NodePtr, double>> children; // 子节点和局部导数
            std::function<void()> backward_fn;  // 自定义反向传播函数

            Node(double val) : value(val), grad(0.0) {}

            // 反向传播实现
            void backward() {
                // 如果有自定义反向函数则优先使用
                if (backward_fn) {
                    backward_fn();
                    return;
                }

                // 标准反向传播
                for (std::pair<NodePtr, double>temp : children) {
                    temp.first->grad += grad * temp.second;
                }
            }

            // 清除梯度
            void zero_grad() {
                grad = 0.0;
            }
        };

        NodePtr node;

    public:
        // 构造函数
        explicit Var(double value = 0.0) : node(std::make_shared<Node>(value)) {}

        // 获取值
        double value() const { return node->value; }

        // 获取梯度
        double grad() const { return node->grad; }

        // 设置梯度
        void set_grad(double g) { node->grad = g; }

        // 反向传播
        void backward() {
            // 拓扑排序确保正确计算顺序
            std::vector<NodePtr> sorted_nodes;
            std::unordered_set<Node*> visited;
            build_topo_sort(node, sorted_nodes, visited);

            // 初始化输出梯度为1
            node->grad = 1.0;

            // 反向计算梯度
            for (auto it = sorted_nodes.rbegin(); it != sorted_nodes.rend(); ++it) {
                (*it)->backward();
            }
        }

        // 清零梯度
        void zero_grad() {
            std::unordered_set<Node*> visited;
            zero_grad(node, visited);
        }

        // 运算符重载
        friend Var operator+(Var a, Var b);
        friend Var operator-(Var a, Var b);
        friend Var operator*(Var a, Var b);
        friend Var operator/(Var a, Var b);
        friend Var operator-(Var x);

        // 数学函数
        friend Var sin(Var x);
        friend Var cos(Var x);
        friend Var exp(Var x);
        friend Var log(Var x);
        friend Var pow(Var x, Var y);

    private:
        // 构建拓扑排序
        static void build_topo_sort(NodePtr node,
            std::vector<NodePtr>& sorted,
            std::unordered_set<Node*>& visited) {
            if (visited.find(node.get()) != visited.end()) return;
            visited.insert(node.get());

            for (std::pair<NodePtr, double>temp : node->children) {
                build_topo_sort(temp.first, sorted, visited);
            }

            sorted.push_back(node);
        }

        // 递归清零梯度
        static void zero_grad(NodePtr node, std::unordered_set<Node*>& visited) {
            if (visited.find(node.get()) != visited.end()) return;
            visited.insert(node.get());

            node->zero_grad();
            for (std::pair<NodePtr, double>temp : node->children) {
                zero_grad(temp.first, visited);
            }
        }
    };

    // 运算符重载实现
    Var operator+(Var a, Var b) {
        Var result(a.node->value + b.node->value);
        result.node->children.emplace_back(a.node, 1.0);
        result.node->children.emplace_back(b.node, 1.0);
        return result;
    }

    Var operator-(Var a, Var b) {
        Var result(a.node->value - b.node->value);
        result.node->children.emplace_back(a.node, 1.0);
        result.node->children.emplace_back(b.node, -1.0);
        return result;
    }

    Var operator*(Var a, Var b) {
        Var result(a.node->value * b.node->value);
        result.node->children.emplace_back(a.node, b.node->value);
        result.node->children.emplace_back(b.node, a.node->value);
        return result;
    }

    Var operator/(Var a, Var b) {
        Var result(a.node->value / b.node->value);
        double b_val = b.node->value;
        result.node->children.emplace_back(a.node, 1.0 / b_val);
        result.node->children.emplace_back(b.node, -a.node->value / (b_val * b_val));
        return result;
    }

    Var operator-(Var x) {
        Var result(-x.node->value);
        result.node->children.emplace_back(x.node, -1.0);
        return result;
    }

    // 数学函数实现
    Var sin(Var x) {
        Var result(std::sin(x.node->value));
        result.node->children.emplace_back(x.node, std::cos(x.node->value));
        return result;
    }

    Var cos(Var x) {
        Var result(std::cos(x.node->value));
        result.node->children.emplace_back(x.node, -std::sin(x.node->value));
        return result;
    }

    Var exp(Var x) {
        Var result(std::exp(x.node->value));
        result.node->children.emplace_back(x.node, result.node->value);
        return result;
    }

    Var log(Var x) {
        Var result(std::log(x.node->value));
        result.node->children.emplace_back(x.node, 1.0 / x.node->value);
        return result;
    }

    Var pow(Var x, Var y) {
        Var result(std::pow(x.node->value, y.node->value));
        double x_val = x.node->value;
        double y_val = y.node->value;
        result.node->children.emplace_back(x.node, y_val * std::pow(x_val, y_val - 1));
        result.node->children.emplace_back(y.node, result.node->value * std::log(x_val));
        return result;
    }

} // namespace autodiff