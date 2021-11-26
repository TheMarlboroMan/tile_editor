#pragma once

#include "../blueprint_types/map_blueprint.h"

#include <tools/string_reader.h>
#include <lm/logger.h>

#include <string>

namespace tools {
	class text_reader;
}

namespace tile_editor {

class blueprint_parser {

	public:

	                        blueprint_parser(lm::logger&);
	map_blueprint           parse_file(const std::string&);
	map_blueprint           parse_string(const std::string&, const std::string& = "");

	private:

	lm::logger&             log;
	std::string             config_file_dir;

	void                    map_property_mode(tools::string_reader&, map_blueprint&);
	void                    tile_mode(tools::string_reader&, map_blueprint&);
	void                    thing_mode(tools::string_reader&, map_blueprint&);
	void                    poly_mode(tools::string_reader&, map_blueprint&);
	void                    session_mode(tools::string_reader&, map_blueprint&);
	void                    grid_settings_mode(tools::string_reader&, map_blueprint&);
};
}
