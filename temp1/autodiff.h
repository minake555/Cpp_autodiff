#pragma once
#ifndef _AUTODIFF_H_
#define _AUTODIFF_H_

#include <iostream>
#include <utility>
#include <vector>
#include <set>
#include <unordered_set>

//思路是这样的，定义一个var代表自动微分的反向模式的基础变量。然后变量内部内置一个node作为有向图的节点。
//重载各类运算符号，例如加法，c=a+b。那么运算符号设置成有返回，重载运算符，将导数后的结果也存到a，b中。
//例如，c对a的导数是1，将1存在a的节点的导数中,b也是同理。
//在节点中定义一个vector,用来将跟当下var有关的子节点全部存起来，注意这里要用指针或者引用，因为别的var有可能也有相同的子节点。

//var的初始化过程是这样的，内部定义一个node，然后定义了一个node的指针。var初始化就是生成一个node指针，将输入值输入到node中。

namespace AD
{
	template<typename T>
	class Var
	{
	private:

		//有向图的节点
		class node;	

		//有向图的节点的指针
		typedef std::shared_ptr<node> node_ptr;	
		


		//有向图的节点的实现
		class node
		{
		public:

			//值
			T value;

			//导数
			T deriv;

			//node_ptr代表子节点，T代表现在这个父节点对子节点的导数
			std::vector<std::pair<node_ptr, T>>node_series;		

			//反向传播
			void backward()
			{
				for (std::pair<node_ptr, T> p : node_series)
				{
					p.first->deriv += deriv * p.second;
				}
			}

			//梯度归零
			void deriv_zero()
			{
				deriv = 0.0;
			}

			//默认单参数构造函数
			node(T input_value) :value(input_value), deriv(0.0) {};

			//默认双参数构造函数
			node(T input_value, T input_deriv) :value(input_value), deriv(input_deriv) {};
		};

	public:
		//封装一个node指针
		node_ptr nodeptr;

		//单参数初始化,用make_share构建一个node节点，并将value作为参数传入node单参数构造函数
		Var(T input_value) :nodeptr(std::make_shared<node>(input_value)) {};

		//返回数值
		T get_value()
		{
			return nodeptr->value;
		}

		//返回导数
		T get_deriv()
		{
			return nodeptr->deriv;
		}

		//设定梯度
		void set_deriv(T input_deriv)
		{
			nodeptr->deriv = input_deriv;
		}

		//反向传播
		void backward()
		{
			//排序后的子节点
			std::vector<node_ptr>sorted_node;

			//用于子节点的排序
			std::unordered_set<node*>visited;

			//拓扑排序
			var_topo_sort(nodeptr, sorted_node, visited);

			nodeptr->deriv = 1.0;

			for (auto i = sorted_node.rbegin(); i != sorted_node.rend(); i++)
			{
				(*i)->backward();
			}

		}

		//+运算符重载
		friend Var<T> operator+(Var<T> a, Var<T> b)
		{
			Var<T> ends(a.get_value() + b.get_value());
			ends.nodeptr->node_series.push_back(std::make_pair(a.nodeptr, 1.0));
			ends.nodeptr->node_series.push_back(std::make_pair(b.nodeptr, 1.0));
			return ends;
		}

		//-运算符重载
		friend Var<T> operator-(Var<T> a, Var<T> b)
		{
			Var<T> ends(a.get_value() - b.get_value());
			ends.nodeptr->node_series.push_back(std::make_pair(a.nodeptr, 1.0));
			ends.nodeptr->node_series.push_back(std::make_pair(b.nodeptr, -1.0));
			return ends;
		}

		//*运算符重载
		friend Var<T> operator*(Var<T> a, Var<T> b)
		{
			Var<T> ends(a.get_value() * b.get_value());
			ends.nodeptr->node_series.push_back(std::make_pair(a.nodeptr, b.nodeptr->value));
			ends.nodeptr->node_series.push_back(std::make_pair(b.nodeptr, a.nodeptr->value));
			return ends;
		}

		///运算符重载
		friend Var<T> operator/(Var<T> a, Var<T> b)
		{
			Var<T> ends(a.get_value() / b.get_value());
			ends.nodeptr->node_series.push_back(std::make_pair(a.nodeptr, 1.0 / b.nodeptr->value));
			ends.nodeptr->node_series.push_back(std::make_pair(b.nodeptr, -a.nodeptr->value / (b.nodeptr->value * b.nodeptr->value)));
			return ends;
		}

		//负号重载
		friend Var<T> operator-(Var<T> a)
		{
			Var<T> ends(-a.get_value());
			ends.nodeptr->node_series.push_back(std::make_pair(a.nodeptr, -1.0));
		}

		//数学函数
		friend Var<T> sin(Var<T> a)
		{
			Var<T>ends(std::sin(a.get_value()));
			ends.nodeptr->node_series.push_back(std::make_pair(a.nodeptr, std::cos(a.nodeptr->value)));
			return ends;
		}

		friend Var<T> cos(Var<T> a)
		{
			Var<T>ends(std::cos(a.get_value()));
			ends.nodeptr->node_series.push_back(std::make_pair(a.nodeptr, -std::sin(a.nodeptr->value)));
			return ends;
		}
		friend Var<T> exp(Var<T> a);
		friend Var<T> log(Var<T> a);
		friend Var<T> pow(Var<T> a, Var<T> b);

		//结果展示
		void show()
		{
			std::cout << "value:" << nodeptr->value << "\t" << "deriv:" << nodeptr->deriv << std::endl;
		}

	private:
		//拓扑排序，其实是深度优先算法，然后后面倒序backward，这样就能实现自动求导的后向模式
		void var_topo_sort(node_ptr input_node, std::vector<node_ptr>& sorted_node, std::unordered_set<node*>& visited)
		{
			if (visited.find(input_node.get()) != visited.end())
			{
				return;
			}
			else
			{
				visited.insert(input_node.get());
			}

			for (std::pair<node_ptr, T>temp : nodeptr->node_series)
			{
				var_topo_sort(temp.first, sorted_node, visited);
			}

			sorted_node.push_back(input_node);
		}

		//清零梯度值
		void var_deriv_zero(node_ptr input_node, std::unordered_set<node*>& visited)
		{
			if (visited.find(input_node.get()) != visited.end())
			{
				return;
			}
			else
			{
				visited.insert(input_node.get());
			}

			input_node->deriv_zero();

			for (std::pair<node_ptr, T>temp : nodeptr->node_series)
			{
				var_deriv_zero(temp.first, visited);
			}
		}

	};
}

#endif // !_AUTODIFF_H_
