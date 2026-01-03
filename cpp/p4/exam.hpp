#ifndef EXAM_HPP
#define EXAM_HPP

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include "blueprint.hpp"
#include "query.hpp"

class ExamResult {
private:
    std::string examTitle;
    std::vector<std::pair<std::string, std::vector<int>>> data;

public:
    ExamResult(const std::string &title, const std::vector<std::pair<std::string, std::vector<int>>> &data);
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

#endif // EXAM_HPP
