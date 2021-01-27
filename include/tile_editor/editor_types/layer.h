#pragma once

#include <cstdint>
#include <string>

namespace tile_editor {

struct tile_layer;
struct thing_layer;
struct poly_layer;

struct layer_visitor {

	virtual void visit(tile_editor::tile_layer&)=0;
	virtual void visit(tile_editor::thing_layer&)=0;
	virtual void visit(tile_editor::poly_layer&)=0;
};

struct const_layer_visitor {

	virtual void visit(const tile_editor::tile_layer&)=0;
	virtual void visit(const tile_editor::thing_layer&)=0;
	virtual void visit(const tile_editor::poly_layer&)=0;
};

//!Generic layer.
struct layer {

	layer(
		std::size_t _set,
		std::size_t _gridset,
		int _alpha,
		const std::string& _name
	):set{_set}, gridset(_gridset), alpha{_alpha}, id{_name} {

	}

	std::size_t         set,
	                    gridset;
	int                 alpha;
	std::string         id; //!< Unique identifier for this layer.

	virtual void        accept(layer_visitor&)=0;
	virtual void        accept(const_layer_visitor&) const=0;
};

}
