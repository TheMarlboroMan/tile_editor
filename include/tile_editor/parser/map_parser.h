#pragma once

#include "editor_types/map.h"
#include <rapidjson/document.h>
#include <string>
#include <vector>

namespace tile_editor {

//!Map parser. Unlike its blueprint counterpart, it will always try to go on
//!and parse whatever we throw at it, even if its syntax is botched. It, however
//!will try to emit error messages for everything that it did not expect 
//!(missing nodes, bad types, extraneous entries...).

class map_parser {

	public:

	using errvector=std::vector<std::string>;
	using jsondoc=rapidjson::Document;
	using jsonval=rapidjson::Value;

	map                    parse_file(const std::string&);
	map                    parse_string(const std::string&);
	const errvector&       get_errors() const {return errors;}
	const std::string      get_version() const {return version;}

	private:

	struct meta {
		std::size_t        set;
		int                alpha;
	};

	void                   parse_meta(const jsondoc&);
	void                   parse_attributes(const jsondoc&, map&);
	void                   parse_tiles(const jsondoc&, map&);
	void                   parse_tile_layer(const jsonval&, map&);
	void                   parse_things(const jsondoc&, map&);
	void                   parse_thing_layer(const jsondoc&, map&);
	void                   parse_polys(const jsondoc&, map&);
	void                   parse_poly_layer(const jsondoc&, map&);
	meta                   parse_meta_node(const jsonval&);
	bool                   check_data_node(const jsonval&, const std::string&);

	errvector              errors;
	std::string            version;
};

}
