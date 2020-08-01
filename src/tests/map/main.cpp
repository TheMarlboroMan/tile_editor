/*
This file tests the map file parsers.
*/

#include "parser/map_parser.h"

#include <iostream>
#include <stdexcept>
#include <map>

void fail(const std::string& _msg);
void test(bool _thing, const std::string& _msg);
void must_fail(std::vector<std::string> _errors, const std::string& _errmsg, const std::string& _type);

int main(int /*argc*/, char ** /*argv*/) {

	tile_editor::map_parser mp;

	std::cout<<"testing invalid map files..."<<std::endl;

	//non-existing map file
	mp.parse_file("data/no-real-file");
	must_fail(mp.get_errors(), "map file does not exist", "non-existing map file");

	//non json contents
	mp.parse_string("this is clearly not json");
	must_fail(mp.get_errors(), "could not parse json", "non json contents");

	//non object root node
	mp.parse_string("[\"hello\"]");
	must_fail(mp.get_errors(), "json root node must be an object", "non object root node");

	//no meta node
	mp.parse_string("{\"hello\":33}");
	must_fail(mp.get_errors(), "no 'meta' node found, metadata will be skipped", "no meta node");

	//meta node is not an object
	mp.parse_string("{\"meta\":[]}");
	must_fail(mp.get_errors(), "'meta' node must be an object, metadata will be skipped", "meta node is not an object");

	//missing version meta
	mp.parse_string("{\"meta\":{}}");
	must_fail(mp.get_errors(), "'meta' node must contain version, version will be skipped", "missing version meta");

	//non-string version meta
	mp.parse_string("{\"meta\":{\"version\": 33}}");
	must_fail(mp.get_errors(), "'meta' node must contain version as a string, version will be skipped", "non-string version meta");

	//missing attributes
	mp.parse_string(R"str(
{
	"meta":{"version":"1.0,0"}
}
)str");
	must_fail(mp.get_errors(), "no 'attributes' node found, attributes will be skipped", "missing attributes");

	//no object attributes
	mp.parse_string(R"str(
{
	"meta":{"version":"1.0,0"},
	"attributes":[1,2,3]
}
)str");
	must_fail(mp.get_errors(), "'attributes' node must be an object, attributes will be skipped", "no object attributes");

	//invalid data type in attributes
	mp.parse_string(R"str(
{
	"meta":{"version":"1.0,0"},
	"attributes": {"hello":[1,2,3]}
}
)str");
	must_fail(mp.get_errors(), "invalid data type in 'attributes', skipping property 'hello'", "invalid data type in attributes");

	//repeated property in attributes
	mp.parse_string(R"str(
{
	"meta":{"version":"1.0,0"},
	"attributes": {"hello":12, "hello":"again"}
}
)str");
	must_fail(mp.get_errors(), "'hello' already exists as attribute, skipping property", "repeated property in attributes");

	//no tiles node
	mp.parse_string(R"str(
{
	"meta":{"version":"1.0,0"},
	"attributes": {"hello":12}
}
)str");
	must_fail(mp.get_errors(), "no 'tiles' node found, tiles will be skipped", "no tiles node");

	//tiles node is not an array
	mp.parse_string(R"str(
{
	"meta":{"version":"1.0,0"},
	"attributes": {"hello":12},
	"tiles": 33
}
)str");
	must_fail(mp.get_errors(), "'tiles' node must be an array, tiles will be skipped", "tiles node is not an array");

	//TODO TODO

	//no things node
	mp.parse_string(R"str(
{
	"meta":{"version":"1.0,0"},
	"attributes": {"hello":12},
	"tiles":[]
}
)str");
	must_fail(mp.get_errors(), "no 'things' node found, things will be skipped", "no things node");

	//things node is not an array
	mp.parse_string(R"str(
{
	"meta":{"version":"1.0,0"},
	"attributes": {"hello":12},
	"tiles":[],
	"things": 33
}
)str");
	must_fail(mp.get_errors(), "'things' node must be an array, things will be skipped", "things node is not an array");

	//TODO: TODO: 

	//no polys node
	mp.parse_string(R"str(
{
	"meta":{"version":"1.0,0"},
	"attributes": {"hello":12},
	"tiles":[],
	"things":[]
}
)str");
	must_fail(mp.get_errors(), "no 'polys' node found, polys will be skipped", "no polys node");

	//polys node is not an array
	mp.parse_string(R"str(
{
	"meta":{"version":"1.0,0"},
	"attributes": {"hello":12},
	"tiles":[],
	"things": [],
	"polys": 33
}
)str");
	must_fail(mp.get_errors(), "'polys' node must be an array, polys will be skipped", "polys node is not an array");

	//TODO TODO

	//Testing good map...

	try {
		std::cout<<"testing valid map file"<<std::endl;
		auto map=mp.parse_file("data/good.map");
		test(mp.get_errors().size()==0, "there were errors parsing the good map");
		test(mp.get_version()=="1.0.0", "could not assert the map version");

		test(1==map.properties.string_properties.count("apropos"), "no 'apropos' attribute");
		test("custom attributes go here"==map.properties.string_properties["apropos"], "invalid value for 'apropos' attribute");

		test(1==map.properties.string_properties.count("name"), "no 'name' attribute");
		test("Test map"==map.properties.string_properties["name"], "invalid value for 'name' attribute");

		test(1==map.properties.int_properties.count("overworld_position_x"), "no 'overworld_position_x' attribute");
		test(0==map.properties.int_properties["overworld_position_x"], "invalid value for 'overworld_position_x' attribute");

		test(1==map.properties.int_properties.count("overworld_position_y"), "no 'overworld_position_y' attribute");
		test(0==map.properties.int_properties["overworld_position_y"], "invalid value for 'overworld_position_y' attribute");

		test(1==map.properties.int_properties.count("special_effects"), "no 'special_effects' attribute");
		test(12==map.properties.int_properties["special_effects"], "invalid value for 'special_effects' attribute");

		test(1==map.properties.double_properties.count("gravity_factor"), "no 'gravity_factor' attribute");
		test(1.2==map.properties.double_properties["gravity_factor"], "invalid value for 'gravity_factor' attribute");
	}
	catch(std::exception& e) {

		std::cerr<<"failed: "<<e.what()<<std::endl;
		for(const auto& err : mp.get_errors()) {
			std::cerr<<">"<<err<<std::endl;
		}
		std::exit(1);
	}

	std::cout<<"all good"<<std::endl;

	return 0;
}

void fail(const std::string& _msg) {

	throw std::runtime_error(_msg);
}

void test(bool _thing, const std::string& _msg) {

	if(!_thing) {
		fail(_msg);
	}
}

void must_fail(
	std::vector<std::string> _errors,
	const std::string& _errmsg,
	const std::string& _type
) {

	if(!_errors.size()) {

		std::cerr<<"'"<<_type<<"' should have failed..."<<std::endl;
		std::exit(1);
	}

	if(_errors[0].find(_errmsg)==std::string::npos) {

		std::cerr<<"expected '"<<_errmsg<<"', got '"<<_errors[0]<<"'"<<std::endl;
		std::exit(1);
	}

	std::cout<<"caught: "<<_type<<std::endl;
}

