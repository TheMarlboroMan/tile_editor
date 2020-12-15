#pragma once

#include "layer.h"
#include "poly.h"

#include <vector>

namespace tile_editor {

//!This is an "in-editor" layer of polygons.
struct poly_layer:
	public layer {

	using container=std::vector<poly>;
	enum class windings {clockwise, counterclockwise, any};
	enum class curves {concave, convex, any};

	                   poly_layer(
		std::size_t _set,
		int _alpha,
		const std::string& _name,
		windings _winding,
		curves _curve,
		const container& _container
	):layer{_set, _alpha, _name},
		winding{_winding},
		curve{_curve},
		data{_container} {

	}

	windings            winding;
	curves              curve;
	container           data;
	void                accept(layer_visitor& _visitor) {_visitor.visit(*this);}
	void                accept(const_layer_visitor& _visitor) const {_visitor.visit(*this);}
};

}
