#pragma once

#include "map_blueprint.h"

#include <tools/text_reader.h>

#include <string>

namespace tools {
	class text_reader;
}

namespace tile_editor {

class config_file_parser {

	public:

	map_blueprint           read(const std::string&);

	private:

	struct config_pair {

		std::string name,
		            value;
		bool        failed=false,
		            disallowed=false,
		            eof=false;
	};

	config_pair             from_reader(tools::text_reader&);
	config_pair             from_reader(tools::text_reader&, const std::vector<std::string>&);
	void                    map_property_mode(tools::text_reader&);
	void                    tile_mode(tools::text_reader&);
	void                    thing_mode(tools::text_reader&);

};
}
