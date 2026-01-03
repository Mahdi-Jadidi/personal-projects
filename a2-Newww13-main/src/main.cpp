#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <queue>
#include <algorithm>
#include <cmath>
using namespace std;

struct Table
{
    int id;
    int x, y;
    int capacity;
    string type;
    vector<string> people;
    queue<string> waitingQueue;
};

struct Student
{
    int id;
    string name;
    int friend_id;
    int enemy_id;
    int table_id = -1;
};

vector<Table> readTables(const string &filePath)
{
    vector<Table> tables;
    ifstream file(filePath);
    string line;
    getline(file, line); // Skip header
    while (getline(file, line))
    {
        stringstream ss(line);
        string token;
        Table table;
        getline(ss, token, ',');
        table.id = stoi(token);
        getline(ss, token, ',');
        table.x = stoi(token);
        getline(ss, token, ',');
        table.y = stoi(token);
        getline(ss, token, ',');
        table.capacity = stoi(token);
        getline(ss, token);
        table.type = token;
        tables.push_back(table);
    }
    return tables;
}

vector<Student> readStudents(const string &filePath)
{
    vector<Student> students;
    ifstream file(filePath);
    string line;
    getline(file, line); // Skip header
    while (getline(file, line))
    {
        stringstream ss(line);
        string token;
        Student student;
        getline(ss, token, ',');
        student.id = stoi(token);
        getline(ss, token, ',');
        student.name = token;
        getline(ss, token, ',');
        student.friend_id = stoi(token);
        getline(ss, token);
        student.enemy_id = stoi(token);
        students.push_back(student);
    }
    return students;
}

void showTableInfo(const vector<Table> &tables, int tableId)
{
    for (const auto &table : tables)
    {
        if (table.id == tableId)
        {
            vector<string> sortedPeople = table.people;
            sort(sortedPeople.begin(), sortedPeople.end());
            cout << "Table ID: " << table.id << endl;
            cout << "People at the table: ";
            for (const auto &person : sortedPeople)
            {
                cout << person << ", ";
            }
            cout << endl;
            cout << "Table remaining capacity: " << table.capacity - table.people.size() << endl;
            cout << "Waiting queue length: " << table.waitingQueue.size() << endl;
            return;
        }
    }
}

int manhattanDistance(int x1, int y1, int x2, int y2)
{
    return abs(x1 - x2) + abs(y1 - y2);
}

int calculateScore(const Table &table, const Student &student, const vector<Student> &students, const vector<Table> &tables)
{
    int score = 0;
    Student friendStudent, enemyStudent;
    for (const auto &s : students)
    {
        if (s.id == student.friend_id)
            friendStudent = s;
        if (s.id == student.enemy_id)
            enemyStudent = s;
    }
    int distanceToFriend = 0, distanceToEnemy = 0;
    if (friendStudent.table_id != -1)
    {
        for (const auto &t : tables)
        {
            if (t.id == friendStudent.table_id)
            {
                distanceToFriend = manhattanDistance(table.x, table.y, t.x, t.y);
                break;
            }
        }
    }
    if (enemyStudent.table_id != -1)
    {
        for (const auto &t : tables)
        {
            if (t.id == enemyStudent.table_id)
            {
                distanceToEnemy = manhattanDistance(table.x, table.y, t.x, t.y);
                break;
            }
        }
    }
    score = distanceToEnemy - distanceToFriend;
    if (table.type == "window")
        score += 6;
    else if (table.type == "door")
        score += 4;
    else if (table.type == "middle")
        score += 2;

    return score;
}

vector<Table> rankTables(const vector<Table> &tables, const Student &student, const vector<Student> &students)
{
    vector<Table> rankedTables = tables;
    sort(rankedTables.begin(), rankedTables.end(), [&](const Table &a, const Table &b)
         {
        int scoreA = calculateScore(a, student, students, tables);
        int scoreB = calculateScore(b, student, students, tables);
        if (scoreA == scoreB) return a.id < b.id;
        return scoreA > scoreB; });
    return rankedTables;
}

void enterStudent(vector<Student> &students, vector<Table> &tables, int studentId)
{
    for (auto &student : students)
    {
        if (student.id == studentId)
        {
            vector<Table> rankedTables = rankTables(tables, student, students);
            for (auto &table : rankedTables)
            {
                if (table.people.size() < table.capacity)
                {
                    tables[table.id - 1].people.push_back(student.name);
                    student.table_id = table.id;
                    cout << student.name << " sits at table " << table.id << endl;
                    return;
                }
            }
            if (!rankedTables.empty())
            {
                tables[rankedTables[0].id - 1].waitingQueue.push(student.name);
                cout << student.name << " enters the waiting queue of table " << rankedTables[0].id << endl;
            }
            return;
        }
    }
}

