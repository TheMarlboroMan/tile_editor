#pragma once

#include "editor_types/map.h"

#include <rapidjson/document.h>

#include <string>
#include <vector>

namespace tile_editor {

class map_serializer {

	public:

	using errvector=std::vector<std::string>;

	void                    to_file(const tile_editor::map&, const std::string&, const std::string&);
	std::string             to_string(const tile_editor::map&, const std::string&);

	const errvector&        get_errors() const {return errors;}

	private:

	using jsondoc=rapidjson::Document;
	using jsonval=rapidjson::Value;

	void                    put_meta(jsondoc&, const std::string&);
	void                    put_attributes(jsondoc&, const tile_editor::map&);
	void                    put_layers(jsondoc&, const tile_editor::map&);
	void                    put_tile_layer(jsondoc&, jsonval&, const tile_editor::tile_layer&);
	void                    put_thing_layer(jsondoc&, jsonval&, const tile_editor::thing_layer&);
	void                    put_poly_layer(jsondoc&, jsonval&, const tile_editor::poly_layer&);

	errvector               errors;
};

}
