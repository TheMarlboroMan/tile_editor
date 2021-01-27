#pragma once

#include "thing_definition_table.h"
#include "poly_definition_table.h"
#include "tileset.h"
#include "property_table.h"
#include "color.h"
#include "grid_data.h"

#include <map>

namespace tile_editor {

//!The different blueprints for a map session.
struct map_blueprint {

	enum class thing_centers {
		center,
		top_left,
		top_right,
		bottom_right,
		bottom_left
	};

	int                                             toolbox_width_percent=33;
	tile_editor::color                              bg_color{32,32,32,0},
	                                                font_color{255,255,255,0};
	thing_centers                                   thing_center{thing_centers::center};

	std::map<std::size_t, tile_editor::grid_data>   gridsets;
	std::map<std::size_t, tileset>                  tilesets;
	std::map<std::size_t, thing_definition_table>   thingsets;
	std::map<std::size_t, poly_definition_table>    polysets;
	property_table                                  properties;
};

}
