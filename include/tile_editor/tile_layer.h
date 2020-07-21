#pragma once

#include "tile.h"

#include <vector>
#include <cstdint>

namespace tile_editor {

//!This is an "in-editor" layer of tiles.
class tile_layer {

	public:

	using               container=std::vector<tile>;
	struct layer_size {
		int             w,
		                h;
	};

	struct grid_separator {

		int             w,
		                h;
	};

	struct      tile_size {

		int             w,
		                h;
	};

	container&          get_tiles() {return tiles;}
	const container&    get_tiles() const {return tiles;}
	layer_size          get_layer_size() const {return layersize;}
	grid_separator      get_grid_separator() const {return separator;}
	tile_size           get_tile_size() const {return tilesize;}
	std::size_t         get_tileset_index() const {return tileset_index;}

	private:

	layer_size          layersize;
	grid_separator      separator;
	tile_size           tilesize;
	std::size_t         tileset_index;
	uint8_t             alpha;
	container           tiles;
};

}