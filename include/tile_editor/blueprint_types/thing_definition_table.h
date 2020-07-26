#pragma once

#include "thing_definition.h"

#include <map>

namespace tile_editor {

//!A table of things.
class thing_definition_table {

	using container=std::map<size_t, thing_definition>;

	const thing_definition&         get(size_t) const;
	thing_definition                get(size_t);

	private:

	container                        data;	//!< Internal data storage.
};
}