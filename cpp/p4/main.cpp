#include <iostream>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <memory>
#include <iomanip>
#include <stdexcept>
using namespace std;

namespace Defs
{
    const string VOID = "";
    const int MISSING = -1;
    const char DIVIDER = ',';
    const char SPLITTER = ':';
    const char MARK = '\'';
    namespace Levels
    {
        const int SIMPLE = 1;
        const int MID = 2;
        const int TOUGH = 3;
    }

    namespace Stats
    {
        const int ALL = 0;
        const int RIGHT = 1;
        const int WRONG = 2;
        const int SKIPPED = 3;
    }

    namespace Responses
    {
        const int BACK = -2;
        const int BAD = -1;
        const int NONE = 0;
    }
}

namespace QueryParts
{
    const int PROMPT = 0;
    const int CHOICE1 = 1;
    const int CHOICE2 = 2;
    const int CHOICE3 = 3;
    const int CHOICE4 = 4;
    const int ANSWER = 5;
    const int COMPLEXITY = 6;
    const int TOPIC = 7;
}
namespace Numbers
{
    const int CHOICES = 4;
    const int START_SIZE = 10;
    const int DEFAULT_RESPONSE = Defs::Responses::NONE;
    const string PICK1 = "1";
    const string PICK2 = "2";
    const string PICK3 = "3";
    const string PICK4 = "4";
    const int DATA_COLS = 4;
    const int WRONG_PTS = 3;
    const int SKIP_PTS = 1;
    const int RIGHT_PTS = 2;
    const int STAT_TYPES = 4;
    const int LEVELS = 3;
    const int SIMPLE_IDX = Defs::Levels::SIMPLE - 1;
    const int MID_IDX = Defs::Levels::MID - 1;
    const int TOUGH_IDX = Defs::Levels::TOUGH - 1;
}

namespace BlueprintParts
{
    const int TOPIC = 0;
    const int COMPLEXITY = 1;
    const int AMOUNT = 2;
}

namespace Actions
{
    const string MAKE_BLUEPRINT = "create_template";
    const string BUILD_EXAM = "generate_test";
    const string TAKE = "attend";
    const string GO_BACK = "previous";
    const string AUTO_BUILD = "auto_generate";
    const string SUMMARY = "report";
    const string FULL_SUMMARY = "all";
    const string EXAM_SUMMARY = "test";
    const string ALL_EXAMS_SUMMARY = "tests";
    const string TOPIC_SUMMARY = "subject";
}

namespace ExamParts
{
    const int TITLE = 1;
    const int BLUEPRINT = 3;
}

vector<string> chopString(const string &input, char separator);
class Query
{
private:
    string selections[Numbers::CHOICES];
    string prompt, topic;
    int solution, complexity, rightCount = 0, wrongCount = 0, skipCount = 0;

public:
    void display(int num, int lastPick) const
    {
        cout << "\n"
             << num << ") " << prompt << "\n";
        int i = 0;
        while (i < Numbers::CHOICES)
        {
            cout << "    " << i + 1 << ". " << selections[i];
            if (lastPick == i + 1)
                cout << " <-";
            cout << "\n";
            i++;
        }
    }
    Query(const string &prompt, const string &opt1, const string &opt2, const string &opt3, const string &opt4, int solution, int complexity, const string &topic) : prompt(prompt), solution(solution), complexity(complexity), topic(topic)
    {
        selections[0] = opt1;
        selections[1] = opt2;
        selections[2] = opt3;
        selections[3] = opt4;
        if (solution < 1 || solution > Numbers::CHOICES || prompt.empty() || opt1.empty() || opt2.empty() || opt3.empty() || opt4.empty())
        {
            throw invalid_argument("Invalid query parameters");
        }
    }
    void logResponse(int pick)
    {
        if (pick == Defs::Responses::NONE)
            skipCount++;
        else if (pick == solution)
            rightCount++;
        else
            wrongCount++;
    }

    int computeScore() const
    {
        return (Numbers::WRONG_PTS * wrongCount) + (Numbers::SKIP_PTS * skipCount) - (Numbers::RIGHT_PTS * rightCount);
    }
    string getPrompt() const { return prompt; }
    string getTopic() const { return topic; }
    int getComplexity() const { return complexity; }
    bool isRight(int pick) const { return pick == solution; }
    int getTotal() const { return rightCount + wrongCount + skipCount; }
    int getRight() const { return rightCount; }
    int getWrong() const { return wrongCount; }
    int getSkips() const { return skipCount; }
};

