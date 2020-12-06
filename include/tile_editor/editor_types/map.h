#pragma once

#include "tile_layer.h"
#include "thing_layer.h"
#include "poly_layer.h"
#include "../editor_types/property_manager.h"

#include <vector>
#include <memory>

namespace tile_editor {

//!An in-editor map.
struct map {

	using layerptr=std::unique_ptr<layer>;
	std::vector<layerptr>           layers;
	property_manager                properties;
};

}
