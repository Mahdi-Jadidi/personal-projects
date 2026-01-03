#pragma once

#include "include/Question.hpp"
#include <vector>
#include <memory>

class Blueprint {
private:
    std::vector<BlueprintSection> sections;
    std::string title;

public:
    Blueprint(const std::string &blueprintStr);
    std::string getTitle() const;
    std::string getSectionTopic(int idx) const;
    int getSectionComplexity(int idx) const;
    int getSectionAmount(int idx) const;
};
