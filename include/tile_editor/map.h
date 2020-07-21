#pragma once

#include "tile_layer.h"
#include "thing_layer.h"
#include "property_manager.h"

#include <vector>

namespace tile_editor {

//!An in-editor map, the container for tiles and things, plus all of its
//!properties.
class map {

	private:

	std::vector<tile_layer>         tile_layers;
	std::vector<thing_layer>        thing_layers;
	property_manager                properties;

};

}