#pragma once

#include "thing_definition.h"

#include <map>
#include <string>

namespace tile_editor {

struct thing_definition_table {

	public:

	using                               table_type=std::map<size_t, thing_definition>;

	std::string                         name;
	table_type                          table;
};

}
