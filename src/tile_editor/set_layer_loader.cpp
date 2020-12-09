#include "tile_editor/app/set_layer_loader.h"
#include "tile_editor/editor_types/tile_layer.h"
#include "tile_editor/editor_types/thing_layer.h"
#include "tile_editor/editor_types/poly_layer.h"

using namespace tile_editor;

set_layer_loader::set_layer_loader(
	tools::grid_list<ldtools::sprite_table::container::value_type>& _tilelist,
	tools::vertical_list<tile_editor::thing_definition>& _thinglist,
	tools::vertical_list<tile_editor::poly_definition>& _polylist,
	std::map<std::size_t, tileset>& _tilesets,
	std::map<std::size_t, thing_definition_table>& _thingsets,
	std::map<std::size_t, poly_definition_table>& _polysets
):
	tile_list{_tilelist},
	thing_list{_thinglist},
	poly_list{_polylist},
	tilesets{_tilesets},
	thingsets{_thingsets},
	polysets{_polysets} {

}

void set_layer_loader::visit(const tile_editor::tile_layer& _layer) {

	tile_list.clear();
	for(const auto& item : tilesets.at(_layer.set).table) {

		tile_list.insert(item);
	}
}

void set_layer_loader::visit(const tile_editor::thing_layer& _layer) {

	insert(_layer, thing_list, thingsets);
}

void set_layer_loader::visit(const tile_editor::poly_layer& _layer) {

	insert(_layer, poly_list, polysets);
}
