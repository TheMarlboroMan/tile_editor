#pragma once

#include "../editor_types/property_manager.h"
#include "../blueprint_types/color.h"
#include <ldt/point_2d.h>
#include <vector>

namespace tile_editor {

//!This is an in-editor poly. Points are expressed as integers.
//!Polygons are winded clockwise.

using poly_point=ldt::point_2d<int>;

struct poly {

	public:

	std::vector<poly_point>             points;
	std::size_t                         type;
	tile_editor::color                  color;
	property_manager                    properties;
};

}
