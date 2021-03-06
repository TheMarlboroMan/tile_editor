#pragma once

#include "tile.h"
#include "layer.h"

#include <vector>

namespace tile_editor {

struct tile_layer:
	public layer {

	using container=std::vector<tile>;

	                   tile_layer(
		std::size_t _set,
		std::size_t _gridset,
		int _alpha,
		const std::string& _name,
		const container& _container
	):layer{_set, _gridset, _alpha, _name}, data{_container} {

	}

	container           data;
	void               accept(layer_visitor& _visitor) {_visitor.visit(*this);}
	void               accept(const_layer_visitor& _visitor) const {_visitor.visit(*this);}
};

}
