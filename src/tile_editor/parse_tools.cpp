#include "parser/parse_tools.h"

#include <stdexcept>
#include <algorithm>
#include <sstream>

using namespace tile_editor;

config_pair tile_editor::from_reader(
	tools::text_reader& _reader,
	const std::vector<std::string>& _allowed) {

	auto result=from_reader(_reader);

	if(_allowed.end()==std::find(
		std::begin(_allowed), 
		std::end(_allowed), 
		result.name
	)) {

		result.disallowed=true;
	}

	return result;
}

config_pair tile_editor::from_reader(
	tools::text_reader& _reader
) {

	config_pair result;

	std::stringstream ss{_reader.read_line()};

	if(_reader.is_eof()) {
	
		result.eof=true;
		return result;
	}

	//skips whitespace for the extraction of a single value...
	ss>>std::ws>>result.name;

	//skip whitespace and read the rest of the line...
	std::getline(ss>>std::ws, result.value);
	if(ss.fail()) {
		result.failed=true;
		return result;
	}

	return result;
}

std::map<std::string, std::string> tile_editor::generic_first_level(
	tools::text_reader& _reader,
	const std::string& _end,
	const std::vector<std::string>& _properties
) {

	std::map<std::string, std::string> propmap;
	for(const auto& str : _properties) {
		propmap[str]="";
	};

	std::vector allowed_words=_properties;
	allowed_words.push_back(_end);

	while(true) {

		auto pair=from_reader(_reader, allowed_words);

		if(pair.eof) {

			throw std::runtime_error(std::string{"unexpected end of file before '"}+_end+"'");
		}

		if(pair.name==_end) {

			break;
		}

		if(pair.disallowed) {

			throw std::runtime_error{std::string{"unrecognised '"}+pair.name+"', not within allowed properties"};
		}

		if(pair.failed) {

			throw std::runtime_error("syntax error: expected property value");
		}

		//Empty properties are not allowed, so this is legal.
		if(propmap[pair.name].size()) {

			throw std::runtime_error(std::string{"repeated property '"}+pair.name+"'");
		}

		propmap[pair.name]=pair.value;
	}

	for(const auto& pair : propmap) {

		if(!pair.second.size()) {
			throw std::runtime_error(std::string{"missing value for '"}+pair.first+"'");
		}
	}

	return propmap;
}