class BlueprintSection
{
private:
    string topic;
    int complexity, amount;

public:
    BlueprintSection(const string &sectionStr)
    {
        vector<string> parts = chopString(sectionStr, Defs::SPLITTER);
        topic = parts[BlueprintParts::TOPIC];
        amount = stoi(parts[BlueprintParts::AMOUNT]);
        complexity = parseComplex(parts[BlueprintParts::COMPLEXITY]);
    }
    string getTopic() const { return topic; }
    int getComplexity() const { return complexity; }
    int getAmount() const { return amount; }
    

private:
    static int parseComplex(const string &complexStr)
    {
        if (complexStr == "hard")
            return Defs::Levels::TOUGH;
        if (complexStr == "medium")
            return Defs::Levels::MID;

        if (complexStr == "easy")
            return Defs::Levels::SIMPLE;
        return 0;
    }
};

class Blueprint
{
private:
    vector<BlueprintSection> sections;
    string title;
public:
    Blueprint(const string &blueprintStr)
    {
        istringstream iss(blueprintStr);
        char mark;
        iss >> mark;
        getline(iss, title, Defs::MARK);
        string sectionStr;
        while (iss >> sectionStr)
        {
            sections.emplace_back(sectionStr);
        }
    }
    string getTitle() const { return title; }
    string getSectionTopic(int idx) const
    {
        return (idx < sections.size()) ? sections[idx].getTopic() : Defs::VOID;
    }
    int getSectionComplexity(int idx) const
    {
        return (idx < sections.size()) ? sections[idx].getComplexity() : Defs::MISSING;
    }
    int getSectionAmount(int idx) const
    {
        return (idx < sections.size()) ? sections[idx].getAmount() : Defs::MISSING;
    }
};

class ExamResult
{
private:
vector<pair<string, vector<int>>> data;
    string examTitle;

public:
    ExamResult(const string &title, const vector<pair<string, vector<int>>> &data) : examTitle(title), data(data) {}
    string getTitle() const { return examTitle; }
    vector<pair<string, vector<int>>> getData() const { return data; }
};
class Exam
{
private:
    string title;
    shared_ptr<Blueprint> pattern;
    vector<shared_ptr<Query>> queries;

public:
    Exam(const string &title, shared_ptr<Blueprint> pattern) : title(title), pattern(pattern) {}
    void addQuery(shared_ptr<Query> q) { queries.push_back(q); }
    void administer(vector<shared_ptr<ExamResult>> &outcomes)
    {
        vector<int> picks(queries.size(), Numbers::DEFAULT_RESPONSE);
        int current = 0;
        while (current < queries.size())
        {
            queries[current]->display(current + 1, picks[current]);
            int pick = getValidPick();
            if (pick == Defs::Responses::BACK)
            {
                if (current > 0)
                    current--;
                continue;
            }
            picks[current] = pick;
            current++;
        }
        cout << "Finished " << title << ".\n";
        logPicks(picks);
        outcomes.push_back(makeResult(picks));
    }
    string getTitle() const { return title; }
    shared_ptr<Blueprint> getPattern() const { return pattern; }

private:
    int getValidPick() const
    {
        string input;
        while (true)
        {
            cout << "Your answer: ";
            getline(cin, input);
            if (input.empty())
                return Defs::Responses::NONE;
            if (input == Actions::GO_BACK)
                return Defs::Responses::BACK;
            if (input == Numbers::PICK4 || input == Numbers::PICK3 || input == Numbers::PICK2 || input == Numbers::PICK1)
            {
                return stoi(input);
            }
            cout << "Invalid answer, please try again.\n";
        }
    }
    void logPicks(const vector<int> &picks)
    {
        size_t i = 0;
        while (i < picks.size())
        {
            queries[i]->logResponse(picks[i]);
            i++;
        }
    }
    shared_ptr<ExamResult> makeResult(const vector<int> &picks) const
    {
        unordered_map<string, vector<int>> resultData;
        size_t i = 0;
        while (i < picks.size())
        {
            const auto &q = queries[i];
            string topic = q->getTopic();
            if (resultData[topic].empty())
            {
                resultData[topic] = vector<int>(Numbers::DATA_COLS, 0);
            }
            resultData[topic][Defs::Stats::ALL]++;
            if (picks[i] == Defs::Responses::NONE)
            {
                resultData[topic][Defs::Stats::SKIPPED]++;
            }
            else if (q->isRight(picks[i]))
            {
                resultData[topic][Defs::Stats::RIGHT]++;
            }
            else
            {
                resultData[topic][Defs::Stats::WRONG]++;
            }
            i++;
        }
        vector<pair<string, vector<int>>> sortedData(resultData.begin(), resultData.end());
        sort(sortedData.begin(), sortedData.end(), [](const auto &a, const auto &b)
             { return a.first < b.first; });
        return make_shared<ExamResult>(title, sortedData);
    }
};

