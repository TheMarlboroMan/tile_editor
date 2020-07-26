#pragma once

#include "thing_definition.h"

#include <map>

namespace tile_editor {

//!A table of things.
struct thing_definition_table {

	using container=std::map<size_t, thing_definition>;
	container                        data;	//!< Internal data storage.
};
}
