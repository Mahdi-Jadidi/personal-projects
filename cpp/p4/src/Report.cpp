#include "include/Report.hpp"
#include "include/Utils.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <unordered_map>

using namespace std;

vector<shared_ptr<Query>> loadQueries(const string &path) {
    vector<shared_ptr<Query>> queries;
    ifstream file(path);
    string line;
    
    getline(file, line); // Skip header
    while (getline(file, line)) {
        auto fields = chopString(line, Defs::DIVIDER);
        queries.push_back(make_shared<Query>(
            fields[0], fields[1], fields[2], fields[3], fields[4],
            stoi(fields[5]),
            (fields[6] == "easy") ? Defs::Levels::SIMPLE : 
            (fields[6] == "medium") ? Defs::Levels::MID : Defs::Levels::TOUGH,
            fields[7]
        ));
    }
    
    return queries;
}

void makeBlueprint(unordered_map<string, shared_ptr<Blueprint>> &blueprints) {
    string line;
    getline(cin, line);
    auto newBlueprint = make_shared<Blueprint>(line);
    
    if (blueprints.count(newBlueprint->getTitle())) {
        cout << "Duplicate name: '" << newBlueprint->getTitle() << "'\n";
        return;
    }
    
    blueprints[newBlueprint->getTitle()] = newBlueprint;
    cout << "Blueprint '" << newBlueprint->getTitle() << "' was created successfully.\n";
}

void buildExam(vector<shared_ptr<Query>> &queries,
              unordered_map<string, shared_ptr<Blueprint>> &blueprints,
              unordered_map<string, shared_ptr<Exam>> &exams) {
    string line;
    getline(cin, line);
    vector<string> parts = chopString(line, Defs::MARK);
    
    auto it = blueprints.find(parts[1]);
    if (it == blueprints.end()) {
        cout << "Could not find blueprint: '" << parts[1] << "'\n";
        return;
    }
    
    auto newExam = make_shared<Exam>(parts[0], it->second);
    
    // Sort queries by priority and text
    sort(queries.begin(), queries.end(), [](const auto &a, const auto &b) {
        if (a->computeScore() == b->computeScore()) {
            return a->getPrompt() < b->getPrompt();
        }
        return a->computeScore() > b->computeScore();
    });
    
    // Add questions based on template
    auto pattern = newExam->getPattern();
    int i = 0;
    while (true) {
        string topic = pattern->getSectionTopic(i);
        if (topic.empty()) break;
        
        int complexity = pattern->getSectionComplexity(i);
        int amount = pattern->getSectionAmount(i);
        int picked = 0;
        
        for (const auto &q : queries) {
            if (q->getTopic() == topic && q->getComplexity() == complexity) {
                newExam->addQuery(q);
                picked++;
                if (picked >= amount) break;
            }
        }
        i++;
    }
    
    exams[newExam->getTitle()] = newExam;
    cout << "Exam '" << newExam->getTitle() << "' was generated successfully.\n";
}

void takeExam(unordered_map<string, shared_ptr<Exam>> &exams,
             vector<shared_ptr<ExamResult>> &outcomes) {
    string examTitle;
    getline(cin, examTitle);
    examTitle = stripMarks(examTitle);
    
    auto it = exams.find(examTitle);
    if (it == exams.end()) {
        cout << "Could not find exam: '" << examTitle << "'\n";
        return;
    }
    
    cout << examTitle << ":\n";
    it->second->administer(outcomes);
}

