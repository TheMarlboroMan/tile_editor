#pragma once

#include "../blueprint_types/thing_definition_table.h"
#include <tools/text_reader.h>
#include <string>


namespace tile_editor {

class thing_parser {

	public:

	thing_definition_table    read_file(const std::string&);

	private:

	void                      parse_object(tools::text_reader&, thing_definition_table&);
};

}
