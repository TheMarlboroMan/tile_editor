#pragma once

#include "editor_types/map.h"

#include <lm/logger.h>

namespace tile_editor {

class map_saver {

	public:

	                        map_saver(lm::logger&);
	bool                    save(const tile_editor::map&, const std::string&);

	private:

	lm::logger&             log;
};
}
