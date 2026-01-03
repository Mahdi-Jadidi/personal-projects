#pragma once

#include "include/Question.hpp"
#include "include/Exam.hpp"
#include <vector>
#include <utility>

std::vector<std::shared_ptr<Query>> loadQueries(const std::string &path);
void makeBlueprint(std::unordered_map<std::string, std::shared_ptr<Blueprint>> &blueprints);
void buildExam(std::vector<std::shared_ptr<Query>> &queries,
              std::unordered_map<std::string, std::shared_ptr<Blueprint>> &blueprints,
              std::unordered_map<std::string, std::shared_ptr<Exam>> &exams);
void takeExam(std::unordered_map<std::string, std::shared_ptr<Exam>> &exams,
             std::vector<std::shared_ptr<ExamResult>> &outcomes);
void autoBuildExam(std::vector<std::shared_ptr<Query>> &queries,
                  std::unordered_map<std::string, std::shared_ptr<Exam>> &exams);
void showFullReport(const std::vector<std::shared_ptr<Query>> &queries);
void showExamReport(const std::vector<std::shared_ptr<ExamResult>> &outcomes);
void showAllExamReports(const std::vector<std::shared_ptr<ExamResult>> &outcomes);
void showTopicReport(const std::vector<std::shared_ptr<Query>> &queries);
bool handleAction(std::vector<std::shared_ptr<Query>> &queries,
                std::unordered_map<std::string, std::shared_ptr<Blueprint>> &blueprints,
                std::unordered_map<std::string, std::shared_ptr<Exam>> &exams,
                std::vector<std::shared_ptr<ExamResult>> &outcomes);
