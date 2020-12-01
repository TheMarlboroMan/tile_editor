#pragma once

#include "poly_definition.h"

#include <map>
#include <string>

namespace tile_editor {

struct poly_definition_table {

	public:

	using                               table_type=std::map<size_t, poly_definition>;

	std::string                         name;
	table_type                          table;
};

}
