#include "parser/blueprint_parser.h"
#include "parser/property_parser.h"
#include "parser/parse_tools.h"
#include "parser/thing_parser.h"
#include "parser/poly_parser.h"

#include <tools/file_utils.h>
#include <tools/string_utils.h>
#include <ldtools/sprite_table.h>

#include <stdexcept>
#include <fstream>
#include <string>
#include <sstream>

using namespace tile_editor;

map_blueprint blueprint_parser::read(const std::string& _filename) {

	if(!tools::file_exists(_filename)) {

		throw std::runtime_error(std::string{"cannot find file '"}+_filename+"'");
	}

	const std::string   beginprop{"beginmapproperties"},
	                    begintile{"begintileset"},
	                    beginobj{"beginobjectset"},
	                    beginpoly{"beginpolyset"},
	                    beginsession{"beginsession"};

	map_blueprint mb;
	int flags=tools::text_reader::ltrim | tools::text_reader::rtrim | tools::text_reader::ignorewscomment;
	tools::text_reader reader{_filename, '#', flags};
	bool properties_set=false;

	try {
		while(true) {

			std::stringstream ss{reader.read_line()};
			if(reader.is_eof()) {
				break;
			}
			
			//We can only expect beginmapproperties, begintileset, beginobjectset...
			//Skip all whitespace in the extraction operations that will follow.
			std::string tag;
			ss>>std::skipws>>tag;

			if(tag==beginprop) {

				if(properties_set) {

					throw std::runtime_error("only one mapproperty node can be specified");
				}

				map_property_mode(reader, mb);
				properties_set=true;
			}
			else if(tag==begintile) {

				tile_mode(reader, mb);
			}
			else if(tag==beginobj) {

				thing_mode(reader, mb);
			}
			else if(tag==beginpoly) {

				poly_mode(reader, mb);
			}
			else if(tag==beginsession) {

				session_mode(reader, mb);
			}
			else {

				throw std::runtime_error(std::string{"unexpected '"+tag+"', expected beginmapproperties, begintileset or beginobjectset"});
			}
		}
	}
	catch(std::exception& e) {

		throw std::runtime_error(
			std::string{e.what()}
			+" on file "+_filename
			+" line "+std::to_string(reader.get_line_number())
		);
	}

	return mb;
}

void blueprint_parser::map_property_mode(
	tools::text_reader& _reader,
	map_blueprint& _blueprint
) {

	auto propmap=generic_first_level(_reader, "endmapproperties", {"file"});

	property_parser pp;
	_blueprint.properties=pp.read_file(propmap["file"]);
}

void blueprint_parser::tile_mode(
	tools::text_reader& _reader,
	map_blueprint& _blueprint
) {

	auto propmap=generic_first_level(_reader, "endtileset", {"file", "id", "image"});

	std::stringstream ss{propmap["id"]};
	std::size_t index{};
	
	ss>>index;

	if(ss.fail()) {

		throw std::runtime_error("invalid id value");
	}

	if(_blueprint.tilesets.count(index)) {

		throw std::runtime_error("repeated id value");
	}

	_blueprint.tilesets[index]={
		ldtools::sprite_table{propmap["file"]},
		propmap["image"]
	};
}

void blueprint_parser::thing_mode(
	tools::text_reader& _reader,
	map_blueprint& _blueprint
) {

	auto propmap=generic_first_level(_reader, "endobjectset", {"file", "id"});
	
	std::stringstream ss{propmap["id"]};
	std::size_t index{};
	
	ss>>index;

	if(ss.fail()) {

		throw std::runtime_error("invalid id value");
	}

	if(_blueprint.thingsets.count(index)) {

		throw std::runtime_error("repeated id value");
	}

	thing_parser tp;
	_blueprint.thingsets[index]=tp.read_file(propmap["file"]);
}

void blueprint_parser::poly_mode(
	tools::text_reader& _reader, 
	map_blueprint& _blueprint
) {

	auto propmap=generic_first_level(_reader, "endpolyset", {"file", "id"});
	
	std::stringstream ss{propmap["id"]};
	std::size_t index{};
	
	ss>>index;

	if(ss.fail()) {

		throw std::runtime_error("invalid id value");
	}

	if(_blueprint.polysets.count(index)) {

		throw std::runtime_error("repeated id value");
	}

	poly_parser pp;
	_blueprint.polysets[index]=pp.read_file(propmap["file"]);
}

void blueprint_parser::session_mode(
	tools::text_reader& _reader, 
	map_blueprint& _blueprint
) {

	auto propmap=generic_first_level(_reader, "endsession", {
		"thingcenter", 
		"bgcolor",
		"gridsize",
		"gridvruler",
		"gridhruler",
		"gridcolor",
		"gridrulercolor",
	}, false);

	const std::string& thingcenter=propmap["thingcenter"];
	if(thingcenter.size()) {

		if(thingcenter=="center") {

			_blueprint.thing_center=map_blueprint::thing_centers::center;
		}
		else if(thingcenter=="topleft") {

			_blueprint.thing_center=map_blueprint::thing_centers::top_left;
		}
		else if(thingcenter=="topright") {

			_blueprint.thing_center=map_blueprint::thing_centers::top_right;
		}
		else if(thingcenter=="bottomright") {

			_blueprint.thing_center=map_blueprint::thing_centers::bottom_right;
		}
		else if(thingcenter=="bottomleft") {

			_blueprint.thing_center=map_blueprint::thing_centers::bottom_left;
		}
		else {

			throw std::runtime_error("invalid value for thingcenter, valid values are center, topleft, topright, bottomright and bottomleft");
		}
	}

	if(propmap["bgcolor"].size()) {

		_blueprint.bg_color=parse_color(propmap["bgcolor"]);
	}

	if(propmap["gridcolor"].size()) {

		_blueprint.grid_data.color=parse_color(propmap["gridcolor"]);
	}

	if(propmap["gridrulercolor"].size()) {

		_blueprint.grid_data.ruler_color=parse_color(propmap["gridrulercolor"]);
	}

	auto to_int=[](const std::string& _str, const std::string& _key) -> int {

		std::stringstream ss(_str);
		int result{};
		ss>>result;

		if(ss.fail()) {

			throw std::runtime_error(std::string{"invalid int value for '"}+_key+"'");
		}

		return result;
	};

	if(propmap["gridsize"].size()) {

		_blueprint.grid_data.size=to_int(propmap["gridsize"], "gridsize");
	}

	if(propmap["gridvruler"].size()) {

		_blueprint.grid_data.vertical_ruler=to_int(propmap["gridvruler"], "gridvruler");
	}

	if(propmap["gridhruler"].size()) {

		_blueprint.grid_data.horizontal_ruler=to_int(propmap["gridhruler"], "gridhruler");
	}
}