pair<string, string> findWeakTopics(const vector<shared_ptr<Query>> &queries) {
    unordered_map<string, pair<int, int>> topicStats; // <correct, total>
    
    for (const auto &q : queries) {
        string topic = q->getTopic();
        topicStats[topic].first += q->getRight();
        topicStats[topic].second += q->getTotal();
    }
    
    vector<pair<string, double>> topicPercents;
    for (const auto &stat : topicStats) {
        double percent = (stat.second.second == 0) ? 0 : 
                        (stat.second.first * 100.0) / stat.second.second;
        topicPercents.emplace_back(stat.first, percent);
    }
    
    sort(topicPercents.begin(), topicPercents.end(), 
        [](const auto &a, const auto &b) {
            if (a.second == b.second) return a.first < b.first;
            return a.second < b.second;
        });
    
    return {topicPercents[0].first, topicPercents[1].first};
}

void autoBuildExam(vector<shared_ptr<Query>> &queries,
                  unordered_map<string, shared_ptr<Exam>> &exams) {
    string examTitle;
    getline(cin, examTitle);
    examTitle = stripMarks(examTitle);
    
    auto weakest = findWeakTopics(queries);
    string virtualBlueprint = "'" + examTitle + "' " + 
        weakest.first + ":easy:3 " + weakest.first + ":medium:2 " + weakest.first + ":hard:1 " +
        weakest.second + ":easy:2 " + weakest.second + ":medium:1 " + weakest.second + ":hard:1";
    
    auto newBlueprint = make_shared<Blueprint>(virtualBlueprint);
    auto newExam = make_shared<Exam>(examTitle, newBlueprint);
    
    // Add questions based on auto-generated template
    sort(queries.begin(), queries.end(), [](const auto &a, const auto &b) {
        if (a->computeScore() == b->computeScore()) {
            return a->getPrompt() < b->getPrompt();
        }
        return a->computeScore() > b->computeScore();
    });
    
    auto pattern = newExam->getPattern();
    int i = 0;
    while (true) {
        string topic = pattern->getSectionTopic(i);
        if (topic.empty()) break;
        
        int complexity = pattern->getSectionComplexity(i);
        int amount = pattern->getSectionAmount(i);
        int picked = 0;
        
        for (const auto &q : queries) {
            if (q->getTopic() == topic && q->getComplexity() == complexity) {
                newExam->addQuery(q);
                picked++;
                if (picked >= amount) break;
            }
        }
        i++;
    }
    
    exams[examTitle] = newExam;
    cout << "Exam '" << examTitle << "' was generated successfully.\n";
}

vector<pair<string, vector<int>>> makeTopicReport(const vector<shared_ptr<Query>> &queries) {
    unordered_map<string, vector<int>> report;
    
    for (const auto &q : queries) {
        string topic = q->getTopic();
        if (report[topic].empty()) {
            report[topic] = vector<int>(Numbers::DATA_COLS, 0);
        }
        
        report[topic][Defs::Stats::ALL] += q->getTotal();
        report[topic][Defs::Stats::RIGHT] += q->getRight();
        report[topic][Defs::Stats::WRONG] += q->getWrong();
        report[topic][Defs::Stats::SKIPPED] += q->getSkips();
    }
    
    vector<pair<string, vector<int>>> sortedReport(report.begin(), report.end());
    sort(sortedReport.begin(), sortedReport.end(),
        [](const auto &a, const auto &b) { return a.first < b.first; });
    
    return sortedReport;
}

vector<int> calcTotalReport(const vector<pair<string, vector<int>>> &topicReport) {
    vector<int> total(Numbers::DATA_COLS, 0);
    
    for (const auto &topic : topicReport) {
        for (int j = 0; j < Numbers::DATA_COLS; j++) {
            total[j] += topic.second[j];
        }
    }
    
    return total;
}

