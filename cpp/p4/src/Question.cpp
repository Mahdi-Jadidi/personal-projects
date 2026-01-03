#include "include/Question.hpp"
#include "include/Utils.hpp"
#include <iostream>
#include <stdexcept>
#include <algorithm>

using namespace std;

// پیاده‌سازی متدهای کلاس Query
Query::Query(const string &prompt, const string &opt1, 
             const string &opt2, const string &opt3, 
             const string &opt4, int solution, int complexity, 
             const string &topic) : 
    prompt(prompt), solution(solution), complexity(complexity), topic(topic) {
    
    selections[0] = opt1;
    selections[1] = opt2;
    selections[2] = opt3;
    selections[3] = opt4;
    
    if (solution < 1 || solution > Numbers::CHOICES || 
        prompt.empty() || opt1.empty() || opt2.empty() || 
        opt3.empty() || opt4.empty()) {
        throw invalid_argument("Invalid query parameters");
    }
}

void Query::display(int num, int lastPick) const {
    cout << "\n" << num << ") " << prompt << "\n";
    for (int i = 0; i < Numbers::CHOICES; i++) {
        cout << "    " << i + 1 << ". " << selections[i];
        if (lastPick == i + 1) cout << " <-";
        cout << "\n";
    }
}

void Query::logResponse(int pick) {
    if (pick == Defs::Responses::NONE) {
        skipCount++;
    } else if (pick == solution) {
        rightCount++;
    } else {
        wrongCount++;
    }
}

int Query::computeScore() const {
    return (Numbers::WRONG_PTS * wrongCount) + 
           (Numbers::SKIP_PTS * skipCount) - 
           (Numbers::RIGHT_PTS * rightCount);
}

string Query::getPrompt() const {
    return prompt;
}

string Query::getTopic() const {
    return topic;
}

int Query::getComplexity() const {
    return complexity;
}

bool Query::isRight(int pick) const {
    return pick == solution;
}

int Query::getTotal() const {
    return rightCount + wrongCount + skipCount;
}

int Query::getRight() const {
    return rightCount;
}

int Query::getWrong() const {
    return wrongCount;
}

int Query::getSkips() const {
    return skipCount;
}

// پیاده‌سازی متدهای کلاس BlueprintSection
BlueprintSection::BlueprintSection(const string &sectionStr) {
    auto parts = chopString(sectionStr, Defs::SPLITTER);
    if (parts.size() != 3) {
        throw invalid_argument("Invalid blueprint section format");
    }
    
    topic = parts[BlueprintParts::TOPIC];
    amount = stoi(parts[BlueprintParts::AMOUNT]);
    
    if (parts[BlueprintParts::COMPLEXITY] == "hard") {
        complexity = Defs::Levels::TOUGH;
    } else if (parts[BlueprintParts::COMPLEXITY] == "medium") {
        complexity = Defs::Levels::MID;
    } else if (parts[BlueprintParts::COMPLEXITY] == "easy") {
        complexity = Defs::Levels::SIMPLE;
    } else {
        throw invalid_argument("Invalid difficulty level");
    }
}

string BlueprintSection::getTopic() const {
    return topic;
}

int BlueprintSection::getComplexity() const {
    return complexity;
}

int BlueprintSection::getAmount() const {
    return amount;
}
