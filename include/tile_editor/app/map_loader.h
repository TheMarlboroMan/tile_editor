#pragma once

#include "editor_types/map.h"
#include "blueprint_types/thing_definition_table.h"
#include "blueprint_types/poly_definition_table.h"
#include "blueprint_types/tileset.h"
#include "tools/message_manager.h"

#include <lm/logger.h>
#include <map>

namespace tile_editor {

class map_loader {

	public:

					        map_loader(
					            lm::logger&,
					            tools::message_manager&,
					            const std::map<std::size_t, tile_editor::tileset>&,
					            const std::map<std::size_t, tile_editor::thing_definition_table>&,
					            const std::map<std::size_t, tile_editor::poly_definition_table>&,
					            const tile_editor::property_table&);
	tile_editor::map        load_from_file(const std::string&);

	private:

	void                    inflate_properties(tile_editor::map&);
	void                    fix_invalid_indexes(tile_editor::map&);

	lm::logger&             log;
	tools::message_manager& message_manager;
	const std::map<std::size_t, tile_editor::tileset>& tilesets;
	const std::map<std::size_t, tile_editor::thing_definition_table>& thingsets;
	const std::map<std::size_t, tile_editor::poly_definition_table>& polysets;
	const tile_editor::property_table&   map_property_blueprints;

};

}
