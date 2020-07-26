#pragma once

#include "property_table.h"

#include <tools/text_reader.h>
#include <string>

namespace tile_editor {

class property_parser {

	public:

	property_table          read_file(const std::string&);
	void                    read(tools::text_reader&, property_table&);

	private:
};

}
