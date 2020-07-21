#pragma once

#include "map_blueprint.h"

#include <string>

namespace tile_editor {

class config_file_parser {

	public:

	map_blueprint           read(const std::string&);

	private:

	std::string             preprocess_line(std::ifstream&, bool&) const;
	void                    property_mode(std::ifstream&);
	void                    tile_mode(std::ifstream&);
	void                    thing_mode(std::ifstream&);

};
}