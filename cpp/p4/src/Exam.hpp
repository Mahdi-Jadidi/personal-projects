
#pragma once

#include "include/Template.hpp"
#include <vector>
#include <memory>
#include <unordered_map>

class ExamResult {
private:
    std::vector<std::pair<std::string, std::vector<int>>> data;
    std::string examTitle;

public:
    ExamResult(const std::string &title, 
              const std::vector<std::pair<std::string, std::vector<int>>> &data);
    std::string getTitle() const;
    std::vector<std::pair<std::string, std::vector<int>>> getData() const;
};

class Exam {
private:
    std::string title;
    std::shared_ptr<Blueprint> pattern;
    std::vector<std::shared_ptr<Query>> queries;

    int getValidPick() const;
    void logPicks(const std::vector<int> &picks);
    std::shared_ptr<ExamResult> makeResult(const std::vector<int> &picks) const;

public:
    Exam(const std::string &title, std::shared_ptr<Blueprint> pattern);
    void addQuery(std::shared_ptr<Query> q);
    void administer(std::vector<std::shared_ptr<ExamResult>> &outcomes);
    std::string getTitle() const;
    std::shared_ptr<Blueprint> getPattern() const;
};
