#pragma once

#include "../blueprint_types/thing_definition_table.h"
#include <tools/text_reader.h>

#include <string>
#include <sstream>
#include <map>
#include <stdexcept>

namespace tile_editor {

class thing_parser {

	public:

	thing_definition_table::table_type    read_file(const std::string&);

	private:

	template <typename T>
	T convert_value(const std::map<std::string, std::string>& _map, const std::string& _key) {

		T result{};
		std::stringstream ss{_map.at(_key)};
		ss>>result;

		if(ss.fail()) {

			throw std::runtime_error{std::string{"invalid value for property '"}+_key+"'"};
		}

		return result;
	}

	void                      parse_object(tools::text_reader&, thing_definition_table::table_type&);
};

}
