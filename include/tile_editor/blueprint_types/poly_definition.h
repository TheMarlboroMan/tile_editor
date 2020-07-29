#pragma once

#include "property_table.h"
#include "color.h"

#include <string>

namespace tile_editor {

struct poly_definition {

	enum class color_types {fixed, customizable};

	std::size_t                     poly_id;
	std::string                     name;
	tile_editor::color              color;
	color_types                     color_type;
	property_table                  properties;
};

}
