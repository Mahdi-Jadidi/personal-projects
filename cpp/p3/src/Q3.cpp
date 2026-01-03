#include <iostream>
#include <vector>
#include <climits>
#include <algorithm>
using namespace std;

struct Menu {
    string name;
    int cost;
};

void findClosestSumRecursive(const vector<Menu> &nums, int currentSum, int target, int &closestSum, vector<bool> &used, int index, int count, vector<int> &currentSubset, vector<int> &closestSubset, int minimum) {
    if (count > 0 && currentSum >= minimum && currentSum <= target) {
        if (abs(currentSum - target) < abs(closestSum - target)) {
            closestSum = currentSum;
            closestSubset = currentSubset;
        }
    }
    for (int i = index; i < nums.size(); ++i) {
        if (!used[i]) {
            used[i] = true;
            currentSubset.push_back(i);
            findClosestSumRecursive(nums, currentSum + nums[i].cost, target, closestSum, used, i + 1, count + 1, currentSubset, closestSubset, minimum);
            currentSubset.pop_back();
            used[i] = false;
        }
    }
}

void findClosestSumAllK(const vector<Menu> &nums, int target, int &closestSum, vector<int> &closestSubset, int minimum) {
    vector<bool> used(nums.size(), false);
    vector<int> currentSubset;
    findClosestSumRecursive(nums, 0, target, closestSum, used, 0, 0, currentSubset, closestSubset, minimum);
}

int main() {
    int n, x, l, r;
    cin >> n >> x >> l >> r;
    vector<Menu> menu(n);
    for (int i = 0; i < n; i++) {
        cin >> menu[i].name >> menu[i].cost;
    }
    vector<int> closestSubset;
    int target = (r * 100 / x) * 100 / 110;
    int closestSum = INT_MAX;
    vector<bool> used(menu.size(), false);
    findClosestSumAllK(menu, target, closestSum, closestSubset, l);
    for (int index : closestSubset) {
        cout << menu[index].name << endl;
    }
    return 0;
}
