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

	for(const auto& member : _doc["attributes"].GetObject()) {

		const std::string propname{member.name.GetString()};
		if(_map.properties.has_property(propname)) {
			errors.push_back(std::string{"'"}+propname+"' already exists as attribute, skipping property");
			continue;
		}

		if(member.value.IsInt()) {

			_map.properties.int_properties[propname]=member.value.GetInt();
		}
		else if(member.value.IsDouble()) {

			_map.properties.double_properties[propname]=member.value.GetDouble();
		}
		else if(member.value.IsString()) {

			_map.properties.string_properties[propname]=member.value.GetString();
		}
		else {

			errors.push_back(std::string{"invalid data type in 'attributes', skipping property '"}+propname+"'");
		}
	}
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

	auto meta=parse_meta_node(_node);

	if(!_node.HasMember("data")) {

		//TODO:
	}

	if(!_node.Size() > 2) {

		errors.push_back("tile layer node has extraneous members that will be skipped");
	}
}

map_parser::meta map_parser::parse_meta_node(const jsonval& c) {

	if(!_layer.HasMember("meta")) {

		errors.push_back("missing meta node in layer, skipping layer meta");
		return {0,0};
	}

	if(!_layer["meta"].IsObject()) {

		errors.push_back("meta node in layer must be an object, skipping layer meta");
		return {0,0};
	}

	meta result{0,0};

	auto can_be_extracted=[_layer&, this](const std::string& _key) -> bool {

		if(!_layer["meta"].HasMember(_key)) {

			errors.push_back(std::string{"meta node in layer has no '"}+_key+"' member, a default set will be used");
			return false;
		}

		if(!_layer["meta"][_key].IsInt()) {

			errors.push_back(std::string{"meta:"}+_key+" node is not an integer, a default set will be used");
			return false;
		}

		return true;
	};

	if(can_be_extracted("set")) {

		result.set=_layer["meta"]["set"].GetInt();
	}

	if(can_be_extracted("alpha")) {

		result.set=_layer["meta"]["alpha"].GetInt();
	}

	if(_layer.size > 2) {
	
		errors.push_back("meta node in layer has extraneous members which will be ignored");
	}

	return result;
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

		//parse_thing_layer(item, _map);
	}
}

void map_parser::parse_thing_layer(const jsondoc& _doc, map& _map) {

}

void map_parser::parse_polys(const jsondoc& _doc, map&_map) {

	if(!_doc.HasMember("polys")) {

		errors.push_back("no 'polys' node found, polys will be skipped");
		return;
	}

	if(!_doc["polys"].IsArray()) {

		errors.push_back("'polys' node must be an array, polys will be skipped");
		return;
	}

	for(const auto& item : _doc["polys"].GetArray()) {

		//parse_poly_layer(item, _map);
	}

}

void map_parser::parse_poly_layer(const jsondoc& _doc, map&_map) {

}
