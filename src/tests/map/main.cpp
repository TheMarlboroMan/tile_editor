/*
This file tests the map file parsers.
*/

#include "parser/map_parser.h"
#include "../tests.h"

#include <iostream>
#include <stdexcept>
#include <map>

void must_fail(std::vector<std::string> _errors, const std::string& _errmsg, const std::string& _type);
void check_layer(const tile_editor::layer& _layer, std::size_t _set, int _alpha, std::size_t _count, const std::string _id, int _line);
void check_tile(const tile_editor::layer&, std::size_t, std::size_t, int, int, int);
void check_thing(const tile_editor::layer&, std::size_t, std::size_t, int, int, std::size_t, int);
void check_poly(const tile_editor::layer&, std::size_t, std::size_t, const std::vector<tile_editor::poly_point>&, std::size_t, int);
void check_component_attribute(const tile_editor::layer& _subject, std::size_t _index, const std::string& _key, int _value, int _line);
void check_component_attribute(const tile_editor::layer& _subject, std::size_t _index, const std::string& _key, double _value, int _line);
void check_component_attribute(const tile_editor::layer& _subject, std::size_t _index, const std::string& _key, const std::string& _value, int _line);

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
	must_fail(mp.get_errors(), "no 'layers' node found, layers will be skipped", "no layers node");

	//tiles node is not an array
	mp.parse_string(R"str(
{
	"meta":{"version":"1.0,0"},
	"attributes": {"hello":12},
	"layers": 33
}
)str");
	must_fail(mp.get_errors(), "'layers' node must be an array, layers will be skipped", "layers node is not an array");

	//tiles node whose member is not an object
	mp.parse_string(R"str(
{
	"meta":{"version":"1.0,0"},
	"attributes": {"hello":12},
	"layers": [
		"a", "b", "c"
	]
}
)str");
	must_fail(mp.get_errors(), "layer must be an object, cannot locate meta, skipping layer meta", "layers node whose member is not an object");

	//tiles node with no meta
	mp.parse_string(R"str(
{
	"meta":{"version":"1.0,0"},
	"attributes": {"hello":12},
	"layers": [
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
	"layers": [
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
	"layers": [
	{
			"meta": {
				"no-type": "hehe"
			}
		}
	]
}
)str");
	must_fail(mp.get_errors(), "meta node in layer has no 'type' member, a default will be used", "node with no type");

	//tiles node with no bad type
	mp.parse_string(R"str(
{
	"meta":{"version":"1.0,0"},
	"attributes": {"hello":12},
	"layers": [
	{
			"meta": {
				"type":"lol"
			}
		}
	]
}
)str");
	must_fail(mp.get_errors(), "unkown meta node type 'lol', cannot be parsed", "layer node with bad type");

	//tiles node with invalid alpha
	mp.parse_string(R"str(
{
	"meta":{"version":"1.0,0"},
	"attributes": {"hello":12},
	"layers": [
		{
			"meta": {
				"type":"tiles",
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
	"layers": [
		{
			"meta": {
				"type":"tiles",
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
	"layers": [
		{
			"meta": {
				"type":"tiles",
				"alpha": 128,
				"set" : 12.44
			}
		}
	]
}
)str");
	must_fail(mp.get_errors(), "meta:set node is not an integer", "tiles node with invalid set");

	//tiles node with no id
	mp.parse_string(R"str(
{
	"meta":{"version":"1.0,0"},
	"attributes": {"hello":12},
	"layers": [
		{
			"meta": {
				"type":"tiles",
				"alpha": 128,
				"set" : 1
			}
		}
	]
}
)str");
	must_fail(mp.get_errors(), "meta node in layer has no 'id' member, a default will be used", "tiles node with no id.");


	//tiles node with invalid id
	mp.parse_string(R"str(
{
	"meta":{"version":"1.0,0"},
	"attributes": {"hello":12},
	"layers": [
		{
			"meta": {
				"type":"tiles",
				"alpha": 128,
				"set" : 1,
				"id": 12
			}
		}
	]
}
)str");
	must_fail(mp.get_errors(), "meta:id node is not a string, a default will be used", "tiles node with no id.");

	//tiles node with extraneous meta members.
	mp.parse_string(R"str(
{
	"meta":{"version":"1.0,0"},
	"attributes": {"hello":12},
	"layers": [
		{
			"meta": {
				"type":"tiles",
				"alpha": 128,
				"set" : 1,
				"id":"lol",
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
	"layers": [
		{
			"meta": {
				"type":"tiles",
				"alpha": 128,
				"set" : 1,
				"id":"lol"
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
	"layers": [
		{
			"meta": {
				"type":"tiles",
				"alpha": 128,
				"set" : 1,
				"id":"lol"
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
	"layers": [
		{
			"meta": {
				"type":"tiles",
				"alpha": 128,
				"set" : 1,
				"id":"lol"
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
	"layers": [
		{
			"meta": {
				"type":"tiles",
				"alpha": 128,
				"set" : 1,
				"id":"lol"
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
	"layers": [
		{
			"meta": {
				"type":"tiles",
				"alpha": 128,
				"set" : 1,
				"id":"lol"
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
	"layers": [
		{
			"meta": {
				"type":"tiles",
				"alpha": 128,
				"set" : 1,
				"id":"lol"
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
	"layers": [
		{
			"meta": {
				"type":"tiles",
				"alpha": 128,
				"set" : 1,
				"id":"lol"
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
	"layers": [
		{
			"meta": {
				"type":"tiles",
				"alpha": 128,
				"set" : 1,
				"id":"lol"
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
	"layers": [
		{
			"meta": {
				"type":"tiles",
				"alpha": 128,
				"set" : 1,
				"id":"lol"
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
	"layers": [
		{
			"meta": {
				"type":"tiles",
				"alpha": 128,
				"set" : 1,
				"id":"lol"
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
	"layers": [
		{
			"meta": {
				"type":"tiles",
				"alpha": 128,
				"set" : 1,
				"id":"lol"
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
	"layers": [
		{
			"meta": {
				"type":"tiles",
				"alpha": 128,
				"set" : 1,
				"id":"lol"
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

	//not t member in thing item
	mp.parse_string(R"str(
{
	"meta":{"version":"1.0,0"},
	"attributes": {"hello":12},
	"layers": [
		{
			"meta":{
				"type":"things",
				"set":1,
				"alpha":0,
				"id":"lol"
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
	"layers": [
		{
			"meta":{
				"type":"things",
				"set":1,
				"alpha":0,
				"id":"lol"
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
	"layers": [
		{
			"meta":{
				"type":"things",
				"set":1,
				"alpha":0,
				"id":"lol"
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
	"layers": [
		{
			"meta":{
				"type":"things",
				"set":1,
				"alpha":0,
				"id":"lol"
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
	"layers": [
		{
			"meta":{
				"type":"things",
				"set":1,
				"alpha":0,
				"id":"lol"
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
	"layers": [
		{
			"meta":{
				"type":"things",
				"set":1,
				"alpha":0,
				"id":"lol"
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
	"layers": [
		{
			"meta":{
				"type":"things",
				"set":1,
				"alpha":0,
				"id":"lol"
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
	"layers": [
		{
			"meta":{
				"type":"things",
				"set":1,
				"alpha":0,
				"id":"lol"
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
	"layers": [
		{
			"meta":{
				"type":"things",
				"set":1,
				"alpha":0,
				"id":"lol"
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
	"layers": [
		{
			"meta":{
				"type":"things",
				"set":1,
				"alpha":0,
				"id":"lol"
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
	"layers": [
		{
			"meta":{
				"type":"things",
				"set":1,
				"alpha":0,
				"id":"lol"
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

	//Non object poly item
	mp.parse_string(R"str(
{
	"meta":{"version":"1.0,0"},
	"attributes": {"hello":12},
	"layers": [
		{
			"meta":{"type":"polys", "id":"lol", "set":1, "alpha":0},
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
	"layers": [
		{
			"meta":{"type":"polys", "id":"lol", "set":1, "alpha":0},
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
	"layers": [
		{
			"meta":{"type":"polys", "id":"lol", "set":1, "alpha":0},
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
	"layers": [
		{
			"meta":{"type":"polys", "id":"lol", "set":1, "alpha":0},
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
	"layers": [
		{
			"meta":{"type":"polys", "id":"lol", "set":1, "alpha":0},
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
	"layers": [
		{
			"meta":{"type":"polys", "id":"lol", "set":1, "alpha":0},
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
	"layers": [
		{
			"meta":{"type":"polys", "id":"lol", "set":1, "alpha":0},
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
	"layers": [
		{
			"meta":{"type":"polys", "id":"lol", "set":1, "alpha":0},
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
	"layers": [
		{
			"meta":{"type":"polys", "id":"lol", "set":1, "alpha":0},
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
	"layers": [
		{
			"meta":{"type":"polys", "id":"lol", "set":1, "alpha":0},
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
	"layers": [
		{
			"meta":{"type":"polys", "id":"lol", "set":1, "alpha":0},
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
	"layers": [
		{
			"meta":{"type":"polys", "id":"lol", "set":1, "alpha":0},
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
	"layers": [
		{
			"meta":{"type":"polys", "id":"lol", "set":1, "alpha":0},
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
	"layers": [
		{
			"meta":{"type":"polys", "id":"lol", "set":1, "alpha":0},
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

/*
OK tile item has no 'p' property, skipping item
OK missing meta node in layer, skipping layer meta
????
OK thing item has no 'p' property, skipping item
OK missing meta node in layer, skipping layer meta
????
OK poly item has no 'p' property, skipping poly
OK missing meta node in layer, skipping layer meta
???
*/

		std::size_t errindex=0;
		test("tile item has no 'p' property, skipping item"==errors[errindex++], "invalid error 0");
		test("missing meta node in layer, skipping layer meta"==errors[errindex++], "invalid error 1");
//		test("missing data in tile layer, skipping layer"==errors[errindex++], "invalid error 2");
		test("thing item has no 'p' property, skipping item"==errors[errindex++], "invalid error 3");
		test("missing meta node in layer, skipping layer meta"==errors[errindex++], "invalid error 4");
//		test("missing data in thing layer, skipping layer"==errors[errindex++], "invalid error 5");
		test("poly item has no 'p' property, skipping poly"==errors[errindex++], "invalid error 6");
		test("missing meta node in layer, skipping layer meta"==errors[errindex++], "invalid error 7");
//		test("missing data in poly layer, skipping layer"==errors[errindex++], "invalid error 8");

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

		test(6==map.layers.size(), "invalid parsing of layer size");

		check_layer(*map.layers[0], 1, 0, 2, "first", __LINE__);
		check_tile(*map.layers[0], 0, 1, 2, 3, __LINE__);
		check_tile(*map.layers[0], 1, 2, 4, 5, __LINE__);

		check_layer(*map.layers[1], 2, 128, 1, "second", __LINE__);
		check_tile(*map.layers[1], 0, 3, 10, 11, __LINE__);

		check_layer(*map.layers[2], 3, 32, 2, "third", __LINE__);

		check_thing(*map.layers[2], 0, 1, 10, 11, 3, __LINE__);
		check_component_attribute(*map.layers[2], 0, "some_attribute", 1, __LINE__);
		check_component_attribute(*map.layers[2], 0, "some_other_attribute", 2.2, __LINE__);
		check_component_attribute(*map.layers[2], 0, "and_another_one", "yes", __LINE__);

		check_thing(*map.layers[2], 1, 2, 14, 15, 1, __LINE__);
		check_component_attribute(*map.layers[2], 1, "an_attribute", 2, __LINE__);

		check_layer(*map.layers[3], 4, 64, 1, "fourth", __LINE__);
		check_thing(*map.layers[3], 0, 3, 16, 17, 0, __LINE__);

		check_layer(*map.layers[4], 4, 64, 3, "fifth", __LINE__);

		check_poly(*map.layers[4], 0, 1, {{10,11}, {12,13}, {14,15}, {16,17}}, 3, __LINE__);
		check_component_attribute(*map.layers[4], 0, "some_attribute", 2, __LINE__);
		check_component_attribute(*map.layers[4], 0, "some_other_attribute", 3.2, __LINE__);
		check_component_attribute(*map.layers[4], 0, "and_another_one", "no", __LINE__);

		check_poly(*map.layers[4], 1, 2, {{18,19}, {20,21}, {22,23}, {24,25}}, 1, __LINE__);
		check_component_attribute(*map.layers[4], 1, "an_attribute", 3, __LINE__);

		check_poly(*map.layers[4], 2, 2, {{118,119}, {120,121}, {122,123}, {124,125}}, 0, __LINE__);

		check_layer(*map.layers[5], 5, 128, 1, "sixth", __LINE__);
		check_poly(*map.layers[5], 0, 3, {{16,17}, {18,19}, {20,21}}, 0, __LINE__);
	}
	catch(std::exception& e) {

		std::cerr<<"failed: "<<e.what()<<std::endl;
		std::exit(1);
	}

	try {

		std::cout<<"testing fully valid map file"<<std::endl;
		auto map=mp.parse_file("data/good.map");
		const auto& errors=mp.get_errors();

		test(errors.size()==0, "there were unexpected errors parsing the good map");
		test(mp.get_version()=="1.0.0", "could not assert the map version");

		test("entryway"==map.properties.string_properties["name"], "invalid name property");
		test(1==map.properties.int_properties["episode"], "invalid episode property");
		test(1==map.properties.int_properties["map"], "invalid map property");

		check_layer(*map.layers[0], 1, 0, 4, "background", __LINE__);
		check_tile(*map.layers[0], 0, 1, 0, 0, __LINE__);
		check_tile(*map.layers[0], 1, 1, 0, 1, __LINE__);
		check_tile(*map.layers[0], 2, 1, 0, 2, __LINE__);
		check_tile(*map.layers[0], 3, 2, 1, 1, __LINE__);

		check_layer(*map.layers[1], 2, 128, 3, "foreground", __LINE__);
		check_tile(*map.layers[1], 0, 1, 1, 0, __LINE__);
		check_tile(*map.layers[1], 1, 1, 1, 1, __LINE__);
		check_tile(*map.layers[1], 2, 1, 1, 2, __LINE__);

		check_layer(*map.layers[2], 1, 128, 3, "objects", __LINE__);

		check_thing(*map.layers[2], 0, 1, 32, 32, 2, __LINE__);
		check_component_attribute(*map.layers[2], 0, "entry_id", 1, __LINE__);
		check_component_attribute(*map.layers[2], 0, "bearing", 90, __LINE__);

		check_thing(*map.layers[2], 1, 2, 128, 32, 4, __LINE__);
		check_component_attribute(*map.layers[2], 1, "w", 16, __LINE__);
		check_component_attribute(*map.layers[2], 1, "h", 32, __LINE__);
		check_component_attribute(*map.layers[2], 1, "map_id", 2, __LINE__);
		check_component_attribute(*map.layers[2], 1, "entry_id", 1, __LINE__);

		check_thing(*map.layers[2], 2, 2, 32, 128, 4, __LINE__);
		check_component_attribute(*map.layers[2], 2, "w", 32, __LINE__);
		check_component_attribute(*map.layers[2], 2, "h", 16, __LINE__);
		check_component_attribute(*map.layers[2], 2, "map_id", 3, __LINE__);
		check_component_attribute(*map.layers[2], 2, "entry_id", 1, __LINE__);

		check_layer(*map.layers[3], 1, 128, 1, "polygons", __LINE__);

		check_poly(*map.layers[3], 0, 1, {{32,32}, {64,32}, {64,128}, {32,128}}, 1, __LINE__);
		check_component_attribute(*map.layers[3], 0, "path_speed", 2.1, __LINE__);

	}
	catch(std::exception& e) {

		std::cerr<<"failed: "<<e.what()<<std::endl;
		std::exit(1);
	}

	std::cout<<"all good"<<std::endl;

	return 0;
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

void check_tile(
	const tile_editor::layer& _layer,
	std::size_t _index, 
	std::size_t _type, 
	int _x, 
	int _y, 
	int _line
) {

	struct visitor:tile_editor::const_layer_visitor {

		int				x, y, line;
		std::size_t		index, type;

		void            visit(const tile_editor::tile_layer& _layer) {

			const tile_editor::tile& tile=_layer.data[index];

			test(type==tile.type, std::string{"invalid type for tile "}+std::to_string(line)+" got "+std::to_string(tile.type)+" expected "+std::to_string(type));
			test(x==tile.x, std::string{"invalid x for tile "}+std::to_string(line)+" got "+std::to_string(tile.x)+" expected "+std::to_string(x));
			test(y==tile.y, std::string{"invalid t for tile "}+std::to_string(line)+" got "+std::to_string(tile.y)+" expected "+std::to_string(y));

		}
		void            visit(const tile_editor::thing_layer&) {
			test(false, std::string{"invalid layer, expected tile, got thing in line "}+std::to_string(line));
		}
		void            visit(const tile_editor::poly_layer&) {
			test(false, std::string{"invalid layer, expected tile, got poly in line "}+std::to_string(line));
		}
	} visitor;

	visitor.x=_x;
	visitor.y=_y;
	visitor.line=_line;
	visitor.type=_type;
	visitor.index=_index;
	_layer.accept(visitor);
}

void check_thing(
	const tile_editor::layer& _layer, 
	std::size_t _index,
	std::size_t _type,
	int _x,
	int _y,
	std::size_t _propcount,
	int _line
) {

	struct visitor:tile_editor::const_layer_visitor {

		int				x, y, line;
		std::size_t		index, type, propcount;

		const int   default_w=1,
		            default_h=1,
		            default_r=128,
		            default_g=128,
		            default_b=128,
		            default_a=128;

		void            visit(const tile_editor::tile_layer&) {
			test(false, std::string{"invalid layer, expected thing, got tile in line "}+std::to_string(line));
		}

		void            visit(const tile_editor::thing_layer& _layer) {

			const tile_editor::thing& _thing=_layer.data[index];

			test(type==_thing.type, std::string{"invalid type for thing "}+std::to_string(line)+" got "+std::to_string(_thing.type)+" expected "+std::to_string(type));
			test(x==_thing.x, std::string{"invalid x for thing "}+std::to_string(line)+" got "+std::to_string(_thing.x)+" expected "+std::to_string(x));
			test(y==_thing.y, std::string{"invalid t for thing "}+std::to_string(line)+" got "+std::to_string(_thing.y)+" expected "+std::to_string(y));
			test(propcount==_thing.properties.size(), std::string{"invalid property size for thing "}+std::to_string(line)+" got "+std::to_string(_thing.properties.size())+" expected "+std::to_string(propcount));
			test(default_w==_thing.w, std::string{"invalid default w for thing "}+std::to_string(line)+" got "+std::to_string(_thing.w)+" expected "+std::to_string(default_w));
			test(default_h==_thing.h, std::string{"invalid default h for thing "}+std::to_string(line)+" got "+std::to_string(_thing.h)+" expected "+std::to_string(default_h));
			test(default_r==_thing.color.r, std::string{"invalid default color r for thing "}+std::to_string(line)+" got "+std::to_string(_thing.color.r)+" expected "+std::to_string(default_r));
			test(default_g==_thing.color.g, std::string{"invalid default color g for thing "}+std::to_string(line)+" got "+std::to_string(_thing.color.g)+" expected "+std::to_string(default_g));
			test(default_b==_thing.color.b, std::string{"invalid default color b for thing "}+std::to_string(line)+" got "+std::to_string(_thing.color.b)+" expected "+std::to_string(default_b));
			test(default_a==_thing.color.a, std::string{"invalid default color a for thing "}+std::to_string(line)+" got "+std::to_string(_thing.color.a)+" expected "+std::to_string(default_a));
		}

		void            visit(const tile_editor::poly_layer&) {
			test(false, std::string{"invalid layer, expected thing, got poly in line "}+std::to_string(line));
		}
	} visitor;

	visitor.x=_x;
	visitor.y=_y;
	visitor.line=_line;
	visitor.type=_type;
	visitor.propcount=_propcount;
	visitor.index=_index;
	_layer.accept(visitor);
}

void check_poly(
	const tile_editor::layer& _layer, 
	std::size_t _index,
	std::size_t _type,
	const std::vector<tile_editor::poly_point>& _points,
	std::size_t _propcount,
	int _line
) {

	struct visitor:tile_editor::const_layer_visitor {

		int line;
		std::size_t index;
		const tile_editor::poly* poly{nullptr};

		void            visit(const tile_editor::tile_layer&) {
			test(false, std::string{"invalid layer, expected poly, got tile in line "}+std::to_string(line));
		}

		void            visit(const tile_editor::thing_layer&) {

			test(false, std::string{"invalid layer, expected poly, got thing in line "}+std::to_string(line));
		}

		void            visit(const tile_editor::poly_layer& _layer) {

			poly=&_layer.data[index];

		}
	} visitor;

	visitor.line=_line;
	visitor.index=_index;
	_layer.accept(visitor);

	const tile_editor::poly& _poly{*visitor.poly};

	test(_type==_poly.type, std::string{"invalid type for poly in line "}+std::to_string(_line)+" got "+std::to_string(_poly.type)+" expected "+std::to_string(_type));
	test(_propcount==_poly.properties.size(), std::string{"invalid property size for poly "}+std::to_string(_line)+" got "+std::to_string(_poly.properties.size())+" expected "+std::to_string(_propcount));
	test(_points == _poly.points, std::string{"invalid points for poly in line "}+std::to_string(_line));
}

void check_layer(
	const tile_editor::layer& _layer, 
	std::size_t _set, 
	int _alpha, 
	std::size_t _count, 
	const std::string _id,
	int _line
) {

	test(_set==_layer.set, std::string{"invalid set value in line "}+std::to_string(_line)+" got "+std::to_string(_layer.set)+" expected "+std::to_string(_set));
	test(_alpha==_layer.alpha, std::string{"invalid alpha value in line "}+std::to_string(_line)+" got "+std::to_string(_layer.alpha)+" expected "+std::to_string(_alpha));
	test(_id==_layer.id, std::string{"invalid id value in line "}+std::to_string(_line)+" got "+_layer.id+" expected "+_id);

	struct size_visitor:tile_editor::const_layer_visitor {

		std::size_t                count;
		int                        line;
		void                       visit(const tile_editor::tile_layer& _layer) {check(_layer.data.size());}
		void                       visit(const tile_editor::thing_layer& _layer) {check(_layer.data.size());}
		void                       visit(const tile_editor::poly_layer& _layer) {check(_layer.data.size());}

		void                       check(std::size_t _size) {

			test(count==_size, std::string{"invalid item count in line "}+std::to_string(line)+" got "+std::to_string(_size)+" expected "+std::to_string(count));
		}
	} visitor;

	visitor.count=_count;
	visitor.line=_line;
	_layer.accept(visitor);
}

struct properties_visitor:public tile_editor::const_layer_visitor {

	std::size_t                           index;
	const tile_editor::property_manager * pm;

	void                       visit(const tile_editor::tile_layer&) {
		
		throw std::runtime_error("tile layer had no properties");
	}

	void                       visit(const tile_editor::thing_layer& _layer) {

		pm=&_layer.data[index].properties;
	}

	void                       visit(const tile_editor::poly_layer& _layer) {

		pm=&_layer.data[index].properties;
	}
};

void check_component_attribute(
	const tile_editor::layer& _subject,
	std::size_t _index,
	const std::string& _key,
	int _value,
	int _line
) {

	properties_visitor visitor;
	visitor.index=_index;
	_subject.accept(visitor);

	test(visitor.pm->has_property(_key), std::string{"property does not exist on line "}+std::to_string(_line));
	test(visitor.pm->int_properties.count(_key), std::string{"property is not an int on line "}+std::to_string(_line));

	auto val=visitor.pm->int_properties.at(_key);
	test(val==_value, std::string{"invalid integer property value, got "}+std::to_string(val)+", expected "+std::to_string(_value)+" on line "+std::to_string(_line));
}

void check_component_attribute(
	const tile_editor::layer& _subject,
	std::size_t _index,
	const std::string& _key,
	double _value,
	int _line
) {

	properties_visitor visitor;
	visitor.index=_index;
	_subject.accept(visitor);

	test(visitor.pm->has_property(_key), std::string{"property does not exist on line "}+std::to_string(_line));
	test(visitor.pm->double_properties.count(_key), std::string{"property is not a double on line "}+std::to_string(_line));

	auto val=visitor.pm->double_properties.at(_key);
	test(val==_value, std::string{"invalid double property value, got "}+std::to_string(val)+", expected "+std::to_string(_value)+" on line "+std::to_string(_line));
}

void check_component_attribute(
	const tile_editor::layer& _subject,
	std::size_t _index,
	const std::string& _key,
	const std::string& _value,
	int _line
) {

	properties_visitor visitor;
	visitor.index=_index;
	_subject.accept(visitor);

	test(visitor.pm->has_property(_key), std::string{"property does not exist on line "}+std::to_string(_line));
	test(visitor.pm->string_properties.count(_key), std::string{"property is not a string on line "}+std::to_string(_line));

	auto val=visitor.pm->string_properties.at(_key);
	test(val==_value, std::string{"invalid string property value, got "}+val+", expected "+_value+" on line "+std::to_string(_line));
}

