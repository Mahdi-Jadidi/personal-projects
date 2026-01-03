#pragma once

#include "include/Defs.hpp"
#include <string>
#include <vector>
#include <memory>

class Query {
private:
    std::string selections[Numbers::CHOICES];
    std::string prompt, topic;
    int solution, complexity, rightCount = 0, wrongCount = 0, skipCount = 0;

public:
    Query(const std::string &prompt, const std::string &opt1, const std::string &opt2, 
          const std::string &opt3, const std::string &opt4, int solution, 
          int complexity, const std::string &topic);
    
    void display(int num, int lastPick) const;
    void logResponse(int pick);
    int computeScore() const;
    std::string getPrompt() const;
    std::string getTopic() const;
    int getComplexity() const;
    bool isRight(int pick) const;
    int getTotal() const;
    int getRight() const;
    int getWrong() const;
    int getSkips() const;
};

class BlueprintSection {
private:
    std::string topic;
    int complexity, amount;

public:
    BlueprintSection(const std::string &sectionStr);
    std::string getTopic() const;
    int getComplexity() const;
    int getAmount() const;
};
