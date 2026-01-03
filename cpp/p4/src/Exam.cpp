#include "include/Exam.hpp"
#include <iostream>
#include <algorithm>
#include <unordered_map>

Exam::Exam(const std::string &title, std::shared_ptr<Blueprint> pattern) : 
    title(title), pattern(pattern) {}

void Exam::addQuery(std::shared_ptr<Query> q) { 
    queries.push_back(q); 
}

void Exam::administer(std::vector<std::shared_ptr<ExamResult>> &outcomes) {
    std::vector<int> picks(queries.size(), Numbers::DEFAULT_RESPONSE);
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
    
    std::cout << "Finished " << title << ".\n";
    logPicks(picks);
    outcomes.push_back(makeResult(picks));
}

// Implement other Exam methods...
