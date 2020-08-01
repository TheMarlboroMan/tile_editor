#pragma once

#include <cstddef>

namespace tile_editor {

//!This is an "in-editor" tile.
struct tile {

	int                 x,
	                    y;
	std::size_t         type;
};

}
