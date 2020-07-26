#pragma once

#include <tools/text_reader.h>

#include <string>
#include <map>
#include <vector>

namespace tile_editor {

struct config_pair {

	std::string name,
	            value;
	bool        failed=false,
	            disallowed=false,
	            eof=false;
};

config_pair     from_reader(tools::text_reader&);
config_pair     from_reader(tools::text_reader&, const std::vector<std::string>&);
std::map<std::string, std::string> generic_first_level(tools::text_reader&, const std::string&, const std::vector<std::string>&);
}
