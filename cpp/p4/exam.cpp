#include "include/Exam.hpp"
#include "include/Utils.hpp"
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <iomanip>

using namespace std;

Exam::Exam(const string &title, shared_ptr<Blueprint> pattern) : 
    title(title), pattern(pattern) {}

void Exam::addQuery(shared_ptr<Query> q) { 
    queries.push_back(q); 
}

void Exam::administer(vector<shared_ptr<ExamResult>> &outcomes) {
    vector<int> picks(queries.size(), Numbers::DEFAULT_RESPONSE);
    int current = 0;
    
    while (current < queries.size()) {
        queries[current]->display(current + 1, picks[current]);
        int pick = getValidPick();
        
        if (pick == Defs::Responses::BACK) {
            if (current > 0) current--;
            continue;
        }
        
        picks[current] = pick;
        current++;
    }
    
    cout << "Finished " << title << ".\n";
    logPicks(picks);
    outcomes.push_back(makeResult(picks));
}

int Exam::getValidPick() const {
    string input;
    while (true) {
        cout << "Your answer: ";
        getline(cin, input);
        
        if (input.empty()) {
            return Defs::Responses::NONE;
        }
        if (input == Actions::GO_BACK) {
            return Defs::Responses::BACK;
        }
        if (input == "1" || input == "2" || input == "3" || input == "4") {
            return stoi(input);
        }
        
        cout << "Invalid answer, please try again.\n";
    }
}

void Exam::logPicks(const vector<int> &picks) {
    for (size_t i = 0; i < picks.size(); i++) {
        queries[i]->logResponse(picks[i]);
    }
}

shared_ptr<ExamResult> Exam::makeResult(const vector<int> &picks) const {
    unordered_map<string, vector<int>> resultData;
    
    for (size_t i = 0; i < picks.size(); i++) {
        const auto &q = queries[i];
        string topic = q->getTopic();
        
        if (resultData[topic].empty()) {
            resultData[topic] = vector<int>(Numbers::DATA_COLS, 0);
        }
        
        resultData[topic][Defs::Stats::ALL]++;
        if (picks[i] == Defs::Responses::NONE) {
            resultData[topic][Defs::Stats::SKIPPED]++;
        }
        else if (q->isRight(picks[i])) {
            resultData[topic][Defs::Stats::RIGHT]++;
        }
        else {
            resultData[topic][Defs::Stats::WRONG]++;
        }
    }
    
    vector<pair<string, vector<int>>> sortedData(resultData.begin(), resultData.end());
    sort(sortedData.begin(), sortedData.end(), 
        [](const auto &a, const auto &b) { return a.first < b.first; });
    
    return make_shared<ExamResult>(title, sortedData);
}

string Exam::getTitle() const { 
    return title; 
}

shared_ptr<Blueprint> Exam::getPattern() const { 
    return pattern; 
}

ExamResult::ExamResult(const string &title, 
                     const vector<pair<string, vector<int>>> &data) : 
    examTitle(title), data(data) {}

string ExamResult::getTitle() const { 
    return examTitle; 
}

vector<pair<string, vector<int>>> ExamResult::getData() const { 
    return data; 
}