vector<shared_ptr<Query>> loadQueries(const string &path)
{
    vector<shared_ptr<Query>> queries;
    ifstream file(path);
    string line;
    getline(file, line);
    while (getline(file, line))
    {
        vector<string> fields = chopString(line, Defs::DIVIDER);
        queries.push_back(make_shared<Query>(
            fields[QueryParts::PROMPT],
            fields[QueryParts::CHOICE1],
            fields[QueryParts::CHOICE2],
            fields[QueryParts::CHOICE3],
            fields[QueryParts::CHOICE4],
            stoi(fields[QueryParts::ANSWER]),
            (fields[QueryParts::COMPLEXITY] == "easy") ? Defs::Levels::SIMPLE : (fields[QueryParts::COMPLEXITY] == "medium") ? Defs::Levels::MID
                                                                                                                             : Defs::Levels::TOUGH,
            fields[QueryParts::TOPIC]));
    }
    return queries;
}

string stripMarks(const string &input)
{
    return input.substr(2, input.length() - 3);
}
vector<string> chopString(const string &input, char separator)
{
    vector<string> parts;
    parts.reserve(Numbers::START_SIZE);
    string part;
    istringstream stream(input);
    while (getline(stream, part, separator))
    {
        parts.push_back(part);
    }
    return parts;
}

void pickExamQueries(shared_ptr<Exam> &exam, vector<shared_ptr<Query>> &queries)
{
    sort(queries.begin(), queries.end(), [](const auto &a, const auto &b)
         {
if (a->computeScore() == b->computeScore()) {
return a->getPrompt() < b->getPrompt();
}
return a->computeScore() > b->computeScore(); });
    auto pattern = exam->getPattern();
    int i = 0;
    while (true)
    {
        string topic = pattern->getSectionTopic(i);
        if (topic.empty())
            break;
        int complexity = pattern->getSectionComplexity(i);
        int amount = pattern->getSectionAmount(i);
        int picked = 0;
        size_t j = 0;
        while (j < queries.size() && picked < amount)
        {
            auto &q = queries[j];
            if (q->getTopic() == topic && q->getComplexity() == complexity)
            {
                exam->addQuery(q);
                picked++;
            }
            j++;
        }
        i++;
    }
}

void makeBlueprint(unordered_map<string, shared_ptr<Blueprint>> &blueprints)
{
    string line;
    getline(cin, line);
    auto newBlueprint = make_shared<Blueprint>(line);
    if (blueprints.count(newBlueprint->getTitle()))
    {
        cout << "Duplicate name: '" << newBlueprint->getTitle() << "'\n";
        return;
    }
    blueprints[newBlueprint->getTitle()] = newBlueprint;
    cout << "Blueprint '" << newBlueprint->getTitle() << "' was created successfully.\n";
}

void buildExam(vector<shared_ptr<Query>> &queries, unordered_map<string, shared_ptr<Blueprint>> &blueprints, unordered_map<string, shared_ptr<Exam>> &exams)
{
    string line;
    getline(cin, line);
    vector<string> parts = chopString(line, Defs::MARK);
    auto it = blueprints.find(parts[ExamParts::BLUEPRINT]);
    if (it == blueprints.end())
    {
        cout << "Could not find blueprint: '" << parts[ExamParts::BLUEPRINT] << "'\n";
        return;
    }
    auto newExam = make_shared<Exam>(parts[ExamParts::TITLE], it->second);
    pickExamQueries(newExam, queries);
    exams[newExam->getTitle()] = newExam;
    cout << "Exam '" << newExam->getTitle() << "' was generated successfully.\n";
}

void takeExam(unordered_map<string, shared_ptr<Exam>> &exams, vector<shared_ptr<ExamResult>> &outcomes)
{
    string examTitle;
    getline(cin, examTitle);
    examTitle = stripMarks(examTitle);
    auto it = exams.find(examTitle);
    if (it == exams.end())
    {
        cout << "Could not find exam: '" << examTitle << "'\n";
        return;
    }
    cout << examTitle << ":\n";
    it->second->administer(outcomes);
}

