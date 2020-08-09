#include "parser/map_parser.h"

#include <tools/json.h>
#include <tools/file_utils.h>
#include <stdexcept>

//TODO: Erase...
#include <iostream>

using namespace tile_editor;

map map_parser::parse_file(const std::string& _filename) {

	errors.clear();
	version.clear();

	if(!tools::file_exists(_filename)) {

		errors.push_back("map file does not exist");
		return map{};
	}

	return parse_string(tools::dump_file(_filename));
}

map map_parser::parse_string(const std::string& _contents) {

	map result;
	errors.clear();
	version.clear();

	try {

		auto doc=tools::parse_json_string(_contents);

		if(!doc.IsObject()) {

			errors.push_back("json root node must be an object");
			return result;
		}

		parse_meta(doc);
		parse_attributes(doc, result);
		parse_tiles(doc, result);
		parse_things(doc, result);
		parse_polys(doc, result);
	}
	//Can only be thrown in parse_json_string.
	catch(tools::parse_json_string_exception& e) {

		errors.push_back(std::string{"could not parse json: "}+e.what());
	}
	catch(std::exception& e) {

		errors.push_back(std::string{"fatal error, map is likely not loaded: "}+e.what());
	}

	return result;
}


void map_parser::parse_meta(const jsondoc& _doc) {

	if(!_doc.HasMember("meta")) {

		errors.push_back("no 'meta' node found, metadata will be skipped");
		return;
	}

	if(!_doc["meta"].IsObject()) {

		errors.push_back("'meta' node must be an object, metadata will be skipped");
		return;
	}

	if(!_doc["meta"].HasMember("version")) {

		errors.push_back("'meta' node must contain version, version will be skipped");
	}
	else {

		if(!_doc["meta"]["version"].IsString()) {

			errors.push_back("'meta' node must contain version as a string, version will be skipped");
		}
		else {

			version=_doc["meta"]["version"].GetString();
		}
	}
}

void map_parser::parse_attributes(const jsondoc& _doc, map& _map) {

	if(!_doc.HasMember("attributes")) {

		errors.push_back("no 'attributes' node found, attributes will be skipped");
		return;
	}

	if(!_doc["attributes"].IsObject()) {

		errors.push_back("'attributes' node must be an object, attributes will be skipped");
		return;
	}

	parse_attributes(_doc["attributes"], _map.properties);
}

void map_parser::parse_tiles(const jsondoc& _doc, map& _map) {

	if(!_doc.HasMember("tiles")) {

		errors.push_back("no 'tiles' node found, tiles will be skipped");
		return;
	}

	if(!_doc["tiles"].IsArray()) {

		errors.push_back("'tiles' node must be an array, tiles will be skipped");
		return;
	}

	for(const auto& item : _doc["tiles"].GetArray()) {

		parse_tile_layer(item, _map);
	}
}

void map_parser::parse_tile_layer(const jsonval& _node, map& _map) {

	if(!_node.IsObject()) {

		errors.push_back("tile layer node must be an object, skipping layer");
		return;
	}

	auto metadata=parse_meta_node(_node);

	if(!check_data_node(_node, "tile")) {

		return;
	}

	//create the layer.
	tile_layer layer{metadata.set, metadata.alpha, {}};

	//iterate on its items.
	for(const auto& item : _node["data"].GetArray()) {

		if(!item.IsObject()) {
			errors.push_back("tile item is not an object, skipping item");
			continue;
		}

		if(!item.HasMember("t")) {

			errors.push_back("tile item has no 't' property, skipping item");
			continue;
		}

		if(!item["t"].IsInt()) {

			errors.push_back("tile item 't' is not an integer, skipping item");
			continue;
		}

		if(!item.HasMember("p")) {

			errors.push_back("tile item has no 'p' property, skipping item");
			continue;
		}

		if(!item["p"].IsArray()) {

			errors.push_back("tile item 'p' is not an array, skipping item");
			continue;
		}

		if(2!=item["p"].Size()) {

			errors.push_back("tile item 'p' must have exactly two elements, skipping item");
			continue;
		}

		if(!item["p"][0].IsInt()) {

			errors.push_back("tile item 'p' must have an integer as its first value, skipping item");
			continue;
		}

		if(!item["p"][1].IsInt()) {

			errors.push_back("tile item 'p' must have an integer as its second value, skipping item");
			continue;
		}

		tile_editor::tile tile{
			item["p"][0].GetInt(),
			item["p"][1].GetInt(),
			item["t"].GetInt()
		};

		layer.data.push_back(tile);

		if(item.Size() > 2) {

			errors.push_back("tile layer item has extraneous members that will be skipped");
		}
	}

	if(_node.Size() > 2) {

		errors.push_back("tile layer node has extraneous members that will be skipped");
	}

	//Add the layer to the map.
	_map.tile_layers.push_back(std::move(layer));
}

