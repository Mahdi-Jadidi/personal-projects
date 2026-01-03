#include "query.hpp"
#include <iostream>
#include <stdexcept>

Query::Query(const std::string &prompt, const std::string &opt1, const std::string &opt2, 
             const std::string &opt3, const std::string &opt4, int solution, 
             int complexity, const std::string &topic) 
    : prompt(prompt), solution(solution), complexity(complexity), topic(topic) {
    selections[0] = opt1;
    selections[1] = opt2;
    selections[2] = opt3;
    selections[3] = opt4;
    if (solution < 1 || solution > Numbers::CHOICES || prompt.empty() || 
        opt1.empty() || opt2.empty() || opt3.empty() || opt4.empty()) {
        throw std::invalid_argument("Invalid query parameters");
    }
}

void Query::display(int num, int lastPick) const {
    std::cout << "\n" << num << ") " << prompt << "\n";
    for (int i = 0; i < Numbers::CHOICES; i++) {
        std::cout << "    " << i + 1 << ". " << selections[i];
        if (lastPick == i + 1)
            std::cout << " <-";
        std::cout << "\n";
    }
}

void Query::logResponse(int pick) {
    if (pick == Defs::Responses::NONE)
        skipCount++;
    else if (pick == solution)
        rightCount++;
    else
        wrongCount++;
}

int Query::computeScore() const {
    return (Numbers::WRONG_PTS * wrongCount) + (Numbers::SKIP_PTS * skipCount) - 
           (Numbers::RIGHT_PTS * rightCount);
}

std::string Query::getPrompt() const { return prompt; }
std::string Query::getTopic() const { return topic; }
int Query::getComplexity() const { return complexity; }
bool Query::isRight(int pick) const { return pick == solution; }
int Query::getTotal() const { return rightCount + wrongCount + skipCount; }
int Query::getRight() const { return rightCount; }
int Query::getWrong() const { return wrongCount; }
int Query::getSkips() const { return skipCount; }
