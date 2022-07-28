#include "parser/blueprint_parser.h"
#include "parser/property_parser.h"
#include "parser/parse_tools.h"
#include "parser/thing_parser.h"
#include "parser/poly_parser.h"

#include <tools/file_utils.h>
#include <tools/string_utils.h>
#include <ldtools/sprite_table.h>
#include <lm/sentry.h>

#include <stdexcept>
#include <fstream>
#include <string>
#include <sstream>
#include <filesystem>
#include <iostream>

using namespace tile_editor;

blueprint_parser::blueprint_parser(
	lm::logger& _log
):
	log{_log}
{}

map_blueprint blueprint_parser::parse_file(const std::string& _filename) {

	if(!tools::file_exists(_filename)) {

		throw std::runtime_error(std::string{"cannot find file '"}+_filename+"'");
	}

	try {

		//set the config file directory so any secondary files can be traced
		//as relative to them.
		std::filesystem::path path{_filename};
		path.remove_filename();
		lm::log(log, lm::lvl::debug)<<"will use '"<<path<<"' as relative path for the config file"<<std::endl;

		return parse_string(tools::dump_file(_filename), path);
	}
	catch(std::exception& e) {

		throw std::runtime_error(
			std::string{
				e.what()
			}
			+" on file "+_filename
		);
	}
}

map_blueprint blueprint_parser::parse_string(
	const std::string& _contents,
	const std::string& _file_dir
) {

	config_file_dir=_file_dir;

	const std::string   beginprop{"beginmapproperties"},
	                    begintile{"begintileset"},
	                    beginobj{"beginobjectset"},
	                    beginpoly{"beginpolyset"},
	                    beginsession{"beginsession"},
	                    begingridsettings{"begingridsettings"},
	                    begindefaultlayer{"begindefaultlayer"};

	map_blueprint mb;
	int flags=tools::string_reader::ltrim | tools::string_reader::rtrim | tools::string_reader::ignorewscomment;
	tools::string_reader reader{_contents, '#', flags};
	bool properties_set=false;

	try {

		std::vector<default_layer> default_layers;

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
			else if(tag==begingridsettings) {

				grid_settings_mode(reader, mb);
			}
			else if(tag==begindefaultlayer) {

				default_layers.push_back(default_layer_mode(reader));
			}
			else {

				throw std::runtime_error(std::string{"unexpected '"+tag+"', expected beginmapproperties, begintileset or beginobjectset"});
			}
		}

		if(!mb.gridsets.size()) {

			//Add a default gridset!!!
			mb.gridsets.insert(std::make_pair(1, grid_data{}));
		}

		//Check all default layers and add them.
		for(const auto& layer : default_layers) {

			if(!mb.gridsets.count(layer.grid_id)) {

				throw std::runtime_error(std::string{"non existing grid id for default layer"});
			}

			if(layer.alpha < 0 || layer.alpha > 255) {

				throw std::runtime_error(std::string{"bad alpha for default layer"});
			}

			switch(layer.type) {

				case default_layer::types::tile:

					if(!mb.tilesets.count(layer.set_id)) {

						throw std::runtime_error(std::string{"non existing set id for default tile layer"});
					}
				break;
				case default_layer::types::poly:

					if(!mb.polysets.count(layer.set_id)) {

						throw std::runtime_error(std::string{"non existing set id for default polygon layer"});
					}
				break;
				case default_layer::types::thing:

					if(!mb.thingsets.count(layer.set_id)) {

						throw std::runtime_error(std::string{"non existing set id for default thing layer"});
					}
				break;
			}
		}

		std::swap(mb.default_layers, default_layers);

		return mb;

	}
	catch(std::exception& e) {

		throw std::runtime_error(
			std::string{e.what()}
			+" line "+std::to_string(reader.get_line_number())
		);
	}


}

