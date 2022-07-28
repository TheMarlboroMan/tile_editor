#pragma once

#include <string>

namespace tile_editor {

struct default_layer {

	std::string                         name;
	enum class types{tile, thing, poly} type;
	std::size_t                         set_id,
	                                    grid_id;
	int                                 alpha;
};

}
