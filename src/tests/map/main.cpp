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
void check_tile(const tile_editor::tile&, std::size_t, int, int, int);
void check_thing(const tile_editor::thing&, std::size_t, int, int, std::size_t, int);
void check_thing_attribute(const tile_editor::thing&, const std::string&, int, int);
void check_thing_attribute(const tile_editor::thing&, const std::string&, double, int);
void check_thing_attribute(const tile_editor::thing&, const std::string&, const std::string&, int);

template <typename T>
void check_layer(const T& _layer, std::size_t _set, int _alpha, std::size_t _count, int _line) {

	test(_set==_layer.set, std::string{"invalid set value in line "}+std::to_string(_line)+" got "+std::to_string(_layer.set)+" expected "+std::to_string(_set));
	test(_alpha==_layer.alpha, std::string{"invalid alpha value in line "}+std::to_string(_line)+" got "+std::to_string(_layer.alpha)+" expected "+std::to_string(_alpha));
	test(_count==_layer.data.size(), std::string{"invalid item count in line "}+std::to_string(_line)+" got "+std::to_string(_layer.data.size())+" expected "+std::to_string(_count));
}

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
	must_fail(mp.get_errors(), "invalid data type in attribute, skipping property 'hello'", "invalid data type in attributes");

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
	must_fail(mp.get_errors(), "missing data in tile layer, skipping layer", "tiles node with no data member");

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
	must_fail(mp.get_errors(), "data in tile layer is not an array, skipping layer", "tiles node with non-array data member");

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

	//non-object thing layer
	mp.parse_string(R"str(
{
	"meta":{"version":"1.0,0"},
	"attributes": {"hello":12},
	"tiles":[],
	"things": [33]
}
)str");
	must_fail(mp.get_errors(), "thing layer node must be an object, skipping layer", "non-object thing layer");

	//the meta node does not need to be tested, it has been tested with the tiles.
	//the data node does not need to be tested either, it has been tested before too.

	//non-object thing item
	mp.parse_string(R"str(
{
	"meta":{"version":"1.0,0"},
	"attributes": {"hello":12},
	"tiles":[],
	"things": [
		{
			"meta":{
				"set":1,
				"alpha":0
			},
			"data":[1]
		}
	]
}
)str");
	must_fail(mp.get_errors(), "thing item is not an object, skipping item", "non-object thing layer");

	//not t member in thing item
	mp.parse_string(R"str(
{
	"meta":{"version":"1.0,0"},
	"attributes": {"hello":12},
	"tiles":[],
	"things": [
		{
			"meta":{
				"set":1,
				"alpha":0
			},
			"data":[{"meh":1}]
		}
	]
}
)str");
	must_fail(mp.get_errors(), "thing item has no 't' property, skipping item", "not t member in thing item");

	//non-int t member in thing item
	mp.parse_string(R"str(
{
	"meta":{"version":"1.0,0"},
	"attributes": {"hello":12},
	"tiles":[],
	"things": [
		{
			"meta":{
				"set":1,
				"alpha":0
			},
			"data":[{"t":"a string"}]
		}
	]
}
)str");
	must_fail(mp.get_errors(), "thing item 't' is not an integer, skipping item", "non-int t member in thing item");

	//not p member in thing item
	mp.parse_string(R"str(
{
	"meta":{"version":"1.0,0"},
	"attributes": {"hello":12},
	"tiles":[],
	"things": [
		{
			"meta":{
				"set":1,
				"alpha":0
			},
			"data":[{"t":1}]
		}
	]
}
)str");
	must_fail(mp.get_errors(), "thing item has no 'p' property, skipping item", "not p member in thing item");

	//non-array p member in thing item
	mp.parse_string(R"str(
{
	"meta":{"version":"1.0,0"},
	"attributes": {"hello":12},
	"tiles":[],
	"things": [
		{
			"meta":{
				"set":1,
				"alpha":0
			},
			"data":[{"t":1, "p":2}]
		}
	]
}
)str");
	must_fail(mp.get_errors(), "thing item 'p' is not an array, skipping item", "non-array p member in thing item");

	//invalid p member size
	mp.parse_string(R"str(
{
	"meta":{"version":"1.0,0"},
	"attributes": {"hello":12},
	"tiles":[],
	"things": [
		{
			"meta":{
				"set":1,
				"alpha":0
			},
			"data":[{"t":1, "p":[1,2,3]}]
		}
	]
}
)str");
	must_fail(mp.get_errors(), "thing item 'p' must have exactly two elements, skipping item", "invalid p member size");

	//invalid p member value (a)
	mp.parse_string(R"str(
{
	"meta":{"version":"1.0,0"},
	"attributes": {"hello":12},
	"tiles":[],
	"things": [
		{
			"meta":{
				"set":1,
				"alpha":0
			},
			"data":[{"t":1, "p":[1.2, 3]}]
		}
	]
}
)str");
	must_fail(mp.get_errors(), "thing item 'p' must have an integer as its first value, skipping item", "invalid p member value (a)");

	//invalid p member value (b)
	mp.parse_string(R"str(
{
	"meta":{"version":"1.0,0"},
	"attributes": {"hello":12},
	"tiles":[],
	"things": [
		{
			"meta":{
				"set":1,
				"alpha":0
			},
			"data":[{"t":1, "p":[1, 2.3]}]
		}
	]
}
)str");
	must_fail(mp.get_errors(), "thing item 'p' must have an integer as its second value, skipping item", "invalid p member value (b)");

	//not a property in thing item.
	mp.parse_string(R"str(
{
	"meta":{"version":"1.0,0"},
	"attributes": {"hello":12},
	"tiles":[],
	"things": [
		{
			"meta":{
				"set":1,
				"alpha":0
			},
			"data":[
				{
					"t":1,
					"p":[1, 2]
				}
			]
		}
	]
}
)str");
	must_fail(mp.get_errors(), "thing item has no 'a' property, skipping item", "not a property in thing item");

	//non-object a property in thing item.
	mp.parse_string(R"str(
{
	"meta":{"version":"1.0,0"},
	"attributes": {"hello":12},
	"tiles":[],
	"things": [
		{
			"meta":{
				"set":1,
				"alpha":0
			},
			"data":[
				{
					"t":1,
					"p":[1, 2],
					"a":"hello"
				}
			]
		}
	]
}
)str");
	must_fail(mp.get_errors(), "thing item 'a' is not an object, skipping item", "non-object a property in thing item");

	//the property parser has been already tested with the map, so we can skip that.

	//thing layer item with extraneous member...
	mp.parse_string(R"str(
{
	"meta":{"version":"1.0,0"},
	"attributes": {"hello":12},
	"tiles":[],
	"things": [
		{
			"meta":{
				"set":1,
				"alpha":0
			},
			"data":[
				{
					"t":1,
					"p":[1,2],
					"a":{
						"a": 1,
						"b": "c",
						"d": 1.2
					},
					"c":"hello"
				}
			]
		}
	]
}
)str");
	must_fail(mp.get_errors(), "thing layer item has extraneous members that will be skipped", "thing layer with extraneous members");

	//thing layer with extraneous members
	mp.parse_string(R"str(
{
	"meta":{"version":"1.0,0"},
	"attributes": {"hello":12},
	"tiles":[],
	"things": [
		{
			"meta":{
				"set":1,
				"alpha":0
			},
			"data":[
				{
					"t":1,
					"p":[1,2],
					"a":{
						"a": 1,
						"b": "c",
						"d": 1.2
					}
				}
			],
			"extraneous":"member"
		}
	]
}
)str");
	must_fail(mp.get_errors(), "thing layer node has extraneous members that will be skipped", "thing layer with extraneous members");

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

	//Non object property for poly layer
	mp.parse_string(R"str(
{
	"meta":{"version":"1.0,0"},
	"attributes": {"hello":12},
	"tiles":[],
	"things": [],
	"polys": [1,2,3]
}
)str");
	must_fail(mp.get_errors(), "poly layer node must be an object, skipping layer", "non object property for poly layer");

	//the meta node does not need to be tested, it has been tested with the tiles.
	//the data node does not need to be tested either, it has been tested before too.

	//Non object poly item
	mp.parse_string(R"str(
{
	"meta":{"version":"1.0,0"},
	"attributes": {"hello":12},
	"tiles":[],
	"things": [],
	"polys": [
		{
			"meta":{"set":1, "alpha":0},
			"data":[1,2,3]
		}
	]
}
)str");
	must_fail(mp.get_errors(), "poly item is not an object, skipping poly", "non object poly item");

	//missing t node in poly item
	mp.parse_string(R"str(
{
	"meta":{"version":"1.0,0"},
	"attributes": {"hello":12},
	"tiles":[],
	"things": [],
	"polys": [
		{
			"meta":{"set":1, "alpha":0},
			"data":[{}]
		}
	]
}
)str");
	must_fail(mp.get_errors(), "poly item has no 't' property, skipping poly", "missing t node in poly item");

	//non-numeric t node in poly item
	mp.parse_string(R"str(
{
	"meta":{"version":"1.0,0"},
	"attributes": {"hello":12},
	"tiles":[],
	"things": [],
	"polys": [
		{
			"meta":{"set":1, "alpha":0},
			"data":[{
				"t":"lala"
			}]
		}
	]
}
)str");
	must_fail(mp.get_errors(), "poly item 't' is not an integer, skipping poly", "non-numeric t node in poly item");

	//missing p node in poly item
	mp.parse_string(R"str(
{
	"meta":{"version":"1.0,0"},
	"attributes": {"hello":12},
	"tiles":[],
	"things": [],
	"polys": [
		{
			"meta":{"set":1, "alpha":0},
			"data":[{
				"t":1
			}]
		}
	]
}
)str");
	must_fail(mp.get_errors(), "poly item has no 'p' property, skipping poly", "missing p node in poly item");

	//non array p node in poly item
	mp.parse_string(R"str(
{
	"meta":{"version":"1.0,0"},
	"attributes": {"hello":12},
	"tiles":[],
	"things": [],
	"polys": [
		{
			"meta":{"set":1, "alpha":0},
			"data":[{
				"t":1,
				"p":1
			}]
		}
	]
}
)str");
	must_fail(mp.get_errors(), "poly item 'p' is not an array, skipping poly", "non array p node in poly item");

	//less than 3 items in poly item
	mp.parse_string(R"str(
{
	"meta":{"version":"1.0,0"},
	"attributes": {"hello":12},
	"tiles":[],
	"things": [],
	"polys": [
		{
			"meta":{"set":1, "alpha":0},
			"data":[{
				"t":1,
				"p":[1,2]
			}]
		}
	]
}
)str");
	must_fail(mp.get_errors(), "poly item 'p' must have at least 3 vertices represented by three arrays, skipping poly", "less than 3 items in poly item");

	//invalid poly item point: not an array
	mp.parse_string(R"str(
{
	"meta":{"version":"1.0,0"},
	"attributes": {"hello":12},
	"tiles":[],
	"things": [],
	"polys": [
		{
			"meta":{"set":1, "alpha":0},
			"data":[{
				"t":1,
				"p": [1,2,3]
			}]
		}
	]
}
)str");
	must_fail(mp.get_errors(), "poly item point must be an array, skipping poly", "invalid poly item point: not an array");

	//invalid poly item point: not exactly two items
	mp.parse_string(R"str(
{
	"meta":{"version":"1.0,0"},
	"attributes": {"hello":12},
	"tiles":[],
	"things": [],
	"polys": [
		{
			"meta":{"set":1, "alpha":0},
			"data":[{
				"t":1,
				"p": [ [1,2,3], 2, 3 ]
			}]
		}
	]
}
)str");
	must_fail(mp.get_errors(), "poly item point must have exactly two items, skipping poly", "invalid poly item point: not exactly two items");

	//invalid poly item point: first component is not an integer
	mp.parse_string(R"str(
{
	"meta":{"version":"1.0,0"},
	"attributes": {"hello":12},
	"tiles":[],
	"things": [],
	"polys": [
		{
			"meta":{"set":1, "alpha":0},
			"data":[{
				"t":1,
				"p": [ ["a",2], 2, 3 ]
			}]
		}
	]
}
)str");
	must_fail(mp.get_errors(), "poly item point first component must be an integer, skipping poly", "invalid poly item point: first component is not an integer");

	//invalid poly item point: second component is not an integer
	mp.parse_string(R"str(
{
	"meta":{"version":"1.0,0"},
	"attributes": {"hello":12},
	"tiles":[],
	"things": [],
	"polys": [
		{
			"meta":{"set":1, "alpha":0},
			"data":[{
				"t":1,
				"p": [ [2, "b"], 2, 3]
			}]
		}
	]
}
)str");
	must_fail(mp.get_errors(), "poly item point second component must be an integer, skipping poly", "invalid poly item point: second component is not an integer");

	//not a property in poly item.
	mp.parse_string(R"str(
{
	"meta":{"version":"1.0,0"},
	"attributes": {"hello":12},
	"tiles":[],
	"things": [],
	"polys": [
		{
			"meta":{"set":1, "alpha":0},
			"data":[{
				"t":1,
				"p": [ [1,2], [3,4], [5, 6] ]
			}]
		}
	]
}
)str");
	must_fail(mp.get_errors(), "poly item has no 'a' property, skipping poly", "not a property in poly item");

	//non-object a property in poly item.
	mp.parse_string(R"str(
{
	"meta":{"version":"1.0,0"},
	"attributes": {"hello":12},
	"tiles":[],
	"things": [],
	"polys": [
		{
			"meta":{"set":1, "alpha":0},
			"data":[{
				"t":1,
				"p": [ [1,2], [3,4], [5,6] ],
				"a": "string"
			}]
		}
	]
}
)str");
	must_fail(mp.get_errors(), "poly item 'a' is not an object, skipping poly", "non-object a property in poly item");

	//the property parser has been already tested with the map, so we can skip that.

	//poly item with extraneous member...
	mp.parse_string(R"str(
{
	"meta":{"version":"1.0,0"},
	"attributes": {"hello":12},
	"tiles":[],
	"things": [],
	"polys": [
		{
			"meta":{"set":1, "alpha":0},
			"data":[{
				"t":1,
				"p": [ [1,2], [3,4], [5,6] ],
				"a": {},
				"lol":33
			}]
		}
	]
}
)str");
	must_fail(mp.get_errors(), "poly layer item has extraneous members that will be skipped", "poly item with extraneous member");

	//poly layer with extraneous member...
	mp.parse_string(R"str(
{
	"meta":{"version":"1.0,0"},
	"attributes": {"hello":12},
	"tiles":[],
	"things": [],
	"polys": [
		{
			"meta":{"set":1, "alpha":0},
			"data":[{
				"t":1,
				"p": [ [1,2], [3,4], [5,6] ],
				"a": {}
			}],
			"lol":"useless"
		}
	]
}
)str");
	must_fail(mp.get_errors(), "poly layer node has extraneous members that will be skipped", "poly layer with extraneous member");

	try {
		std::cout<<"testing mostly valid map file"<<std::endl;
		auto map=mp.parse_file("data/almost_good.map");
		const auto& errors=mp.get_errors();
		test(errors.size()==6, "there were unexpected errors parsing the mostly good map");

		test("tile item has no 'p' property, skipping item"==errors[0], "invalid error");
		test("missing meta node in layer, skipping layer meta"==errors[1], "invalid error");
		test("missing data in tile layer, skipping layer"==errors[2], "invalid error");
		test("thing item has no 'p' property, skipping item"==errors[3], "invalid error");
		test("missing meta node in layer, skipping layer meta"==errors[4], "invalid error");
		test("missing data in thing layer, skipping layer"==errors[5], "invalid error");

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

		check_layer(map.tile_layers[0], 1, 0, 2, __LINE__);
		check_tile(map.tile_layers[0].data[0], 1, 2, 3, __LINE__);
		check_tile(map.tile_layers[0].data[1], 2, 4, 5, __LINE__);

		check_layer(map.tile_layers[1], 2, 128, 1, __LINE__);
		check_tile(map.tile_layers[1].data[0], 3, 10, 11, __LINE__);

		test(2==map.thing_layers.size(), "invalid parsing of thing layers");
		check_layer(map.thing_layers[0], 3, 32, 2, __LINE__);

		check_thing(map.thing_layers[0].data[0], 1, 10, 11, 3, __LINE__);

		check_thing_attribute(map.thing_layers[0].data[0], "some_attribute", 1, __LINE__);
		check_thing_attribute(map.thing_layers[0].data[0], "some_other_attribute", 2.2, __LINE__);
		check_thing_attribute(map.thing_layers[0].data[0], "and_another_one", "yes", __LINE__);

		check_thing(map.thing_layers[0].data[1], 2, 14, 15, 1, __LINE__);
		check_thing_attribute(map.thing_layers[0].data[1], "an_attribute", 2, __LINE__);

		check_layer(map.thing_layers[1], 4, 64, 1, __LINE__);
		check_thing(map.thing_layers[1].data[0], 3, 16, 17, 0, __LINE__);

		//TODO: Check poly layers


	}
	catch(std::exception& e) {

		std::cerr<<"failed: "<<e.what()<<std::endl;
		std::exit(1);
	}

	try {
	/*
		std::cout<<"testing fully valid map file"<<std::endl;
		auto map=mp.parse_file("data/good.map");
		const auto& errors=mp.get_errors();
		test(errors.size()==0, "there were unexpected errors parsing the good map");
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

		check_layer(map.tile_layers[0], 1, 0, 2, __LINE__);
		check_tile(map.tile_layers[0].data[0], 1, 2, 3, __LINE__);
		check_tile(map.tile_layers[0].data[1], 2, 4, 5, __LINE__);

		check_layer(map.tile_layers[1], 2, 128, 1, __LINE__);
		check_tile(map.tile_layers[1].data[0], 3, 10, 11, __LINE__);
	*/
	}
	catch(std::exception& e) {

		std::cerr<<"failed: "<<e.what()<<std::endl;
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

void check_tile(const tile_editor::tile& _tile, std::size_t _type, int _x, int _y, int _line) {

	test(_type==_tile.type, std::string{"invalid type for tile "}+std::to_string(_line)+" got "+std::to_string(_tile.type)+" expected "+std::to_string(_type));
	test(_x==_tile.x, std::string{"invalid x for tile "}+std::to_string(_line)+" got "+std::to_string(_tile.x)+" expected "+std::to_string(_x));
	test(_y==_tile.y, std::string{"invalid t for tile "}+std::to_string(_line)+" got "+std::to_string(_tile.y)+" expected "+std::to_string(_y));
}

void check_thing(
	const tile_editor::thing& _thing,
	std::size_t _type,
	int _x,
	int _y,
	std::size_t _propcount,
	int _line
) {

	const int default_w=1,
	          default_h=1,
	          default_r=128,
	          default_g=128,
	          default_b=128,
	          default_a=128;

	test(_type==_thing.type, std::string{"invalid type for thing "}+std::to_string(_line)+" got "+std::to_string(_thing.type)+" expected "+std::to_string(_type));
	test(_x==_thing.x, std::string{"invalid x for thing "}+std::to_string(_line)+" got "+std::to_string(_thing.x)+" expected "+std::to_string(_x));
	test(_y==_thing.y, std::string{"invalid t for thing "}+std::to_string(_line)+" got "+std::to_string(_thing.y)+" expected "+std::to_string(_y));
	test(_propcount==_thing.properties.size(), std::string{"invalid property size for thing "}+std::to_string(_line)+" got "+std::to_string(_thing.properties.size())+" expected "+std::to_string(_propcount));
	test(default_w==_thing.w, std::string{"invalid default w for thing "}+std::to_string(_line)+" got "+std::to_string(_thing.w)+" expected "+std::to_string(default_w));
	test(default_h==_thing.h, std::string{"invalid default h for thing "}+std::to_string(_line)+" got "+std::to_string(_thing.h)+" expected "+std::to_string(default_h));
	test(default_r==_thing.color.r, std::string{"invalid default color r for thing "}+std::to_string(_line)+" got "+std::to_string(_thing.color.r)+" expected "+std::to_string(default_r));
	test(default_g==_thing.color.g, std::string{"invalid default color g for thing "}+std::to_string(_line)+" got "+std::to_string(_thing.color.g)+" expected "+std::to_string(default_g));
	test(default_b==_thing.color.b, std::string{"invalid default color b for thing "}+std::to_string(_line)+" got "+std::to_string(_thing.color.b)+" expected "+std::to_string(default_b));
	test(default_a==_thing.color.a, std::string{"invalid default color a for thing "}+std::to_string(_line)+" got "+std::to_string(_thing.color.a)+" expected "+std::to_string(default_a));
}

void check_thing_attribute(
	const tile_editor::thing& _thing,
	const std::string& _key,
	int _value,
	int _line
) {
	test(_thing.properties.has_property(_key), std::string{"property does not exist on line "}+std::to_string(_line));
	test(_thing.properties.int_properties.count(_key), std::string{"property is not an int on line "}+std::to_string(_line));

	auto val=_thing.properties.int_properties.at(_key);
	test(val==_value, std::string{"invalid integer property value, got "}+std::to_string(val)+", expected "+std::to_string(_value)+" on line "+std::to_string(_line));
}
void check_thing_attribute(
	const tile_editor::thing& _thing,
	const std::string& _key,
	double _value,
	int _line
) {

	test(_thing.properties.has_property(_key), std::string{"property does not exist on line "}+std::to_string(_line));
	test(_thing.properties.double_properties.count(_key), std::string{"property is not a double on line "}+std::to_string(_line));

	auto val=_thing.properties.double_properties.at(_key);
	test(val==_value, std::string{"invalid double property value, got "}+std::to_string(val)+", expected "+std::to_string(_value)+" on line "+std::to_string(_line));
}

void check_thing_attribute(
	const tile_editor::thing& _thing,
	const std::string& _key,
	const std::string& _value,
	int _line
) {

	test(_thing.properties.has_property(_key), std::string{"property does not exist on line "}+std::to_string(_line));
	test(_thing.properties.string_properties.count(_key), std::string{"property is not a string on line "}+std::to_string(_line));

	auto val=_thing.properties.string_properties.at(_key);
	test(val==_value, std::string{"invalid string property value, got "}+val+", expected "+_value+" on line "+std::to_string(_line));
}
