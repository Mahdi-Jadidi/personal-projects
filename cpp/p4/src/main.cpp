#include "include/Report.hpp"
#include <iostream>
#include <memory>
#include <unordered_map>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Please provide question file path\n";
        return 1;
    }
    
    auto queries = loadQueries(argv[1]);
    std::unordered_map<std::string, std::shared_ptr<Blueprint>> blueprints;
    std::unordered_map<std::string, std::shared_ptr<Exam>> exams;
    std::vector<std::shared_ptr<ExamResult>> outcomes;
    
    while (handleAction(queries, blueprints, exams, outcomes));
    
    return 0;
}
