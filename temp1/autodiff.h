#pragma once
#ifndef _AUTODIFF_H_
#define _AUTODIFF_H_

#include <iostream>
#include <utility>
#include <vector>
#include <set>
#include <unordered_set>

//˼·�������ģ�����һ��var�����Զ�΢�ֵķ���ģʽ�Ļ���������Ȼ������ڲ�����һ��node��Ϊ����ͼ�Ľڵ㡣
//���ظ���������ţ�����ӷ���c=a+b����ô����������ó��з��أ��������������������Ľ��Ҳ�浽a��b�С�
//���磬c��a�ĵ�����1����1����a�Ľڵ�ĵ�����,bҲ��ͬ��
//�ڽڵ��ж���һ��vector,������������var�йص��ӽڵ�ȫ����������ע������Ҫ��ָ��������ã���Ϊ���var�п���Ҳ����ͬ���ӽڵ㡣

//var�ĳ�ʼ�������������ģ��ڲ�����һ��node��Ȼ������һ��node��ָ�롣var��ʼ����������һ��nodeָ�룬������ֵ���뵽node�С�

namespace AD
{
	template<typename T>
	class Var
	{
	private:

		//����ͼ�Ľڵ�
		class node;	

		//����ͼ�Ľڵ��ָ��
		typedef std::shared_ptr<node> node_ptr;	
		


		//����ͼ�Ľڵ��ʵ��
		class node
		{
		public:

			//ֵ
			T value;

			//����
			T deriv;

			//node_ptr�����ӽڵ㣬T��������������ڵ���ӽڵ�ĵ���
			std::vector<std::pair<node_ptr, T>>node_series;		

			//���򴫲�
			void backward()
			{
				for (std::pair<node_ptr, T> p : node_series)
				{
					p.first->deriv += deriv * p.second;
				}
			}

			//�ݶȹ���
			void deriv_zero()
			{
				deriv = 0.0;
			}

			//Ĭ�ϵ��������캯��
			node(T input_value) :value(input_value), deriv(0.0) {};

			//Ĭ��˫�������캯��
			node(T input_value, T input_deriv) :value(input_value), deriv(input_deriv) {};
		};

	public:
		//��װһ��nodeָ��
		node_ptr nodeptr;

		//��������ʼ��,��make_share����һ��node�ڵ㣬����value��Ϊ��������node���������캯��
		Var(T input_value) :nodeptr(std::make_shared<node>(input_value)) {};

		//������ֵ
		T get_value()
		{
			return nodeptr->value;
		}

		//���ص���
		T get_deriv()
		{
			return nodeptr->deriv;
		}

		//�趨�ݶ�
		void set_deriv(T input_deriv)
		{
			nodeptr->deriv = input_deriv;
		}

		//���򴫲�
		void backward()
		{
			//�������ӽڵ�
			std::vector<node_ptr>sorted_node;

			//�����ӽڵ������
			std::unordered_set<node*>visited;

			//��������
			var_topo_sort(nodeptr, sorted_node, visited);

			nodeptr->deriv = 1.0;

			for (auto i = sorted_node.rbegin(); i != sorted_node.rend(); i++)
			{
				(*i)->backward();
			}

		}

		//+���������
		friend Var<T> operator+(Var<T> a, Var<T> b)
		{
			Var<T> ends(a.get_value() + b.get_value());
			ends.nodeptr->node_series.push_back(std::make_pair(a.nodeptr, 1.0));
			ends.nodeptr->node_series.push_back(std::make_pair(b.nodeptr, 1.0));
			return ends;
		}

		//-���������
		friend Var<T> operator-(Var<T> a, Var<T> b)
		{
			Var<T> ends(a.get_value() - b.get_value());
			ends.nodeptr->node_series.push_back(std::make_pair(a.nodeptr, 1.0));
			ends.nodeptr->node_series.push_back(std::make_pair(b.nodeptr, -1.0));
			return ends;
		}

		//*���������
		friend Var<T> operator*(Var<T> a, Var<T> b)
		{
			Var<T> ends(a.get_value() * b.get_value());
			ends.nodeptr->node_series.push_back(std::make_pair(a.nodeptr, b.nodeptr->value));
			ends.nodeptr->node_series.push_back(std::make_pair(b.nodeptr, a.nodeptr->value));
			return ends;
		}

		///���������
		friend Var<T> operator/(Var<T> a, Var<T> b)
		{
			Var<T> ends(a.get_value() / b.get_value());
			ends.nodeptr->node_series.push_back(std::make_pair(a.nodeptr, 1.0 / b.nodeptr->value));
			ends.nodeptr->node_series.push_back(std::make_pair(b.nodeptr, -a.nodeptr->value / (b.nodeptr->value * b.nodeptr->value)));
			return ends;
		}

		//��������
		friend Var<T> operator-(Var<T> a)
		{
			Var<T> ends(-a.get_value());
			ends.nodeptr->node_series.push_back(std::make_pair(a.nodeptr, -1.0));
		}

		//��ѧ����
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

		//���չʾ
		void show()
		{
			std::cout << "value:" << nodeptr->value << "\t" << "deriv:" << nodeptr->deriv << std::endl;
		}

	private:
		//����������ʵ����������㷨��Ȼ����浹��backward����������ʵ���Զ��󵼵ĺ���ģʽ
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

		//�����ݶ�ֵ
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
