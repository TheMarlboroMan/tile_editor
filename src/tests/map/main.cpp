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

	//tiles node whose member is not an object
	mp.parse_string(R"str(
{
	"meta":{"version":"1.0,0"},
	"attributes": {"hello":12},
	"tiles": [
		"a", "b", "c"
	]
}
)str");
	must_fail(mp.get_errors(), "tile layer node must be an object, skipping layer", "tiles node whose member is not an object");

	//tiles node with no meta
	mp.parse_string(R"str(
{
	"meta":{"version":"1.0,0"},
	"attributes": {"hello":12},
	"tiles": [
		{
			"no_meta": 6
		}
	]
}
)str");
	must_fail(mp.get_errors(), "missing meta node in layer, skipping layer meta", "tiles node with no meta");

	//tiles node with invalid meta type
	mp.parse_string(R"str(
{
	"meta":{"version":"1.0,0"},
	"attributes": {"hello":12},
	"tiles": [
		{
			"meta": "hey"
		}
	]
}
)str");
	must_fail(mp.get_errors(), "meta node in layer must be an object, skipping layer meta", "tiles node with invalid meta type");

	//tiles node with no alpha
	mp.parse_string(R"str(
{
	"meta":{"version":"1.0,0"},
	"attributes": {"hello":12},
	"tiles": [
		{
			"meta": {
				"no-alpha": "hehe"
			}
		}
	]
}
)str");
	must_fail(mp.get_errors(), "meta node in layer has no 'alpha'", "tiles node with no alpha");

	//tiles node with invalid alpha
	mp.parse_string(R"str(
{
	"meta":{"version":"1.0,0"},
	"attributes": {"hello":12},
	"tiles": [
		{
			"meta": {
				"alpha": "string"
			}
		}
	]
}
)str");
	must_fail(mp.get_errors(), "meta:alpha node is not an integer", "tiles node with invalid alpha");

	//tiles node with no set
	mp.parse_string(R"str(
{
	"meta":{"version":"1.0,0"},
	"attributes": {"hello":12},
	"tiles": [
		{
			"meta": {
				"alpha": 128
			}
		}
	]
}
)str");
	must_fail(mp.get_errors(), "meta node in layer has no 'set'", "tiles node with no set");

	//tiles node with invalid set
	mp.parse_string(R"str(
{
	"meta":{"version":"1.0,0"},
	"attributes": {"hello":12},
	"tiles": [
		{
			"meta": {
				"alpha": 128,
				"set" : 12.44
			}
		}
	]
}
)str");
	must_fail(mp.get_errors(), "meta:set node is not an integer", "tiles node with invalid set");

	//tiles node with extraneous meta members.
	mp.parse_string(R"str(
{
	"meta":{"version":"1.0,0"},
	"attributes": {"hello":12},
	"tiles": [
		{
			"meta": {
				"alpha": 128,
				"set" : 1,
				"intruder": "yes"
			}
		}
	]
}
)str");
	must_fail(mp.get_errors(), "meta node in layer has extraneous members which will be ignored", "tiles node with extraneous meta members.");

	//tiles node with no data member
	mp.parse_string(R"str(
{
	"meta":{"version":"1.0,0"},
	"attributes": {"hello":12},
	"tiles": [
		{
			"meta": {
				"alpha": 128,
				"set" : 1
			}
		}
	]
}
)str");
	must_fail(mp.get_errors(), "missing data in layer, skipping layer", "tiles node with no data member");

	//tiles node with non-array data member
	mp.parse_string(R"str(
{
	"meta":{"version":"1.0,0"},
	"attributes": {"hello":12},
	"tiles": [
		{
			"meta": {
				"alpha": 128,
				"set" : 1
			},
			"data": 12
		}
	]
}
)str");
	must_fail(mp.get_errors(), "data in layer is not an array, skipping layer", "tiles node with non-array data member");

	//tiles node not an object
	mp.parse_string(R"str(
{
	"meta":{"version":"1.0,0"},
	"attributes": {"hello":12},
	"tiles": [
		{
			"meta": {
				"alpha": 128,
				"set" : 1
			},
			"data": [1,2,3]
		}
	]
}
)str");
	must_fail(mp.get_errors(), "tile item is not an object, skipping item", "tiles node not an object");

	//tiles node item with no type
	mp.parse_string(R"str(
{
	"meta":{"version":"1.0,0"},
	"attributes": {"hello":12},
	"tiles": [
		{
			"meta": {
				"alpha": 128,
				"set" : 1
			},
			"data": [
				{}
			]
		}
	]
}
)str");
	must_fail(mp.get_errors(), "tile item has no 't' property, skipping item", "tiles node item with no type");

	//tiles node item with non-int type
	mp.parse_string(R"str(
{
	"meta":{"version":"1.0,0"},
	"attributes": {"hello":12},
	"tiles": [
		{
			"meta": {
				"alpha": 128,
				"set" : 1
			},
			"data": [
				{"t":1.234}
			]
		}
	]
}
)str");
	must_fail(mp.get_errors(), "tile item 't' is not an integer, skipping item", "tiles node item with non-int type");

	//tiles node item with no location
	mp.parse_string(R"str(
{
	"meta":{"version":"1.0,0"},
	"attributes": {"hello":12},
	"tiles": [
		{
			"meta": {
				"alpha": 128,
				"set" : 1
			},
			"data": [
				{"t": 1}
			]
		}
	]
}
)str");
	must_fail(mp.get_errors(), "tile item has no 'p' property, skipping item", "tiles node item with no location");

	//tiles node item with non-array location
	mp.parse_string(R"str(
{
	"meta":{"version":"1.0,0"},
	"attributes": {"hello":12},
	"tiles": [
		{
			"meta": {
				"alpha": 128,
				"set" : 1
			},
			"data": [
				{
					"t": 1,
					"p": 12
				}
			]
		}
	]
}
)str");
	must_fail(mp.get_errors(), "tile item 'p' is not an array, skipping item", "tiles node item with non-array location");

	//tiles node item with bad length location
	mp.parse_string(R"str(
{
	"meta":{"version":"1.0,0"},
	"attributes": {"hello":12},
	"tiles": [
		{
			"meta": {
				"alpha": 128,
				"set" : 1
			},
			"data": [
				{
					"t": 1,
					"p": [1,2,3]
				}
			]
		}
	]
}
)str");
	must_fail(mp.get_errors(), "tile item 'p' must have exactly two elements, skipping item", "tiles node item with bad length location");

	//tiles node item with non integer location value (a)
	mp.parse_string(R"str(
{
	"meta":{"version":"1.0,0"},
	"attributes": {"hello":12},
	"tiles": [
		{
			"meta": {
				"alpha": 128,
				"set" : 1
			},
			"data": [
				{
					"t": 1,
					"p": ["a", 1]
				}
			]
		}
	]
}
)str");
	must_fail(mp.get_errors(), "tile item 'p' must have an integer as its first value, skipping item", "tiles node item with non integer location value (a)");

	//tiles node item with non integer location value (b)
	mp.parse_string(R"str(
{
	"meta":{"version":"1.0,0"},
	"attributes": {"hello":12},
	"tiles": [
		{
			"meta": {
				"alpha": 128,
				"set" : 1
			},
			"data": [
				{
					"t": 1,
					"p": [1,"a"]
				}
			]
		}
	]
}
)str");
	must_fail(mp.get_errors(), "tile item 'p' must have an integer as its second value, skipping item", "tiles node item with non integer location value (b)");

	//tiles node item extraneous members.
	mp.parse_string(R"str(
{
	"meta":{"version":"1.0,0"},
	"attributes": {"hello":12},
	"tiles": [
		{
			"meta": {
				"alpha": 128,
				"set" : 1
			},
			"data": [
				{
					"t": 1,
					"p": [2,3],
					"what":"now"
				}
			]
		}
	]
}
)str");
	must_fail(mp.get_errors(), "tile layer item has extraneous members that will be skipped", "tiles node item extraneous members.");

	//tiles layer with extraneous members (non meta or data).
	mp.parse_string(R"str(
{
	"meta":{"version":"1.0,0"},
	"attributes": {"hello":12},
	"tiles": [
		{
			"meta": {
				"alpha": 128,
				"set" : 1
			},
			"data": [
				{
					"t": 1,
					"p": [2,3]
				}
			],
			"what":"now"
		}
	]
}
)str");
	must_fail(mp.get_errors(), "tile layer node has extraneous members that will be skipped", "tiles layer with extraneous members (non meta or data)");

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

		test(2==map.tile_layers.size(), "invalid parsing of tile layers");

		test(1==map.tile_layers[0].set, "invalid set value for first tile layer");
		test(0==map.tile_layers[0].alpha, "invalid alpha value for first tile layer");
		test(2==map.tile_layers[0].data.size(), "invalid item count for first tile layer");

		test(1==map.tile_layers[0].data[0].type, "invalid type for first tile layer item 0");
		test(2==map.tile_layers[0].data[0].x, "invalid x for first tile layer item 0");
		test(3==map.tile_layers[0].data[0].y, "invalid t for first tile layer item 0");
		test(2==map.tile_layers[0].data[1].type, "invalid type for first tile layer item 1");
		test(4==map.tile_layers[0].data[1].x, "invalid x for first tile layer item 1");
		test(5==map.tile_layers[0].data[1].y, "invalid t for first tile layer item 1");

		test(2==map.tile_layers[1].set, "invalid set value for second tile layer");
		test(128==map.tile_layers[1].alpha, "invalid alpha value for second tile layer");
		test(1==map.tile_layers[1].data.size(), "invalid item count for second tile layer");

		test(3==map.tile_layers[1].data[0].type, "invalid type for first tile layer item 1");
		test(10==map.tile_layers[1].data[0].x, "invalid x for second tile layer item 0");
		test(11==map.tile_layers[1].data[0].y, "invalid t for second tile layer item 0");
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

