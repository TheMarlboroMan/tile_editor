#pragma once

#include "editor_types/map.h"

#include <lm/logger.h>

namespace tile_editor {

class map_saver {

	public:

	                        map_saver(lm::logger&);

	//!Saves the given map. Its internals might change (for example, items
	//!will be reordered).
	bool                    save(tile_editor::map&, const std::string&);

	private:

	void                    pre_save(tile_editor::map&);
	void                    pre_save(tile_editor::tile_layer&);
	void                    pre_save(tile_editor::thing_layer&);
	void                    pre_save(tile_editor::poly_layer&);

	lm::logger&             log;
};
}
