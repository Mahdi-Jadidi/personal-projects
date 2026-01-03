#include <iostream>
#include <vector>
using namespace std;

struct person{
    int age;
    int donate;
    int owned = 0;
    int present = 1;
};

int findOld(vector<person> &people, int a, int b){
    if (a > b)
        return -1;
    int age = -1, old = -1;
    for (int i = a; i <= b; i++){
        if (people[i].age > age && people[i].present == 1){
            old = i;
            age = people[i].age;
        }
    }
    return old;
}

void donateMoney(vector<person> &people, int old){
    if (old == -1)
        return;
    int left_old = findOld(people, 0, old - 1);
    int right_old = findOld(people, old + 1, people.size() - 1);
    bool can_donate_left = (left_old != -1);
    bool can_donate_right = (right_old != -1);
    if (can_donate_left){
    for (int i = old - 1; i >= left_old; i--){
            if (people[i].present == 0){
                can_donate_left = false;
                break;
            }
        }
    }
    if (can_donate_right){
        for (int i = old + 1; i <= right_old; i++){
            if (people[i].present == 0){
                can_donate_right = false;
                break;
            }
        }
    }
    if (can_donate_left && can_donate_right){
        people[left_old].owned += people[old].donate / 2;
        people[right_old].owned += people[old].donate / 2;
    }
    else if (can_donate_left){
        people[left_old].owned += people[old].donate;
    }
    else if (can_donate_right){
        people[right_old].owned += people[old].donate;
    }
    else{
        people[old].owned += people[old].donate;
    }
    people[old].present = 0;
    int next_old = findOld(people, 0, people.size() - 1);
    donateMoney(people, next_old);
}

int main()
{
    int n;
    cin >> n;
    vector<person> people(n);
    for (int i = 0; i < people.size(); i++){
        cin >> people[i].age;
    }
    for (int i = 0; i < people.size(); i++){
        cin >> people[i].donate;
    }
    int old = findOld(people, 0, people.size() - 1);
    donateMoney(people, old);
    for (int i = 0; i < people.size(); i++){
        cout << people[i].owned << ' ';
    }
    return 0;
}
