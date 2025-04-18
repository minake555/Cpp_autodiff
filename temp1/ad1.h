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

        // ����ͼ�ڵ�ṹ
        struct Node {
            double value;  // �ڵ�ֵ
            double grad;   // �ݶ�ֵ
            std::vector<std::pair<NodePtr, double>> children; // �ӽڵ�;ֲ�����
            std::function<void()> backward_fn;  // �Զ��巴�򴫲�����

            Node(double val) : value(val), grad(0.0) {}

            // ���򴫲�ʵ��
            void backward() {
                // ������Զ��巴����������ʹ��
                if (backward_fn) {
                    backward_fn();
                    return;
                }

                // ��׼���򴫲�
                for (std::pair<NodePtr, double>temp : children) {
                    temp.first->grad += grad * temp.second;
                }
            }

            // ����ݶ�
            void zero_grad() {
                grad = 0.0;
            }
        };

        NodePtr node;

    public:
        // ���캯��
        explicit Var(double value = 0.0) : node(std::make_shared<Node>(value)) {}

        // ��ȡֵ
        double value() const { return node->value; }

        // ��ȡ�ݶ�
        double grad() const { return node->grad; }

        // �����ݶ�
        void set_grad(double g) { node->grad = g; }

        // ���򴫲�
        void backward() {
            // ��������ȷ����ȷ����˳��
            std::vector<NodePtr> sorted_nodes;
            std::unordered_set<Node*> visited;
            build_topo_sort(node, sorted_nodes, visited);

            // ��ʼ������ݶ�Ϊ1
            node->grad = 1.0;

            // ��������ݶ�
            for (auto it = sorted_nodes.rbegin(); it != sorted_nodes.rend(); ++it) {
                (*it)->backward();
            }
        }

        // �����ݶ�
        void zero_grad() {
            std::unordered_set<Node*> visited;
            zero_grad(node, visited);
        }

        // ���������
        friend Var operator+(Var a, Var b);
        friend Var operator-(Var a, Var b);
        friend Var operator*(Var a, Var b);
        friend Var operator/(Var a, Var b);
        friend Var operator-(Var x);

        // ��ѧ����
        friend Var sin(Var x);
        friend Var cos(Var x);
        friend Var exp(Var x);
        friend Var log(Var x);
        friend Var pow(Var x, Var y);

    private:
        // ������������
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

        // �ݹ������ݶ�
        static void zero_grad(NodePtr node, std::unordered_set<Node*>& visited) {
            if (visited.find(node.get()) != visited.end()) return;
            visited.insert(node.get());

            node->zero_grad();
            for (std::pair<NodePtr, double>temp : node->children) {
                zero_grad(temp.first, visited);
            }
        }
    };

    // ���������ʵ��
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

    // ��ѧ����ʵ��
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