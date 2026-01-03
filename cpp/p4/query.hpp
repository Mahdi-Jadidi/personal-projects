#ifndef QUERY_HPP
#define QUERY_HPP

#include <string>
#include <vector>
#include "defs.hpp"

class Query {
private:
    std::string prompt;
    std::string selections[Numbers::CHOICES];
    int solution;
    int complexity;
    std::string topic;
    int rightCount = 0;
    int wrongCount = 0;
    int skipCount = 0;

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

#endif // QUERY_HPP
