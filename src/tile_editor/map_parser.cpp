#include "parser/map_parser.h"

#include <tools/json.h>
#include <tools/file_utils.h>
#include <stdexcept>

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
		parse_layers(doc, result);
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

void map_parser::parse_layers(
	const jsondoc& _doc, 
	map& _map
) {

	if(!_doc.HasMember("layers")) {

		errors.push_back("no 'layers' node found, layers will be skipped");
		return;
	}

	if(!_doc["layers"].IsArray()) {

		errors.push_back("'layers' node must be an array, layers will be skipped");
		return;
	}

	for(const auto& node : _doc["layers"].GetArray()) {

		if(!node.IsObject()) {

			errors.push_back("layer must be an object, cannot locate meta, skipping layer meta");
			continue;
		}

		auto metadata=parse_meta_node(node);

		switch(metadata.type) {
			case meta::types::bad:
				continue;
			case meta::types::tiles:
				parse_tile_layer(node, metadata, _map);
			break;

			case meta::types::things:
				parse_thing_layer(node, metadata, _map);
			break;

			case meta::types::polys:
				parse_poly_layer(node, metadata, _map);
			break;
		}
	}
}

void map_parser::parse_tile_layer(
	const jsonval& _node, 
	const meta& _meta, 
	map& _map
) {

	if(!_node.IsObject()) {

		errors.push_back("tile layer node must be an object, skipping layer");
		return;
	}

	if(!check_data_node(_node, "tile")) {

		return;
	}

	//create the layer.
	tile_layer layer{_meta.set, _meta.alpha, _meta.id, {}};

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
			(std::size_t)item["t"].GetInt()
		};

		layer.data.push_back(tile);

		if(item.MemberCount() > 2) {

			errors.push_back("tile layer item has extraneous members that will be skipped");
		}
	}

	if(_node.MemberCount() > 2) {

		errors.push_back("tile layer node has extraneous members that will be skipped");
	}

	//Add the layer to the map.
	_map.layers.emplace_back(
		map::layerptr{new tile_layer{std::move(layer)}}
	);
}

void map_parser::parse_thing_layer(
	const jsonval& _node, 
	const meta& _meta,
	map& _map
) {

	if(!_node.IsObject()) {

		errors.push_back("thing layer node must be an object, skipping layer");
		return;
	}

	if(!check_data_node(_node, "thing")) {

		return;
	}

	thing_layer layer{_meta.set, _meta.alpha, _meta.id, {}};

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
			//TODO: Is this ever done???
			1,
			1,
			(std::size_t)item["t"].GetInt(),
			{128,128,128,128}, //default color too.
			pm
		};

		layer.data.push_back(thing);

		if(item.MemberCount() > 3) {

			errors.push_back("thing layer item has extraneous members that will be skipped");
		}
	}

	if(_node.MemberCount() > 2) {

		errors.push_back("thing layer node has extraneous members that will be skipped");
	}

	_map.layers.emplace_back(
		map::layerptr{new thing_layer{std::move(layer)}}
	);
}

void map_parser::parse_poly_layer(
	const jsonval& _node, 
	const meta& _meta,
	map& _map
) {

	if(!_node.IsObject()) {

		errors.push_back("poly layer node must be an object, skipping layer");
		return;
	}

	if(!check_data_node(_node, "poly")) {

		return;
	}

	poly_layer layer{_meta.set, _meta.alpha, _meta.id, {}};

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
			(std::size_t)item["t"].GetInt(),
			pm
		};

		layer.data.push_back(poly);

		if(item.MemberCount() > 3) {

			errors.push_back("poly layer item has extraneous members that will be skipped");
		}
	}

	if(_node.MemberCount() > 2) {

		errors.push_back("poly layer node has extraneous members that will be skipped");
	}

	_map.layers.emplace_back(
		map::layerptr{new poly_layer{std::move(layer)}}
	);
}

map_parser::meta map_parser::parse_meta_node(const jsonval& _layer) {

	if(!_layer.HasMember("meta")) {

		errors.push_back("missing meta node in layer, skipping layer meta");
		return {0,0, generate_default_id(), meta::types::bad};
	}

	if(!_layer["meta"].IsObject()) {

		errors.push_back("meta node in layer must be an object, skipping layer meta");
		return {0,0, generate_default_id(), meta::types::bad};
	}

	auto node_exists=[this, &_layer](const std::string& _key) -> bool {

		if(!_layer["meta"].HasMember(_key.c_str())) {

			errors.push_back(std::string{"meta node in layer has no '"}+_key+"' member, a default will be used");
			return false;
		}

		return true;
	};

	auto int_can_be_extracted=[this, &_layer, node_exists](const std::string& _key) -> bool {

		if(!node_exists(_key)) {

			return false;
		}

		if(!_layer["meta"][_key.c_str()].IsInt()) {

			errors.push_back(std::string{"meta:"}+_key+" node is not an integer, a default will be used");
			return false;
		}

		return true;
	};

	auto string_can_be_extracted=[this, &_layer, node_exists](const std::string& _key) -> bool {

		if(!node_exists(_key)) {

			return false;
		}

		if(!_layer["meta"][_key.c_str()].IsString()) {

			errors.push_back(std::string{"meta:"}+_key+" node is not a string, a default will be used");
			return false;
		}

		std::string val{_layer["meta"][_key.c_str()].GetString()};
		if(!val.size()) {

			errors.push_back(std::string{"meta:"}+_key+" is an empty string, a default will be used");
			return false;
		}

		return true;
	};

	if(!string_can_be_extracted("type")) {

		errors.push_back("meta node does not contain type: cannot be parsed");
		return {0,0, "", meta::types::bad};
	}

	std::string strtype{_layer["meta"]["type"].GetString()};
	meta::types type{meta::types::bad};

	if(strtype=="tiles") {

		type=meta::types::tiles;
	}
	else if(strtype=="things") {

		type=meta::types::things;
	}
	else if(strtype=="polys") {

		type=meta::types::polys;
	}
	else {

		errors.push_back(std::string{"unkown meta node type '"}+strtype+"', cannot be parsed");
		return {0,0, "", meta::types::bad};
	}

	meta result{0,0, "", type};
	if(int_can_be_extracted("alpha")) {

		result.alpha=_layer["meta"]["alpha"].GetInt();
	}

	if(int_can_be_extracted("set")) {

		result.set=_layer["meta"]["set"].GetInt();
	}

	result.id=string_can_be_extracted("id")
		? _layer["meta"]["id"].GetString()
		: generate_default_id();

	if(_layer["meta"].MemberCount() > 4) {

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

std::string map_parser::generate_default_id() {

	std::string result{"default_id_"};
	return result+std::to_string(++default_id_count);
}
