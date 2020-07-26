#pragma once

#include <property_manager.h>

namespace tile_editor {

//!This is an in-editor thing, a logic object. Named following the grand Doom
//!tradition. Its properties will depend on what properties are established
//!in the blueprint of its thing_definition (according to its type id).
struct thing {

	public:

	int                                 x,
	                                    y,
	                                    w,
	                                    h,
	                                    type;

	property_manager                    properties;
};

}