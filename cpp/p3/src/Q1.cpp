#include <iostream>
#include <string>
using namespace std;

string calculate(int n, int num = 1, int stage = 2)
{
    if (n == 1)
        return "";
    if (stage == n)
    {
        int denomi = num * 2 + 1;
        return "+\\frac{" + to_string(num * 2) + "}{" + to_string(denomi) + "}";
    }
    int currentNum = num * 2;
    string left = calculate(n, currentNum, stage + 1);
    string right = calculate(n, currentNum + 1, stage + 1);
    return "+\\frac{" + to_string(currentNum) + left + "}{" + to_string(currentNum + 1) + right + "}";
}

int main()
{
    int depth;
    cin >> depth;
    string result = '1' + calculate(depth);
    cout << result << endl;
    return 1;
}