void blueprint_parser::map_property_mode(
	tools::string_reader& _reader,
	map_blueprint& _blueprint
) {

	lm::log(log, lm::lvl::debug)<<"entering map property mode"<<std::endl;
	auto propmap=generic_first_level(_reader, "endmapproperties", {"file"});

	property_parser pp(true);

	std::string filename{config_file_dir+propmap["file"]};

	_blueprint.properties=pp.read_file(filename);
	lm::log(log, lm::lvl::debug)<<"read "<<_blueprint.properties.size()<<" map properties from "<<filename<<std::endl;
}

void blueprint_parser::tile_mode(
	tools::string_reader& _reader,
	map_blueprint& _blueprint
) {

	lm::log(log, lm::lvl::debug)<<"entering tile mode"<<std::endl;

	auto propmap=generic_first_level(_reader, "endtileset", {"file", "id", "image", "name"});

	std::stringstream ss{propmap["id"]};
	std::size_t index{};

	ss>>index;

	if(ss.fail()) {

		throw std::runtime_error("invalid id value");
	}

	if(_blueprint.tilesets.count(index)) {

		throw std::runtime_error("repeated id value");
	}

	std::string filename{config_file_dir+propmap["file"]};
	std::string imagefile{config_file_dir+propmap["image"]};

	_blueprint.tilesets[index]={
		ldtools::sprite_table{filename},
		imagefile,
		propmap["name"]
	};

	lm::log(log, lm::lvl::debug)<<"read "<<_blueprint.tilesets[index].table.size()<<" entries for tileset with index "<<index<<" from "<<filename<<std::endl;
}

void blueprint_parser::thing_mode(
	tools::string_reader& _reader,
	map_blueprint& _blueprint
) {

	lm::log(log, lm::lvl::debug)<<"entering thing mode"<<std::endl;

	auto propmap=generic_first_level(_reader, "endobjectset", {"file", "id", "name"});

	std::stringstream ss{propmap["id"]};
	std::size_t index{};

	ss>>index;

	if(ss.fail()) {

		throw std::runtime_error("invalid id value");
	}

	if(_blueprint.thingsets.count(index)) {

		throw std::runtime_error("repeated id value");
	}

	//TODO: if the alpha for the thing is zero,
	//should emit a warning.
	std::string filename{config_file_dir+propmap["file"]};

	thing_parser tp;
	_blueprint.thingsets.emplace(
		index,
		thing_definition_table{propmap["name"], tp.read_file(filename)}
	);

	lm::log(log, lm::lvl::debug)<<"read "<<_blueprint.thingsets[index].table.size()<<" entries for thingset with index "<<index<<" from "<<filename<<std::endl;
}

void blueprint_parser::poly_mode(
	tools::string_reader& _reader,
	map_blueprint& _blueprint
) {

	lm::log(log, lm::lvl::debug)<<"entering poly mode"<<std::endl;

	auto propmap=generic_first_level(_reader, "endpolyset", {"file", "id", "name"});

	std::stringstream ss{propmap["id"]};
	std::size_t index{};

	ss>>index;

	if(ss.fail()) {

		throw std::runtime_error("invalid id value");
	}

	if(_blueprint.polysets.count(index)) {

		throw std::runtime_error("repeated id value");
	}

	//TODO: if the alpha for the poly is zero,
	//should emit a warning.

	poly_parser pp;

	const std::string filename{config_file_dir+propmap["file"]};

	_blueprint.polysets.emplace(
		index,
		poly_definition_table{propmap["name"], pp.read_file(filename)}
	);

	lm::log(log, lm::lvl::debug)<<"read "<<_blueprint.polysets[index].table.size()<<" entries for polyset with index "<<index<<" from "<<filename<<std::endl;
}

