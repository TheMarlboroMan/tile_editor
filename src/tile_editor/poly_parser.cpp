#include "parser/poly_parser.h"
#include "parser/parse_tools.h"
#include "parser/property_parser.h"
#include "blueprint_types/property_table.h"
#include "blueprint_types/property_definition.h"

#include <tools/file_utils.h>

#include <sstream>
#include <map>

using namespace tile_editor;

poly_definition_table::table_type poly_parser::read_file(const std::string& _filename) {

	if(!tools::file_exists(_filename)) {

		throw std::runtime_error(std::string{"cannot find file '"}+_filename+"'");
	}

	const std::string   begin{"beginpoly"};

	poly_definition_table::table_type result;
	int flags=tools::text_reader::ltrim | tools::text_reader::rtrim | tools::text_reader::ignorewscomment;
	tools::text_reader reader{_filename, '#', flags};

	try {
		while(true) {

			std::stringstream ss{reader.read_line()};
			if(reader.is_eof()) {
				break;
			}

			std::string tag;
			ss>>std::skipws>>tag;

			if(tag!=begin) {

				throw std::runtime_error(std::string{"unexpected '"+tag+"', expected beginobject"});
			}

			parse_poly(reader, result);
		}
	}
	catch(std::exception& e) {

		throw std::runtime_error(
			std::string{e.what()}
			+" on file "+_filename
			+" line "+std::to_string(reader.get_line_number())
		);
	}

	return result;
}

void poly_parser::parse_poly(
	tools::text_reader& _reader,
	poly_definition_table::table_type& _result
) {

	//This works mostly the same as a thing definition.

	const std::string   end{"endpoly"},
	                    propstart{"beginproperty"};

	property_table pt;
	std::map<std::string, std::string> properties{
		{"id", ""},
		{"name", ""},
		{"color", ""}
	};

	while(true) {

		const auto pair=from_reader(_reader);

		if(pair.eof) {

			throw std::runtime_error{"unexpected file end before 'endpoly'"};
		}

		if(pair.name==propstart) {

			property_parser pp(false);
			pp.read(_reader, pt);
			continue;
		}

		if(pair.name==end) {

			break;
		}

		if(pair.failed) {

			throw std::runtime_error(std::string{"missing property value for '"}+pair.name+"'");
		}

		if(!properties.count(pair.name)) {

			throw std::runtime_error(std::string{"unknown property name '"}+pair.name+"'");
		}

		if(properties[pair.name].size()) {

			throw std::runtime_error(std::string{"repeated property '"}+pair.name+"'");
		}

		properties[pair.name]=pair.value;
	}

	for(const auto& pair : properties) {

		if(!pair.second.size()) {

			throw std::runtime_error{std::string{"missing property '"}+pair.first+"'"};
		}
	}

	//Now, any property linked to w or h must immediately fail.
	for(const auto& prop : pt.int_properties) {

		if(prop.second.linked_to==property_links::w || prop.second.linked_to==property_links::h) {

			throw std::runtime_error{"polygon properties cannot be linked to width or height"};
		}
	}

	std::size_t id{};
	std::stringstream ss{properties["id"]};
	ss>>id;

	if(ss.fail()) {

		throw std::runtime_error("invalid id value");
	}

	if(_result.count(id)) {

		throw std::runtime_error("repeated poly definition id");
	}

	auto color=parse_color(properties["color"]);

	_result[id]={
		id,
		properties["name"],
		color,
		pt
	};
}
