#include "include/Template.hpp"
#include "include/Utils.hpp"
#include <sstream>

Blueprint::Blueprint(const std::string &blueprintStr) {
    std::istringstream iss(blueprintStr);
    char mark;
    iss >> mark;
    getline(iss, title, Defs::MARK);
    
    std::string sectionStr;
    while (iss >> sectionStr) {
        sections.emplace_back(sectionStr);
    }
}

std::string Blueprint::getTitle() const { return title; }

std::string Blueprint::getSectionTopic(int idx) const {
    return (idx < sections.size()) ? sections[idx].getTopic() : Defs::VOID;
}

int Blueprint::getSectionComplexity(int idx) const {
    return (idx < sections.size()) ? sections[idx].getComplexity() : Defs::MISSING;
}

int Blueprint::getSectionAmount(int idx) const {
    return (idx < sections.size()) ? sections[idx].getAmount() : Defs::MISSING;
}