void map_parser::parse_things(const jsondoc& _doc, map& _map) {

	if(!_doc.HasMember("things")) {

		errors.push_back("no 'things' node found, things will be skipped");
		return;
	}

	if(!_doc["things"].IsArray()) {

		errors.push_back("'things' node must be an array, things will be skipped");
		return;
	}

	for(const auto& item : _doc["things"].GetArray()) {

		parse_thing_layer(item, _map);
	}
}

void map_parser::parse_thing_layer(const jsonval& _node, map& _map) {

	if(!_node.IsObject()) {

		errors.push_back("thing layer node must be an object, skipping layer");
		return;
	}

	auto metadata=parse_meta_node(_node);

	if(!check_data_node(_node, "thing")) {

		return;
	}

	thing_layer layer{metadata.set, metadata.alpha, {}};

	for(const auto& item : _node["data"].GetArray()) {

		if(!item.IsObject()) {
			errors.push_back("thing item is not an object, skipping item");
			continue;
		}

		if(!item.HasMember("t")) {

			errors.push_back("thing item has no 't' property, skipping item");
			continue;
		}

		if(!item["t"].IsInt()) {

			errors.push_back("thing item 't' is not an integer, skipping item");
			continue;
		}

		if(!item.HasMember("p")) {

			errors.push_back("thing item has no 'p' property, skipping item");
			continue;
		}

		if(!item["p"].IsArray()) {

			errors.push_back("thing item 'p' is not an array, skipping item");
			continue;
		}

		if(2!=item["p"].Size()) {

			errors.push_back("thing item 'p' must have exactly two elements, skipping item");
			continue;
		}

		if(!item["p"][0].IsInt()) {

			errors.push_back("thing item 'p' must have an integer as its first value, skipping item");
			continue;
		}

		if(!item["p"][1].IsInt()) {

			errors.push_back("thing item 'p' must have an integer as its second value, skipping item");
			continue;
		}

		if(!item.HasMember("a")) {

			errors.push_back("thing item has no 'a' property, skipping item");
			continue;
		}

		if(!item["a"].IsObject()) {

			errors.push_back("thing item 'a' is not an object, skipping item");
			continue;
		}

		tile_editor::property_manager pm;

		parse_attributes(item["a"], pm);

		tile_editor::thing thing{
			item["p"][0].GetInt(),
			item["p"][1].GetInt(),
			//Width and height values are set by default and will be reviewed later when the blueprints are loaded.
			1,
			1,
			item["t"].GetInt(),
			{128,128,128,128}, //default color too.
			pm
		};

		layer.data.push_back(thing);

		if(item.Size() > 3) {

			errors.push_back("thing layer item has extraneous members that will be skipped");
		}
	}

	if(_node.Size() > 2) {

		errors.push_back("thing layer node has extraneous members that will be skipped");
	}

	//Add the layer to the map.
	_map.thing_layers.push_back(std::move(layer));
}

void map_parser::parse_polys(const jsondoc& _doc, map& _map) {

	if(!_doc.HasMember("polys")) {

		errors.push_back("no 'polys' node found, polys will be skipped");
		return;
	}

	if(!_doc["polys"].IsArray()) {

		errors.push_back("'polys' node must be an array, polys will be skipped");
		return;
	}

	for(const auto& item : _doc["polys"].GetArray()) {

		parse_poly_layer(item, _map);
	}

}

