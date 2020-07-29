#pragma once

#include "property_table.h"
#include "color.h"

#include <string>

namespace tile_editor {

//!A blueprint for a thing, contains the properties that it might have, but
//!not its values.
struct thing_definition {

	enum class size_types {fixed, resizable};

	std::size_t                     type_id;
	int                             w, h;
	size_types                      size_type;
	std::string                     name;
	tile_editor::color              color;
	property_table                  properties;
};

}
