#pragma once

#include "editor_types/map.h"
#include <rapidjson/document.h>
#include <string>
#include <vector>

namespace tile_editor {

class map_parser {

	public:

	using errvector=std::vector<std::string>;
	using jsondoc=rapidjson::Document;

	map                    parse_file(const std::string&);
	map                    parse_string(const std::string&);
	const errvector&       get_errors() const {return errors;}
	const std::string      get_version() const {return version;}

	private:

	void                   parse_meta(const jsondoc&);
	void                   parse_attributes(const jsondoc&, map&);
	void                   parse_tiles(const jsondoc&, map&);
	void                   parse_tile_layer(const jsondoc&, map&);
	void                   parse_things(const jsondoc&, map&);
	void                   parse_thing_layer(const jsondoc&, map&);
	void                   parse_polys(const jsondoc&, map&);
	void                   parse_poly_layer(const jsondoc&, map&);

	errvector              errors;
	std::string            version;
};

}
