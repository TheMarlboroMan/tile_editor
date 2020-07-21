#pragma once

#include "thing_definition_table.h"
#include "property_table.h"

#include <ldtools/sprite_table.h>

#include <map>

namespace tile_editor {

//!The different blueprints for a map.
class map_blueprint {

	//TODO: Nope, missing the background image file!!!!!
	std::map<std::size_t, ldtools::sprite_table>    tilesets;
	std::map<std::size_t, thing_definition_table>   thingsets;
	property_table                                  properties;
};

}