void showFullReport(const vector<shared_ptr<Query>> &queries) {
    auto report = makeTopicReport(queries);
    auto total = calcTotalReport(report);
    
    cout << "Total report:\n\n";
    for (const auto &topic : report) {
        cout << topic.first << ": " << topic.second[Defs::Stats::RIGHT] 
             << " corrects, " << topic.second[Defs::Stats::WRONG] 
             << " incorrects and " << topic.second[Defs::Stats::SKIPPED] 
             << " blanks. Score: ";
        
        if (topic.second[Defs::Stats::ALL] == 0) {
            cout << "0.000%.\n";
        } else {
            cout << fixed << setprecision(3) 
                 << (topic.second[Defs::Stats::RIGHT] * 100.0) / topic.second[Defs::Stats::ALL] 
                 << "%.\n";
        }
    }
    
    cout << "\nTotal results: " << total[Defs::Stats::RIGHT] 
         << " corrects, " << total[Defs::Stats::WRONG] 
         << " incorrects and " << total[Defs::Stats::SKIPPED] << " blanks.\n";
    
    cout << "Total score: " << fixed << setprecision(3) 
         << (total[Defs::Stats::RIGHT] * 100.0) / total[Defs::Stats::ALL] << "%.\n";
}

vector<int> calcExamTotal(const vector<shared_ptr<ExamResult>> &outcomes, int idx) {
    vector<int> total(Numbers::DATA_COLS, 0);
    
    for (const auto &topic : outcomes[idx]->getData()) {
        for (int j = 0; j < Numbers::DATA_COLS; j++) {
            total[j] += topic.second[j];
        }
    }
    
    return total;
}

vector<vector<int>> makeComplexityReport(const vector<shared_ptr<Query>> &queries, 
                                       const string &targetTopic) {
    vector<vector<int>> report(Numbers::STAT_TYPES, vector<int>(Numbers::LEVELS, 0));
    
    for (const auto &q : queries) {
        if (q->getTopic() == targetTopic) {
            int complex = q->getComplexity();
            report[Defs::Stats::ALL][complex - 1] += q->getTotal();
            report[Defs::Stats::RIGHT][complex - 1] += q->getRight();
            report[Defs::Stats::WRONG][complex - 1] += q->getWrong();
            report[Defs::Stats::SKIPPED][complex - 1] += q->getSkips();
        }
    }
    
    return report;
}

void showExamReport(const vector<shared_ptr<ExamResult>> &outcomes) {
    string examTitle;
    getline(cin, examTitle);
    examTitle = stripMarks(examTitle);
    
    // Find the exam result
    auto it = find_if(outcomes.begin(), outcomes.end(),
        [&examTitle](const auto &result) { return result->getTitle() == examTitle; });
    
    if (it == outcomes.end()) {
        cout << "No results found for exam: '" << examTitle << "'\n";
        return;
    }
    
    auto examResult = *it;
    auto total = calcExamTotal(outcomes, distance(outcomes.begin(), it));
    
    cout << "Results for " << examTitle << ":\n\n";
    for (const auto &topic : examResult->getData()) {
        cout << topic.first << ": " << topic.second[Defs::Stats::RIGHT] 
             << " corrects, " << topic.second[Defs::Stats::WRONG] 
             << " incorrects and " << topic.second[Defs::Stats::SKIPPED] 
             << " blanks. Score: ";
        
        if (topic.second[Defs::Stats::ALL] == 0) {
            cout << "0.000%.\n";
        } else {
            cout << fixed << setprecision(3) 
                 << (topic.second[Defs::Stats::RIGHT] * 100.0) / topic.second[Defs::Stats::ALL] 
                 << "%.\n";
        }
    }
    
    cout << "\nTotal results: " << total[Defs::Stats::RIGHT] 
         << " corrects, " << total[Defs::Stats::WRONG] 
         << " incorrects and " << total[Defs::Stats::SKIPPED] << " blanks.\n";
    
    cout << "Total score: " << fixed << setprecision(3) 
         << (total[Defs::Stats::RIGHT] * 100.0) / total[Defs::Stats::ALL] << "%.\n";
}

