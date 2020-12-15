#pragma once

#include "editor_types/map.h"
#include <rapidjson/document.h>
#include <string>
#include <vector>
#include "editor_types/poly_layer.h"

namespace tile_editor {

//!Map parser. Unlike its blueprint counterpart, it will always try to go on
//!and parse whatever we throw at it, even if its syntax is botched. It, however
//!will try to emit error messages for everything that it did not expect
//!(missing nodes, bad types, extraneous entries...).

class map_parser {

	public:

	using errvector=std::vector<std::string>;

	map                    parse_file(const std::string&);
	map                    parse_string(const std::string&);
	const errvector&       get_errors() const {return errors;}
	const std::string      get_version() const {return version;}

	private:

	using jsondoc=rapidjson::Document;
	using jsonval=rapidjson::Value;

	struct meta {
		enum class types{tiles, things, polys, bad};
		std::size_t        set;
		int                alpha;
		std::string        id;
		types              type;
		//These two are only for poly nodes, still these are short lived objects,
		//let us forget about the transgression.
		poly_layer::windings winding{poly_layer::windings::clockwise};
		poly_layer::curves curve{poly_layer::curves::convex};
	};

	void                   parse_meta(const jsondoc&);
	void                   parse_attributes(const jsondoc&, map&);
	void                   parse_layers(const jsondoc&, map&);
	void                   parse_tile_layer(const jsonval&, const meta&, map&);
	void                   parse_thing_layer(const jsonval&, const meta&, map&);
	void                   parse_poly_layer(const jsonval&, const meta&, map&);
	meta                   parse_meta_node(const jsonval&);
	bool                   check_data_node(const jsonval&, const std::string&);
	void                   parse_attributes(const jsonval&, tile_editor::property_manager&);
	std::string            generate_default_id();

	errvector              errors;
	std::string            version;
	int                    default_id_count{0};
};

}
