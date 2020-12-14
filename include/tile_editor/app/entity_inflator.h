#pragma once

namespace tile_editor {

	struct thing;
	struct poly;
	struct thing_definition;
	struct poly_definition;
}

namespace app {

class entity_inflator {

	public:

	void            inflate(tile_editor::thing&, const tile_editor::thing_definition&);
	void            inflate(tile_editor::poly&, const tile_editor::poly_definition&);
};

}

