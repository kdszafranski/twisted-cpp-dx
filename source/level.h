#pragma once

#include "constants.h"
#include <vector>
#include <string>
#include "enums.h"

/// LEVELS

struct Level {
	std::string levelName;
	std::vector<BLOCK> data;
};

