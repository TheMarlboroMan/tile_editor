#pragma once

#include "../blueprint_types/property_table.h"

#include <tools/text_reader.h>
#include <string>
#include <sstream>

namespace tile_editor {

class property_parser {

	public:
	                        property_parser(bool);
	property_table          read_file(const std::string&);
	void                    read(tools::text_reader&, property_table&);

	private:

	bool                    map_mode;

	template <typename T>
	void 					insert(
		const std::string& _name, 
		const std::string& _default, 
		const std::string& _comment, 
		property_links _link,
		std::map<std::string, property_definition<T>>& _target) {

		std::stringstream ss{_default};
		T defval{};
		ss>>defval;

		if(ss.fail()) {

			throw std::runtime_error("invalid property value");
		}

		_target[_name]=property_definition<T>{
			_name,
			defval,
			_comment,
			_link
		};
	}
};

}
