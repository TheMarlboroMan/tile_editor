#pragma once

#include "property_table.h"

#include <string>

namespace tile_editor {

//!A blueprint for a thing, contains the properties that it might have, but
//!not its values.
struct thing_definition {

	enum class size_type {fixed, resizable};
	struct color{int r, g, b;};

	std::size_t                     type_id;
	int                             w, h;
	size_type                       sizetype;
	std::string                     name;
	color                           color;
	property_table                  properties;
};

}
