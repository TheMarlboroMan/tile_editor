#include "tile_editor/editor_types/layer.h"
#include "tile_editor/blueprint_types/thing_definition_table.h"
#include "tile_editor/blueprint_types/poly_definition_table.h"
#include "tile_editor/blueprint_types/tileset.h"

#include <tools/grid_list.h>
#include <tools/vertical_list.h>
#include <map>

namespace tile_editor {

//!This class will load a vertical/grid list of tiles/things/poly blueprints
//!from a given layer.
struct set_layer_loader:
	const_layer_visitor {

	set_layer_loader(
		tools::grid_list<ldtools::sprite_table::container::value_type>&,
		tools::vertical_list<tile_editor::thing_definition>&,
		tools::vertical_list<tile_editor::poly_definition>&,
		std::map<std::size_t, tileset>&,
		std::map<std::size_t, thing_definition_table>&,
		std::map<std::size_t, poly_definition_table>&
	);

	tools::grid_list<ldtools::sprite_table::container::value_type>& tile_list;
	tools::vertical_list<tile_editor::thing_definition>& thing_list;
	tools::vertical_list<tile_editor::poly_definition>& poly_list;
	std::map<std::size_t, tileset>&                 tilesets;
	std::map<std::size_t, thing_definition_table>&  thingsets;
	std::map<std::size_t, poly_definition_table>&   polysets;

	void visit(const tile_editor::tile_layer&);
	void visit(const tile_editor::thing_layer&);
	void visit(const tile_editor::poly_layer&);

	template<typename T, typename L, typename S>
	void insert(
		const L& _layer,
		T& _list,
		const S& _sets
	) {

		_list.clear();
		for(const auto& item : _sets.at(_layer.set).table) {

			_list.insert(item.second);
		}
	}

};

}