pair<string, string> findWeakTopics(const vector<shared_ptr<Query>> &queries)
{
    unordered_map<string, pair<int, int>> topicStats;
    size_t i = 0;
    while (i < queries.size())
    {
        string topic = queries[i]->getTopic();
        topicStats[topic].first += queries[i]->getRight();
        topicStats[topic].second += queries[i]->getTotal();
        i++;
    }
    vector<pair<string, double>> topicPercents;
    for (const auto &stat : topicStats)
    {
        double percent = (stat.second.first * 100.0) / stat.second.second;
        topicPercents.emplace_back(stat.first, percent);
    }
    sort(topicPercents.begin(), topicPercents.end(), [](const auto &a, const auto &b)
         {
if (a.second == b.second) return a.first < b.first;
return a.second < b.second; });
    return {topicPercents[0].first, topicPercents[1].first};
}

void autoBuildExam(vector<shared_ptr<Query>> &queries, unordered_map<string, shared_ptr<Exam>> &exams)
{
    string examTitle;
    getline(cin, examTitle);
    examTitle = stripMarks(examTitle);
    auto weakest = findWeakTopics(queries);
    string virtualBlueprint = "'" + examTitle + "' " + weakest.first + ":easy:3 " + weakest.first + ":medium:2 " + weakest.first + ":hard:1 " + weakest.second + ":easy:2 " + weakest.second + ":medium:1 " + weakest.second + ":hard:1";
    auto newBlueprint = make_shared<Blueprint>(virtualBlueprint);
    auto newExam = make_shared<Exam>(examTitle, newBlueprint);
    pickExamQueries(newExam, queries);
    exams[examTitle] = newExam;
    cout << "Exam '" << examTitle << "' was generated successfully.\n";
}

vector<pair<string, vector<int>>> makeTopicReport(const vector<shared_ptr<Query>> &queries)
{
    unordered_map<string, vector<int>> report;
    size_t i = 0;
    while (i < queries.size())
    {
        string topic = queries[i]->getTopic();
        if (report[topic].empty())
        {
            report[topic] = vector<int>(Numbers::DATA_COLS, 0);
        }
        report[topic][Defs::Stats::ALL] += queries[i]->getTotal();
        report[topic][Defs::Stats::RIGHT] += queries[i]->getRight();
        report[topic][Defs::Stats::WRONG] += queries[i]->getWrong();
        report[topic][Defs::Stats::SKIPPED] += queries[i]->getSkips();
        i++;
    }
    vector<pair<string, vector<int>>> sortedReport(report.begin(), report.end());
    sort(sortedReport.begin(), sortedReport.end(), [](const auto &a, const auto &b)
         { return a.first < b.first; });
    return sortedReport;
}

vector<int> calcTotalReport(const vector<pair<string, vector<int>>> &topicReport)
{
    vector<int> total(Numbers::DATA_COLS, 0);
    size_t i = 0;
    while (i < topicReport.size())
    {
        for (int j = 0; j < Numbers::DATA_COLS; j++)
        {
            total[j] += topicReport[i].second[j];
        }
        i++;
    }
    return total;
}

void showFullReport(const vector<shared_ptr<Query>> &queries)
{
    auto report = makeTopicReport(queries);
    auto total = calcTotalReport(report);
    cout << "Total report:\n\n";
    size_t i = 0;
    while (i < report.size())
    {
        cout << report[i].first << ": " << report[i].second[Defs::Stats::RIGHT] << " corrects, " << report[i].second[Defs::Stats::WRONG] << " incorrects and " << report[i].second[Defs::Stats::SKIPPED] << " blanks. Score: ";
        if (report[i].second[Defs::Stats::ALL] == 0)
        {
            cout << "0.000%.\n";
        }
        else
        {
            cout << fixed << setprecision(3) << (report[i].second[Defs::Stats::RIGHT] * 100.0) / report[i].second[Defs::Stats::ALL] << "%.\n";
        }
        i++;
    }
    cout << "\nTotal results: " << total[Defs::Stats::RIGHT] << " corrects, " << total[Defs::Stats::WRONG] << " incorrects and " << total[Defs::Stats::SKIPPED] << " blanks.\n";
    cout << "Total score: " << fixed << setprecision(3) << (total[Defs::Stats::RIGHT] * 100.0) / total[Defs::Stats::ALL] << "%.\n";
}

vector<int> calcExamTotal(const vector<shared_ptr<ExamResult>> &outcomes, int idx)
{
    vector<int> total(Numbers::DATA_COLS, 0);
    size_t i = 0;
    while (i < outcomes[idx]->getData().size())
    {
        for (int j = 0; j < Numbers::DATA_COLS; j++)
        {
            total[j] += outcomes[idx]->getData()[i].second[j];
        }
        i++;
    }
    return total;
}


