#include "tile_editor/app/env.h"

#include <stdlib.h>

using namespace tile_editor;

std::string env::get_app_path() const {

	return std::string{getenv("HOME")}+"/.tile_editor/";
}
