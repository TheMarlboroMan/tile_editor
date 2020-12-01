#pragma once

#include <string>
#include <ldtools/sprite_table.h>

namespace tile_editor {

struct tileset {

	ldtools::sprite_table   table;
	std::string             image_path;
	std::string             name;
};

}
