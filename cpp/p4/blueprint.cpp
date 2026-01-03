#include "blueprint.hpp"
#include <sstream>
#include <stdexcept>

BlueprintSection::BlueprintSection(const std::string &sectionStr) {
    std::vector<std::string> parts = chopString(sectionStr, Defs::SPLITTER);
    topic = parts[BlueprintParts::TOPIC];
    
    if (parts[BlueprintParts::COMPLEXITY] == "easy")
        complexity = Defs::Levels::SIMPLE;
    else if (parts[BlueprintParts::COMPLEXITY] == "medium")
        complexity = Defs::Levels::MID;
    else if (parts[BlueprintParts::COMPLEXITY] == "hard")
        complexity = Defs::Levels::TOUGH;
    else
        throw std::invalid_argument("Invalid complexity level");
    
    amount = std::stoi(parts[BlueprintParts::AMOUNT]);
}

std::string BlueprintSection::getTopic() const { return topic; }
int BlueprintSection::getComplexity() const { return complexity; }
int BlueprintSection::getAmount() const { return amount; }

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