vector<vector<int>> makeComplexityReport(const vector<shared_ptr<Query>> &queries, const string &topic)
{
    vector<vector<int>> report(Numbers::STAT_TYPES, vector<int>(Numbers::LEVELS, 0));
    size_t i = 0;
    while (i < queries.size())
    {
        if (queries[i]->getTopic() == topic)
        {
            int complex = queries[i]->getComplexity();
            report[Defs::Stats::ALL][complex - 1] += queries[i]->getTotal();
            report[Defs::Stats::RIGHT][complex - 1] += queries[i]->getRight();
            report[Defs::Stats::WRONG][complex - 1] += queries[i]->getWrong();
            report[Defs::Stats::SKIPPED][complex - 1] += queries[i]->getSkips();
        }
        i++;
    }
    return report;
}
void showExamReport(const vector<shared_ptr<ExamResult>> &outcomes)
{
    return;
}

void showAllExamReports(const vector<shared_ptr<ExamResult>> &outcomes)
{
    return;
}
void showTopicReport(const vector<shared_ptr<Query>> &queries)
{
    string topic;
    cin >> topic;
    cout << "Results for " << topic << ":\n\n";
    auto report = makeComplexityReport(queries, topic);
    cout << "Easy: " << report[Defs::Stats::RIGHT][Numbers::SIMPLE_IDX] << " corrects, " << report[Defs::Stats::WRONG][Numbers::SIMPLE_IDX] << " incorrects and " << report[Defs::Stats::SKIPPED][Numbers::SIMPLE_IDX] << " blanks.\n";
    cout << "Medium: " << report[Defs::Stats::RIGHT][Numbers::MID_IDX] << " corrects, " << report[Defs::Stats::WRONG][Numbers::MID_IDX] << " incorrects and " << report[Defs::Stats::SKIPPED][Numbers::MID_IDX] << " blanks.\n";
    cout << "Hard: " << report[Defs::Stats::RIGHT][Numbers::TOUGH_IDX] << " corrects, " << report[Defs::Stats::WRONG][Numbers::TOUGH_IDX] << " incorrects and " << report[Defs::Stats::SKIPPED][Numbers::TOUGH_IDX] << " blanks.\n";
    int totalRight = report[Defs::Stats::RIGHT][Numbers::SIMPLE_IDX] + report[Defs::Stats::RIGHT][Numbers::MID_IDX] + report[Defs::Stats::RIGHT][Numbers::TOUGH_IDX];
    int totalAll = report[Defs::Stats::ALL][Numbers::SIMPLE_IDX] + report[Defs::Stats::ALL][Numbers::MID_IDX] + report[Defs::Stats::ALL][Numbers::TOUGH_IDX];
    cout << "\nTotal score: " << fixed << setprecision(3) << (totalRight * 100.0) / totalAll << "%.\n";
}

bool handleAction(vector<shared_ptr<Query>> &queries, unordered_map<string, shared_ptr<Blueprint>> &blueprints, unordered_map<string, shared_ptr<Exam>> &exams, vector<shared_ptr<ExamResult>> &outcomes)
{
    string action;
    if (!(cin >> action))
        return false;
    if (action == Actions::MAKE_BLUEPRINT)
    {
        makeBlueprint(blueprints);
    }
    else if (action == Actions::AUTO_BUILD)
    {
        autoBuildExam(queries, exams);
    }
    
    else if (action == Actions::TAKE)
    {
        takeExam(exams, outcomes);
    }
    else if (action == Actions::BUILD_EXAM)
    {
        buildExam(queries, blueprints, exams);
    }
    else if (action == Actions::SUMMARY)
    {
        string summaryType;
        cin >> summaryType;
        if (summaryType == Actions::FULL_SUMMARY)
        {
            showFullReport(queries);
        }
        else if (summaryType == Actions::ALL_EXAMS_SUMMARY)
        {
            showAllExamReports(outcomes);
        }
        else if (summaryType == Actions::EXAM_SUMMARY)
        {
            showExamReport(outcomes);
        }
        
        else if (summaryType == Actions::TOPIC_SUMMARY)
        {
            showTopicReport(queries);
        }
    }
    return true;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        cerr << "Please provide question file path\n";
        return 1;
    }
    auto queries = loadQueries(argv[1]);
    unordered_map<string, shared_ptr<Blueprint>> blueprints;
    unordered_map<string, shared_ptr<Exam>> exams;
    vector<shared_ptr<ExamResult>> outcomes;
    while (handleAction(queries, blueprints, exams, outcomes))
        ;
    return 0;
}
