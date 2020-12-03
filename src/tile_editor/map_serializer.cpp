#include "serializer/map_serializer.h"

#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <tools/json.h>
#include <fstream>

using namespace tile_editor;

void map_serializer::to_file(
	const tile_editor::map& _map,
	const std::string& _version,
	const std::string& _filename
) {

	std::ofstream file{_filename};
	file<<this->to_string(_map, _version);
}

std::string map_serializer::to_string(
	const tile_editor::map& _map,
	const std::string& _version
) {

	jsondoc doc{rapidjson::kObjectType};

	put_meta(doc, _version);
	put_attributes(doc, _map);
	put_tiles(doc, _map);
	put_things(doc, _map);
	put_polys(doc, _map);

	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	doc.Accept(writer);
	return buffer.GetString();
}

void map_serializer::put_meta(
	jsondoc& _doc,
	const std::string& _version
) {

	jsondoc meta{rapidjson::kObjectType};

	auto& allocator=_doc.GetAllocator();
	meta.AddMember(
		"version",
		tools::json_string(_version, allocator),
		allocator
	);

	_doc.AddMember("meta", meta, allocator);
}

void map_serializer::put_attributes(
	jsondoc& _doc,
	const tile_editor::map& _map
) {
	jsondoc attributes{rapidjson::kObjectType};
	auto& allocator=_doc.GetAllocator();

	const auto pm=_map.properties;
	for(const auto& pair : pm.int_properties) {

		attributes.AddMember(
			jsonval{pair.first.c_str(), allocator},
			jsonval{pair.second},
			allocator
		);
	}

	for(const auto& pair : pm.double_properties) {

		attributes.AddMember(
			jsonval{pair.first.c_str(), allocator},
			jsonval{pair.second},
			allocator
		);
	}

	for(const auto& pair : pm.string_properties) {

		attributes.AddMember(
			jsonval{pair.first.c_str(), allocator},
			tools::json_string(pair.second, allocator),
			allocator
		);
	}

	_doc.AddMember("attributes", attributes, allocator);
}

void map_serializer::put_tiles(
	jsondoc& _doc,
	const tile_editor::map& _map
) {
	jsonval tiles{rapidjson::kArrayType};
	for(const auto& layer : _map.tile_layers) {

		put_tile_layer(_doc, tiles, layer);
	}

	_doc.AddMember("tiles", tiles, _doc.GetAllocator());
}

void map_serializer::put_tile_layer(
	jsondoc& _doc,
	jsonval& _container,
	const tile_editor::tile_layer& _layer
) {

	auto& allocator=_doc.GetAllocator();

	//Add meta...
	jsonval meta{rapidjson::kObjectType};
	meta.AddMember("set", _layer.set, allocator);
	meta.AddMember("alpha", _layer.alpha, allocator);

	//Add data...
	jsonval data{rapidjson::kArrayType};
	for(const auto& entry : _layer.data) {

		jsonval item{rapidjson::kObjectType};
		item.AddMember("t", entry.type, allocator);

		jsonval pos{rapidjson::kArrayType};
		pos.PushBack(entry.x, allocator);
		pos.PushBack(entry.y, allocator);
		item.AddMember("p", pos, allocator);

		data.PushBack(item, allocator);
	}

	jsonval layer{rapidjson::kObjectType};
	layer.AddMember("meta", meta, allocator);
	layer.AddMember("data", data, allocator);
	_container.GetArray().PushBack(layer, allocator);
}


void map_serializer::put_things(
	jsondoc& _doc,
	const tile_editor::map& _map
) {
	jsonval things{rapidjson::kArrayType};
	for(const auto& layer : _map.thing_layers) {

		put_thing_layer(_doc, things, layer);
	}

	_doc.AddMember("things", things, _doc.GetAllocator());
}