void map_parser::parse_poly_layer(const jsonval& _node, map& _map) {

	if(!_node.IsObject()) {

		errors.push_back("poly layer node must be an object, skipping layer");
		return;
	}

	auto metadata=parse_meta_node(_node);

	if(!check_data_node(_node, "poly")) {

		return;
	}

	poly_layer layer{metadata.set, metadata.alpha, {}};

	for(const auto& item : _node["data"].GetArray()) {

		if(!item.IsObject()) {
			errors.push_back("poly item is not an object, skipping poly");
			continue;
		}

		if(!item.HasMember("t")) {

			errors.push_back("poly item has no 't' property, skipping poly");
			continue;
		}

		if(!item["t"].IsInt()) {

			errors.push_back("poly item 't' is not an integer, skipping poly");
			continue;
		}

		if(!item.HasMember("p")) {

			errors.push_back("poly item has no 'p' property, skipping poly");
			continue;
		}

		if(!item["p"].IsArray()) {

			errors.push_back("poly item 'p' is not an array, skipping poly");
			continue;
		}

		if(3 > item["p"].Size()) {

			errors.push_back("poly item 'p' must have at least 3 vertices represented by three arrays, skipping poly");
			continue;
		}

		//BEGIN TODO.
		std::vector<tile_editor::poly_point> points;
		bool must_skip=false;
		for(const auto& point : item["p"].GetArray()) {

			if(!point.IsArray()) {

				errors.push_back("poly item point must be an array, skipping poly");
				must_skip=true;
				break;
			}

			if(2 != point.Size()) {

				errors.push_back("poly item point must have exactly two items, skipping poly");
				must_skip=true;
				break;
			}

			if(!point[0].IsInt()) {

				errors.push_back("poly item point first component must be an integer, skipping poly");
				must_skip=true;
				break;
			}

			if(!point[1].IsInt()) {

				errors.push_back("poly item point second component must be an integer, skipping poly");
				must_skip=true;
				break;
			}

			points.push_back({point[0].GetInt(), point[1].GetInt()});
		}

		if(must_skip) {
			continue;
		}

		if(!item.HasMember("a")) {

			errors.push_back("poly item has no 'a' property, skipping poly");
			continue;
		}

		if(!item["a"].IsObject()) {

			errors.push_back("poly item 'a' is not an object, skipping poly");
			continue;
		}

		tile_editor::property_manager pm;
		parse_attributes(item["a"], pm);

		tile_editor::poly poly{
			points,
			item["t"].GetInt(),
			pm
		};

		layer.data.push_back(poly);

		if(item.Size() > 3) {

			errors.push_back("poly layer item has extraneous members that will be skipped");
		}
	}

	if(_node.Size() > 2) {

		errors.push_back("poly layer node has extraneous members that will be skipped");
	}

	//Add the layer to the map.
	_map.poly_layers.push_back(std::move(layer));
}

map_parser::meta map_parser::parse_meta_node(const jsonval& _layer) {

	if(!_layer.HasMember("meta")) {

		errors.push_back("missing meta node in layer, skipping layer meta");
		return {0,0};
	}

	if(!_layer["meta"].IsObject()) {

		errors.push_back("meta node in layer must be an object, skipping layer meta");
		return {0,0};
	}

	meta result{0,0};

	auto can_be_extracted=[this, &_layer](const std::string& _key) -> bool {

		if(!_layer["meta"].HasMember(_key.c_str())) {

			errors.push_back(std::string{"meta node in layer has no '"}+_key+"' member, a default set will be used");
			return false;
		}

		if(!_layer["meta"][_key.c_str()].IsInt()) {

			errors.push_back(std::string{"meta:"}+_key+" node is not an integer, a default set will be used");
			return false;
		}

		return true;
	};

	if(can_be_extracted("alpha")) {

		result.alpha=_layer["meta"]["alpha"].GetInt();
	}

	if(can_be_extracted("set")) {

		result.set=_layer["meta"]["set"].GetInt();
	}

	if(_layer["meta"].Size() > 2) {

		errors.push_back("meta node in layer has extraneous members which will be ignored");
	}

	return result;
}

bool map_parser::check_data_node(const jsonval& _node, const std::string& _type) {

	if(!_node.HasMember("data")) {

		errors.push_back(std::string{"missing data in "}+_type+" layer, skipping layer");
		return false;
	}

	if(!_node["data"].IsArray()) {

		errors.push_back(std::string{"data in "}+_type+" layer is not an array, skipping layer");
		return false;
	}

	return true;
}


void map_parser::parse_attributes(const jsonval& _item, tile_editor::property_manager& _pm) {

	for(const auto& member : _item.GetObject()) {

		const std::string propname{member.name.GetString()};
		if(_pm.has_property(propname)) {
			errors.push_back(std::string{"'"}+propname+"' already exists as attribute, skipping property");
			continue;
		}

		if(member.value.IsInt()) {

			_pm.int_properties[propname]=member.value.GetInt();
		}
		else if(member.value.IsDouble()) {

			_pm.double_properties[propname]=member.value.GetDouble();
		}
		else if(member.value.IsString()) {

			_pm.string_properties[propname]=member.value.GetString();
		}
		else {

			errors.push_back(std::string{"invalid data type in attribute, skipping property '"}+propname+"'");
		}
	}
}
