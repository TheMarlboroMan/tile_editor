#pragma once

#include "thing.h"

#include <vector>

namespace tile_editor {

//!This is an "in-editor" layer of things.
class thing_layer {

	public:

	using               container=std::vector<thing>;

	container&          get_things() {return things;}
	const container&    get_things() const {return things;}
	std::size_t         get_thingset_index() const {return thingset_index;}

	private:

	std::size_t         thingset_index;
	container           things;
};

}