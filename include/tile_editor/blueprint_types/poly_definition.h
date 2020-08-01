#pragma once

#include "property_table.h"
#include "color.h"

#include <string>

namespace tile_editor {

struct poly_definition {

	std::size_t                     poly_id;
	std::string                     name;
	tile_editor::color              color;
	property_table                  properties;
};

}