void map_serializer::put_thing_layer(
	jsondoc& _doc,
	jsonval& _container,
	const tile_editor::thing_layer& _layer
) {

	auto& allocator=_doc.GetAllocator();

	//Add meta...
	jsonval meta{rapidjson::kObjectType};
	meta.AddMember("set", _layer.set, allocator);
	meta.AddMember("alpha", _layer.alpha, allocator);

	//Add data...
	jsonval data{rapidjson::kArrayType};
	for(const auto& entry : _layer.data) {

		jsonval item{rapidjson::kObjectType};
		item.AddMember("t", entry.type, allocator);

		jsonval pos{rapidjson::kArrayType};
		pos.PushBack(entry.x, allocator);
		pos.PushBack(entry.y, allocator);
		item.AddMember("p", pos, allocator);

//TODO: Lots of repeated code, room for improvement...

		jsonval attr{rapidjson::kObjectType};
		const auto pm=entry.properties;
		for(const auto& pair : pm.int_properties) {

			attr.AddMember(
				jsonval{pair.first.c_str(), allocator},
				jsonval{pair.second},
				allocator
			);
		}

		for(const auto& pair : pm.double_properties) {

			attr.AddMember(
				jsonval{pair.first.c_str(), allocator},
				jsonval{pair.second},
				allocator
			);
		}

		for(const auto& pair : pm.string_properties) {

			attr.AddMember(
				jsonval{pair.first.c_str(), allocator},
				tools::json_string(pair.second, allocator),
				allocator
			);
		}

		item.AddMember("a", attr, allocator);
		data.PushBack(item, allocator);
	}

	jsonval layer{rapidjson::kObjectType};
	layer.AddMember("meta", meta, allocator);
	layer.AddMember("data", data, allocator);
	_container.GetArray().PushBack(layer, allocator);
}


void map_serializer::put_polys(
	jsondoc& _doc,
	const tile_editor::map& _map
) {
	jsonval polys{rapidjson::kArrayType};
	for(const auto& layer : _map.poly_layers) {

		put_poly_layer(_doc, polys, layer);
	}

	_doc.AddMember("polys", polys, _doc.GetAllocator());
}

void map_serializer::put_poly_layer(
	jsondoc& _doc,
	jsonval& _container,
	const tile_editor::poly_layer& _layer
) {

	auto& allocator=_doc.GetAllocator();

	//Add meta...
	jsonval meta{rapidjson::kObjectType};
	meta.AddMember("set", _layer.set, allocator);
	meta.AddMember("alpha", _layer.alpha, allocator);
	//Add data...
	jsonval data{rapidjson::kArrayType};
	for(const auto& entry : _layer.data) {

		jsonval item{rapidjson::kObjectType};
		item.AddMember("t", entry.type, allocator);

		jsonval pos{rapidjson::kArrayType};
		for(const auto& pt : entry.points) {

			jsonval point{rapidjson::kArrayType};
			point.PushBack(pt.x, allocator);
			point.PushBack(pt.y, allocator);
			pos.PushBack(point, allocator);
		}

		item.AddMember("p", pos, allocator);

//TODO: Lots of repeated code, room for improvement...

		jsonval attr{rapidjson::kObjectType};
		const auto pm=entry.properties;
		for(const auto& pair : pm.int_properties) {

			attr.AddMember(
				jsonval{pair.first.c_str(), allocator},
				jsonval{pair.second},
				allocator
			);
		}

		for(const auto& pair : pm.double_properties) {

			attr.AddMember(
				jsonval{pair.first.c_str(), allocator},
				jsonval{pair.second},
				allocator
			);
		}

		for(const auto& pair : pm.string_properties) {

			attr.AddMember(
				jsonval{pair.first.c_str(), allocator},
				tools::json_string(pair.second, allocator),
				allocator
			);
		}

		item.AddMember("a", attr, allocator);
		data.PushBack(item, allocator);
	}
	jsonval layer{rapidjson::kObjectType};
	layer.AddMember("meta", meta, allocator);
	layer.AddMember("data", data, allocator);
	_container.GetArray().PushBack(layer, allocator);
}
