#ifndef BLUEPRINT_HPP
#define BLUEPRINT_HPP

#include <string>
#include <vector>
#include "defs.hpp"

class BlueprintSection {
private:
    std::string topic;
    int complexity;
    int amount;

public:
    BlueprintSection(const std::string &sectionStr);
    std::string getTopic() const;
    int getComplexity() const;
    int getAmount() const;
};

class Blueprint {
private:
    std::string title;
    std::vector<BlueprintSection> sections;

public:
    Blueprint(const std::string &blueprintStr);
    std::string getTitle() const;
    std::string getSectionTopic(int idx) const;
    int getSectionComplexity(int idx) const;
    int getSectionAmount(int idx) const;
};

#endif // BLUEPRINT_HPP
