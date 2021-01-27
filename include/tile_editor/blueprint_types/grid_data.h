
#pragma once

#include "color.h"

#include <string>

namespace tile_editor {

//!Grid data for a session.
struct grid_data {

	std::string         name{"default"};
	int                 size{32},
	                    vertical_ruler{8},
	                    horizontal_ruler{8};
	tile_editor::color  color={0,0,0,255},
	                    subcolor={0,0,0,128},
	                    ruler_color={0,255,0,255},
	                    origin_color={255,255,255,255};
};

}
