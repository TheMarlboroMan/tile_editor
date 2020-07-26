#pragma once

#include "../blueprint_types/map_blueprint.h"

#include <tools/text_reader.h>

#include <string>

namespace tools {
	class text_reader;
}

namespace tile_editor {

class config_parser {

	public:

	map_blueprint           read(const std::string&);

	private:

	void                    map_property_mode(tools::text_reader&, map_blueprint&);
	void                    tile_mode(tools::text_reader&, map_blueprint&);
	void                    thing_mode(tools::text_reader&, map_blueprint&);
};
}
