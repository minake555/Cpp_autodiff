#pragma once
#ifndef _EIGEN1_H_
#define _EIGEN1_H_

#include <vector>
#include <iostream>

namespace Eigen1
{
	template<typename T>
	class Matrix2x
	{
	private:
		int row;//��
		int col;//��
		std::vector<std::vector<T>> data;

		double zero_rate = 0.000001;//����̫С����Ϊ0

	public:
		//Ĭ�ϳ�ʼ��
		Matrix2x()
		{
			this->row = 0;
			this->col = 0;
		}

		//���г�ʼ��
		Matrix2x(int size_row, int size_col)
		{
			this->row = size_row;
			this->col = size_col;
			for (int i = 0; i < row; i++)
			{
				data.push_back(std::vector<T>(this->col, 0));
			}
		}

		//�������캯��
		Matrix2x(const Matrix2x<T>& a)
		{
			this->row = a.row;
			this->col = a.col;
			this->data = a.data;
		}

		//������ֵΪ��ķ���
		Matrix2x(int n)
		{
			this->row = n;
			this->col = n;
			for (int i = 0; i < this->row; i++)
			{
				data.push_back(std::vector<T>(this->col, 0));
			}
		}

		//���ڷ����쵥λ����
		Matrix2x(int n,char c)
		{
			this->row = n;
			this->col = n;
			for (int i = 0; i < this->row; i++)
			{
				data.push_back(std::vector<T>(this->col, 0));
			}
			if (c == 'I')
			{
				for (int i = 0; i < n; i++)
				{
					data[i][i] = 1;
				}
			}
			else
			{
				std::cout << "�������Ͳ�����Ĭ�Ϸ��������" << std::endl;
			}
		}

		//����[]
		std::vector<T>& operator [](int row)
		{
			if (row > this->row)
			{
				std::cout << "�±곬��" << std::endl;
				std::vector<T>temp(this->row, 0);
				return temp;
			}
			else
			{
				return this->data[row];
			}
		}
		const std::vector<T> operator [](int row)const
		{
			if (row > this->row)
			{
				std::cout << "�±곬��" << std::endl;
				return std::vector<T>(this->row, 0);
			}
			else
			{
				return this->data[row];
			}
		}

		//����+
		friend Matrix2x<T> operator +(const Matrix2x<T>& a, const Matrix2x<T>& b)
		{
			if (a.row != b.row && a.col != b.col)
			{
				std::cout << "����ӷ�ʧЧ��������������������Ƿ�һ��" << std::endl;
				return a;
			}
			else
			{
				Matrix2x<T>ends(a.row, a.col);
				for (int i = 0; i < a.row; i++)
				{
					for (int j = 0; j < a.col; j++)
					{
						ends[i][j] = a.data[i][j] + b.data[i][j];
					}
				}
				return ends;
			}
		}

		//����-
		friend Matrix2x<T> operator -(const Matrix2x<T>& a, const Matrix2x<T>& b)
		{
			if (a.row != b.row && a.col != b.col)
			{
				std::cout << "�������ʧЧ��������������������Ƿ�һ��" << std::endl;
				return a;
			}
			else
			{
				Matrix2x<T>ends(a.row, a.col);
				for (int i = 0; i < a.row; i++)
				{
					for (int j = 0; j < a.col; j++)
					{
						ends[i][j] = a.data[i][j] - b.data[i][j];
					}
				}
				return ends;
			}
		}

		//����*������*����
		friend Matrix2x<T> operator * (const Matrix2x<T>& a, const Matrix2x<T>& b)
		{
			if (b.row != a.col)
			{
				std::cout << "����˷�ʧЧ������ǰ�����������������Ƿ���ͬ" << std::endl;
				return a;
			}
			else
			{
				Matrix2x ends(a.row, b.col);
				int ends_row = a.row;
				int ends_col = b.col;
				for (int i = 0; i < ends_row; i++)
				{
					for (int j = 0; j < ends_col; j++)
					{
						T temp = 0;
						for (int k = 0; k < a.col; k++)
						{
							T tempa = a[i][k];
							T tempb = b[k][j];
							temp += a[i][k] * b[k][j];
						}
						ends[i][j] = temp;
					}
				}
				return ends;
			}
		}

		//����*������*����
		template<typename U>
		friend Matrix2x<T> operator *(U a, const Matrix2x<T>& b)
		{
			a = T(a);
			Matrix2x<T>ends(b);
			for (int i = 0; i < ends.row; i++)
			{
				for (int j = 0; j < ends.col; j++)
				{
					auto temp = ends[i][j] * a;
					ends[i][j] = temp;
				}
			}
			return ends;
		}

		//����*������*����
		template<typename U>
		friend Matrix2x<T> operator *(const Matrix2x<T>& b, U a)
		{
			return a * b;
		}

		//��������
		Matrix2x<T> inv()
		{
			if (this->row != this->col)
			{
				std::cout << "����Ϊ�����޷�ʵ�־�������" << std::endl;
				return *this;
			}
			else
			{
				int count = 0;
				for (int i = 0; i < row; i++)
				{
					if (abs(this->data[i][0]) < zero_rate)
					{
						count++;
					}
				}
				if (count == this->row)
				{
					std::cout << "�����ȿ����޷�ʵ�־�������" << std::endl;
					return *this;
				}
				else
				{
					Matrix2x temp1(this->row, this->col * 2);
					for (int i = 0; i < this->row; i++)
					{
						for (int j = 0; j < this->col; j++)
						{
							temp1[i][j] = this->data[i][j];
						}
					}
					for (int i = 0; i < this->row; i++)
					{
						temp1[i][i + this->col] = 1;
					}

					for (int i = 0; i < this->col; i++)
					{
						for (int j = i; j < this->row; j++)
						{
							if (abs(temp1[j][i]) < zero_rate)
							{
								for (int k = j; k < this->row; k++)
								{
									if (abs(temp1[k][i]) > zero_rate)
										swap(temp1.data[j], temp1.data[k]);
								}
							}
						}

						T temp2 = temp1[i][i];
						for (int j = 0; j < this->col * 2; j++)
						{
							temp1[i][j] = temp1[i][j] / temp2;
						}
						std::vector<T>temp4 = temp1.data[i];
						for (int j = 0; j < this->row; j++)
						{
							if (j == i)
								continue;
							else
							{
								T temp3 = temp1[j][i];
								for (int k = 0; k < this->col * 2; k++)
								{
									temp1[j][k] = temp1[j][k] - temp3 * temp4[k];
								}
							}
						}


						for (int j = i + 1; j < this->col; j++)
						{
							int count = 0;
							for (int k = 0; k < this->row; k++)
							{
								if (abs(temp1[k][j]) < zero_rate)
									count++;
							}
							if (count == this->row)
							{
								std::cout << "�����ȿ����޷�ʵ�־�������" << std::endl;
								return *this;
							}
						}
					}

					Matrix2x<T>ends(this->row, this->col);
					for (int i = 0; i < this->row; i++)
					{
						for (int j = 0; j < this->col; j++)
						{
							ends[i][j] = temp1[i][j + this->col];
						}
					}
					return ends;
				}
			}
		}

		//����չʾ
		void show()
		{
			for (int i = 0; i < this->col; i++)
			{
				for (int j = 0; j < this->col; j++)
				{
					std::cout << this->data[i][j] << " ";
				}
				std::cout << std::endl;
			}
		}
	};
}

#endif // !_EIGEN1_H_
