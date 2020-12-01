#pragma once

#include "../blueprint_types/poly_definition_table.h"
#include <tools/text_reader.h>
#include <string>

namespace tile_editor {

class poly_parser {

	public:

	poly_definition_table::table_type     read_file(const std::string&);

	private:

	void                      parse_poly(tools::text_reader&, poly_definition_table::table_type&);
};

}
