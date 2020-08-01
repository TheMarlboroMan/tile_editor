#pragma once

#include "tile_layer.h"
#include "thing_layer.h"
#include "poly_layer.h"
#include "../editor_types/property_manager.h"

#include <vector>

namespace tile_editor {

//!An in-editor map, the container for tiles and things, plus all of its
//!properties.
struct map {

	std::vector<tile_layer>         tile_layers;
	std::vector<thing_layer>        thing_layers;
	std::vector<poly_layer>         poly_layers;
	property_manager                properties;

};

}