void reserveTable(vector<Table> &tables, int studentId, int tableId, const string &studentName)
{
    for (auto &table : tables)
    {
        if (table.id == tableId)
        {
            if (table.people.size() < table.capacity)
            {
                table.people.push_back(studentName);
                cout << studentName << " sits at table " << table.id << endl;
            }
            else
            {
                table.waitingQueue.push(studentName);
                cout << studentName << " enters the waiting queue of table " << table.id << endl;
            }
            return;
        }
    }
}

void exitStudent(vector<Student> &students, vector<Table> &tables, int studentId)
{
    Student *student = nullptr;
    for (auto &s : students)
    {
        if (s.id == studentId)
        {
            student = &s;
            break;
        }
    }
    if (student == nullptr)
    {
        cerr << "Student not found!" << endl;
        return;
    }
    Table *studentTable = nullptr;
    for (auto &table : tables)
    {
        if (table.id == student->table_id)
        {
            studentTable = &table;
            break;
        }
    }
    if (studentTable == nullptr)
    {
        cerr << "Table not found!" << endl;
        return;
    }
    auto it = find(studentTable->people.begin(), studentTable->people.end(), student->name);
    if (it != studentTable->people.end())
    {
        studentTable->people.erase(it);
        if (!studentTable->waitingQueue.empty())
        {
            string nextStudentName = studentTable->waitingQueue.front();
            studentTable->waitingQueue.pop();
            Student *nextStudent = nullptr;
            for (auto &s : students)
            {
                if (s.name == nextStudentName)
                {
                    nextStudent = &s;
                    break;
                }
            }
            if (nextStudent != nullptr)
            {
                nextStudent->table_id = studentTable->id;
                studentTable->people.push_back(nextStudentName);
                cout << nextStudentName << " sits at table " << studentTable->id << endl;
            }
        }
        student->table_id = -1;
        cout << student->name << " exits!" << endl;
    }
    else
    {
        cerr << "Student not found at the table!" << endl;
    }
}

void switchSeats(vector<Student> &students, vector<Table> &tables, int studentId)
{
    Student *student = nullptr;
    Student *friendStudent = nullptr;
    for (auto &s : students)
    {
        if (s.id == studentId)
        {
            student = &s;
        }
        if (s.id == student->friend_id)
        {
            friendStudent = &s;
        }
    }
    if (student == nullptr || friendStudent == nullptr)
    {
        cerr << "Student or friend not found!" << endl;
        return;
    }
    Table *studentTable = nullptr;
    Table *friendTable = nullptr;
    for (auto &table : tables)
    {
        if (table.id == student->table_id)
        {
            studentTable = &table;
        }
        if (table.id == friendStudent->table_id)
        {
            friendTable = &table;
        }
    }
    if (studentTable == nullptr || friendTable == nullptr)
    {
        cerr << "Table not found!" << endl;
        return;
    }
    auto itStudent = find(studentTable->people.begin(), studentTable->people.end(), student->name);
    auto itFriend = find(friendTable->people.begin(), friendTable->people.end(), friendStudent->name);
    if (itStudent != studentTable->people.end() && itFriend != friendTable->people.end())
    {
        swap(*itStudent, *itFriend);
        swap(student->table_id, friendStudent->table_id);
        cout << student->name << " switches seats with " << friendStudent->name << "!" << endl;
    }
    else
    {
        cerr << "Students not found at their tables!" << endl;
    }
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cerr << "Usage: " << argv[0] << " <tables_file> <students_file>" << endl;
        return 1;
    }
    string tablesFilePath = argv[1];
    string studentsFilePath = argv[2];
    vector<Table> tables = readTables(tablesFilePath);
    vector<Student> students = readStudents(studentsFilePath);
    string command;
    while (true)
    {
        cout << "Enter command: ";
        getline(cin, command);
        if (command.empty())
            continue;
        stringstream ss(command);
        string action;
        ss >> action;
        if (action == "show_table_info")
        {
            int tableId;
            ss >> tableId;
            showTableInfo(tables, tableId);
        }
        else if (action == "enter")
        {
            int studentId;
            ss >> studentId;
            enterStudent(students, tables, studentId);
        }
        else if (action == "reserve_table")
        {
            int studentId, tableId;
            ss >> studentId >> tableId;
            reserveTable(tables, studentId, tableId, students[studentId - 1].name);
        }
        else if (action == "exit")
        {
            int studentId;
            ss >> studentId;
            exitStudent(students, tables, studentId);
        }
        else if (action == "switch")
        {
            int studentId;
            ss >> studentId;
            switchSeats(students, tables, studentId);
        }
        else if (action == "quit")
        {
            break;
        }
        else
        {
            cerr << "Invalid command!" << endl;
        }
    }
    return 0;
}
