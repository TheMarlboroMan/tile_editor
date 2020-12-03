#pragma once

#include "editor_types/map.h"
#include "tools/message_manager.h"

#include <lm/logger.h>

namespace tile_editor {

class map_saver {

	public:

	                        map_saver(lm::logger&, tools::message_manager&);
	void                    save(const tile_editor::map&, const std::string&);

	private:

	lm::logger&             log;
	tools::message_manager& message_manager;
};
}