void blueprint_parser::session_mode(
	tools::string_reader& _reader,
	map_blueprint& _blueprint
) {

	lm::log(log, lm::lvl::debug)<<"entering session mode"<<std::endl;

	auto propmap=generic_first_level(_reader, "endsession", {
		"thingcenter",
		"bgcolor",
		"fontcolor",
		"toolboxwidthpercent"
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

	if(propmap["fontcolor"].size()) {

		_blueprint.font_color=parse_color(propmap["fontcolor"]);
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

	if(propmap["toolboxwidthpercent"].size()) {

		int percent=to_int(propmap["gridsize"], "toolboxwidthpercent");

		if(percent < 1 || percent > 100) {

			throw std::runtime_error("toolboxwidth percent must be between 1 and 100");
		}

		_blueprint.toolbox_width_percent=percent;

	}
}

void blueprint_parser::grid_settings_mode(
	tools::string_reader& _reader,
	map_blueprint& _blueprint
) {

	lm::log(log, lm::lvl::debug)<<"entering grid settings mode"<<std::endl;

	auto propmap=generic_first_level(_reader, "endgridsettings", {
		"id",
		"name",
		"gridsize",
		"gridvruler",
		"gridhruler",
		"gridcolor",
		"gridrulercolor",
		"gridorigincolor",
		"gridsubcolor",

	}, false);

	auto to_int=[](const std::string& _str, const std::string& _key) -> int {

		std::stringstream ss(_str);
		int result{};
		ss>>result;

		if(ss.fail()) {

			throw std::runtime_error(std::string{"invalid int value for '"}+_key+"'");
		}

		return result;
	};

	if(!propmap.count("id")) {

		throw std::runtime_error(std::string{"grid settings require the id key to be defined"});
	}

	if(!propmap.count("name")) {

		throw std::runtime_error(std::string{"grid settings require the name key to be defined"});
	}

	std::size_t id=to_int(propmap["id"], "id");

	_blueprint.gridsets[id]=grid_data{};
	auto& current=_blueprint.gridsets.at(id);

	current.name=propmap["name"];

	if(propmap["gridcolor"].size()) {

		current.color=parse_color(propmap["gridcolor"]);
	}

	if(propmap["gridrulercolor"].size()) {

		current.ruler_color=parse_color(propmap["gridrulercolor"]);
	}

	if(propmap["gridorigincolor"].size()) {

		current.origin_color=parse_color(propmap["gridorigincolor"]);
	}

	if(propmap["gridsubcolor"].size()) {

		current.subcolor=parse_color(propmap["gridsubcolor"]);
	}

	if(propmap["gridsize"].size()) {

		current.size=to_int(propmap["gridsize"], "gridsize");
	}

	if(propmap["gridvruler"].size()) {

		current.vertical_ruler=to_int(propmap["gridvruler"], "gridvruler");
	}

	if(propmap["gridhruler"].size()) {

		current.horizontal_ruler=to_int(propmap["gridhruler"], "gridhruler");
	}
}

default_layer blueprint_parser::default_layer_mode(
	tools::string_reader& _reader
) {

	auto propmap=generic_first_level(
		_reader,
		"enddefaultlayer", {"name", "type", "setid", "gridid", "alpha"}
	);

	default_layer::types type{default_layer::types::tile};

	if(propmap["type"]=="tile") {

		type=default_layer::types::tile;
	}
	else if(propmap["type"]=="thing") {

		type=default_layer::types::thing;
	}
	else if(propmap["type"]=="poly") {

		type=default_layer::types::poly;
	}
	else {

		throw std::runtime_error(std::string{"bad default layer type, must be tile, thing or poly"});
	}

	std::stringstream ss{propmap["setid"]};
	std::size_t set_id{}, grid_id{};
	int alpha{};

	ss>>set_id;
	if(ss.fail()) {

		throw std::runtime_error("invalid set id value for default layer");
	}

	ss.clear();
	ss.str(propmap["gridid"]);
	ss>>grid_id;
	if(ss.fail()) {

		throw std::runtime_error("invalid grid id value for default layer");
	}

	ss.clear();
	ss.str(propmap["alpha"]);
	ss>>alpha;
	if(ss.fail()) {

		throw std::runtime_error("invalid alpha value for default layer");
	}

	lm::log(log, lm::lvl::debug)<<"read default layer"<<std::endl;

	return default_layer{propmap["name"], type, set_id, grid_id, alpha};
}
