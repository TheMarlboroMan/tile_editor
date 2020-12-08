#pragma once

#include "thing_definition_table.h"
#include "poly_definition_table.h"
#include "tileset.h"
#include "property_table.h"
#include "color.h"

#include <map>

namespace tile_editor {

//!Grid data for a session.
struct grid_data {

	int                 size=32,
	                    vertical_ruler=8,
	                    horizontal_ruler=8;
	tile_editor::color  color={0,0,0,255},
	                    ruler_color={0,255,0,255},
	                    origin_color={255,255,255,255};
};

//!The different blueprints for a map session.
struct map_blueprint {

	enum class thing_centers {
		center,
		top_left,
		top_right,
		bottom_right,
		bottom_left
	};

	tile_editor::grid_data                          grid_data;
	tile_editor::color                              bg_color{32,32,32,0};
	thing_centers                                   thing_center{thing_centers::center};

	std::map<std::size_t, tileset>                  tilesets;
	std::map<std::size_t, thing_definition_table>   thingsets;
	std::map<std::size_t, poly_definition_table>    polysets;
	property_table                                  properties;
};

}