void showAllExamReports(const vector<shared_ptr<ExamResult>> &outcomes) {
    cout << "Results per attended tests:\n\n";
    
    for (const auto &result : outcomes) {
        auto total = calcExamTotal(outcomes, &result - &outcomes[0]);
        
        cout << result->getTitle() << ": " << total[Defs::Stats::RIGHT] 
             << " corrects, " << total[Defs::Stats::WRONG] 
             << " incorrects and " << total[Defs::Stats::SKIPPED] 
             << " blanks. Score: ";
        
        if (total[Defs::Stats::ALL] == 0) {
            cout << "0.000%.\n";
        } else {
            cout << fixed << setprecision(3) 
                 << (total[Defs::Stats::RIGHT] * 100.0) / total[Defs::Stats::ALL] 
                 << "%.\n";
        }
    }
}

void showTopicReport(const vector<shared_ptr<Query>> &queries) {
    string topic;
    getline(cin, topic);
    topic = topic.substr(1); // Remove leading space
    
    cout << "Results for " << topic << ":\n\n";
    auto report = makeComplexityReport(queries, topic);
    
    cout << "Easy: " << report[Defs::Stats::RIGHT][Numbers::SIMPLE_IDX] 
         << " corrects, " << report[Defs::Stats::WRONG][Numbers::SIMPLE_IDX] 
         << " incorrects and " << report[Defs::Stats::SKIPPED][Numbers::SIMPLE_IDX] << " blanks.\n";
    
    cout << "Medium: " << report[Defs::Stats::RIGHT][Numbers::MID_IDX] 
         << " corrects, " << report[Defs::Stats::WRONG][Numbers::MID_IDX] 
         << " incorrects and " << report[Defs::Stats::SKIPPED][Numbers::MID_IDX] << " blanks.\n";
    
    cout << "Hard: " << report[Defs::Stats::RIGHT][Numbers::TOUGH_IDX] 
         << " corrects, " << report[Defs::Stats::WRONG][Numbers::TOUGH_IDX] 
         << " incorrects and " << report[Defs::Stats::SKIPPED][Numbers::TOUGH_IDX] << " blanks.\n";
    
    int totalRight = report[Defs::Stats::RIGHT][Numbers::SIMPLE_IDX] + 
                    report[Defs::Stats::RIGHT][Numbers::MID_IDX] + 
                    report[Defs::Stats::RIGHT][Numbers::TOUGH_IDX];
    
    int totalAll = report[Defs::Stats::ALL][Numbers::SIMPLE_IDX] + 
                  report[Defs::Stats::ALL][Numbers::MID_IDX] + 
                  report[Defs::Stats::ALL][Numbers::TOUGH_IDX];
    
    cout << "\nTotal score: " << fixed << setprecision(3) 
         << (totalRight * 100.0) / totalAll << "%.\n";
}

bool handleAction(vector<shared_ptr<Query>> &queries,
                unordered_map<string, shared_ptr<Blueprint>> &blueprints,
                unordered_map<string, shared_ptr<Exam>> &exams,
                vector<shared_ptr<ExamResult>> &outcomes) {
    string action;
    if (!(cin >> action)) return false;
    
    if (action == Actions::MAKE_BLUEPRINT) {
        makeBlueprint(blueprints);
    } 
    else if (action == Actions::BUILD_EXAM) {
        buildExam(queries, blueprints, exams);
    } 
    else if (action == Actions::TAKE) {
        takeExam(exams, outcomes);
    } 
    else if (action == Actions::AUTO_BUILD) {
        autoBuildExam(queries, exams);
    } 
    else if (action == Actions::SUMMARY) {
        string summaryType;
        cin >> summaryType;
        
        if (summaryType == Actions::FULL_SUMMARY) {
            showFullReport(queries);
        } 
        else if (summaryType == Actions::EXAM_SUMMARY) {
            showExamReport(outcomes);
        } 
        else if (summaryType == Actions::ALL_EXAMS_SUMMARY) {
            showAllExamReports(outcomes);
        } 
        else if (summaryType == Actions::TOPIC_SUMMARY) {
            showTopicReport(queries);
        }
    }
    
    return true;
}
