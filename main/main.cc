#include <HighPrecision.hh>
#include <Windows.h>

int main()
{
    SetConsoleOutputCP(65001);
    HighPrecisionCalculator::HighPrecisionExpression exp1;
    while (1)
    {
        std::cout << "请输入要计算的表达式(输入exit退出)：" << std::endl;
        std::cin >> exp1;
        if (exp1 == "exit")
            break;
        if (exp1.calculate())
            std::cout << "=" << exp1 << std::endl;
        else
            std::cout << "非法的表达式" << std::endl;
        system("pause");
        system("cls");
        exp1.clear();
    }
    return 0;
}