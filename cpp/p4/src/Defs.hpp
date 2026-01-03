#pragma once

#include <string>

namespace Defs {
    const std::string VOID = "";
    const int MISSING = -1;
    const char DIVIDER = ',';
    const char SPLITTER = ':';
    const char MARK = '\'';

    namespace Levels {
        const int SIMPLE = 1;
        const int MID = 2;
        const int TOUGH = 3;
    }

    namespace Stats {
        const int ALL = 0;
        const int RIGHT = 1;
        const int WRONG = 2;
        const int SKIPPED = 3;
    }

    namespace Responses {
        const int BACK = -2;
        const int BAD = -1;
        const int NONE = 0;
    }
}

namespace Numbers {
    const int CHOICES = 4;
    const int START_SIZE = 10;
    const int DEFAULT_RESPONSE = Defs::Responses::NONE;
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

namespace Actions {
    const std::string MAKE_BLUEPRINT = "create_template";
    const std::string BUILD_EXAM = "generate_test";
    const std::string TAKE = "attend";
    const std::string GO_BACK = "previous";
    const std::string AUTO_BUILD = "auto_generate";
    const std::string SUMMARY = "report";
    const std::string FULL_SUMMARY = "all";
    const std::string EXAM_SUMMARY = "test";
    const std::string ALL_EXAMS_SUMMARY = "tests";
    const std::string TOPIC_SUMMARY = "subject";
}
