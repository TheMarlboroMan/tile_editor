#pragma once

#include "thing_definition_table.h"
#include "poly_definition_table.h"
#include "property_table.h"
#include "tileset.h"

#include <map>

namespace tile_editor {

//!The different blueprints for a map.
struct map_blueprint {

	std::map<std::size_t, tileset>                  tilesets;
	std::map<std::size_t, thing_definition_table>   thingsets;
	std::map<std::size_t, poly_definition_table>    polysets;
	property_table                                  properties;

	//TODO:
	/*
	bd color
	grid color
	grid ruler color
	vertical grid size
	horizontal grid size
	vertical grid rule each
	horizontal grid rule each
	thing center type
	*/
};

}
