#include <iostream>

#include "eigen1.h"

#include "autodiff.h"

int main() 
{

    //using namespace autodiff;

    //// 创建变量
    //Var x(2.0);
    //Var y(3.0);

    //// 构建计算图
    //Var z = x * y + sin(x);

    //// 反向传播
    //z.backward();

    //// 输出结果
    //std::cout << "Function value: " << z.value() << std::endl;
    //std::cout << "Gradient wrt x: " << x.grad() << std::endl;
    //std::cout << "Gradient wrt y: " << y.grad() << std::endl;

    //// 重置计算图
    //x.zero_grad();
    //y.zero_grad();

    //// 另一个例子
    //Var a(1.0);
    //Var b(2.0);
    //Var c = exp(a * b) / (a + b);
    //c.backward();

    //std::cout << "\nSecond example:" << std::endl;
    //std::cout << "Function value: " << c.value() << std::endl;
    //std::cout << "Gradient wrt a: " << a.grad() << std::endl;
    //std::cout << "Gradient wrt b: " << b.grad() << std::endl;

    //Eigen1::Matrix2x<double>a(2, 2);
    //a[0][0] = 1;
    //a[0][1] = 2;
    //a[1][0] = 5;
    //a[1][1] = 6;
    //Eigen1::Matrix2x<double>b = a.inv();
    //b.show();

    using AD::Var;

    // 创建变量
    Var<double> x(2.0);
    Var<double> y(3.0);

    // 构建计算图
    Var<double> z = x * y + sin(x);

    // 反向传播
    z.backward();
    x.show();
    y.show();
    std::cout << std::cos(2) <<std:: endl;

	system("pause");
	return 0;
}