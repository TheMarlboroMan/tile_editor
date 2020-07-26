#pragma once

#include "thing_definition_table.h"
#include "property_table.h"
#include "tileset.h"

#include <map>

namespace tile_editor {

//!The different blueprints for a map.
struct map_blueprint {

	std::map<std::size_t, tileset>                  tilesets;
	std::map<std::size_t, thing_definition_table>   thingsets;
	property_table                                  properties;
};

}
