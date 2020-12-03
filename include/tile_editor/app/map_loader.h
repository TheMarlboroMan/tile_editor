#pragma once

#include "editor_types/map.h"
#include "tools/message_manager.h"

#include <lm/logger.h>

namespace tile_editor {

class map_loader {

	public:

					        map_loader(lm::logger&, tools::message_manager&);
	tile_editor::map        load_from_file(const std::string&);

	private:

	lm::logger&             log;
	tools::message_manager& message_manager;

};

}
