#include "app/map_loader.h"
#include "parser/map_parser.h"
#include "editor_types/thing_layer.h"
#include "editor_types/poly_layer.h"
#include "editor_types/tile_layer.h"

#include <lm/sentry.h>

//TODO:
#include <iostream>

using namespace tile_editor;

map_loader::map_loader(
	lm::logger& _logger,
	tools::message_manager& _message_manager,
	const std::map<std::size_t, thing_definition_table>& _thingsets,
	const std::map<std::size_t, poly_definition_table>& _polysets
):
	log(_logger),
	message_manager{_message_manager},
	thingsets{_thingsets},
	polysets{_polysets} {

}

tile_editor::map map_loader::load_from_file(const std::string& _path) {

	tile_editor::map_parser mp;
	tile_editor::map result=mp.parse_file(_path);

	inflate_properties(result);

	for(const auto& msg : mp.get_errors()) {

		lm::log(log, lm::lvl::notice)<<msg<<std::endl;
		message_manager.add(msg);
	}

	if(mp.get_errors().size()) {

		message_manager.add("there were errors loading the map, please check the log file");
	}
	else {

		std::string msg=std::string{"loaded map "}
			+_path+
			+"with "
			+std::to_string(result.layers.size())+" layers and "
			+std::to_string(result.properties.size())+" properties";

		lm::log(log, lm::lvl::info)<<msg<<std::endl;

		message_manager.add(msg);
	}

	return result;
}

void map_loader::inflate_properties(tile_editor::map& _map) {

	//Collector of pointers...
	struct :layer_visitor {

		std::vector<tile_editor::thing_layer*> things;
		std::vector<tile_editor::poly_layer*> polys;

		void visit(tile_editor::tile_layer&) {}
		void visit(tile_editor::thing_layer& _layer) {
			things.push_back(&_layer);
		}
		void visit(tile_editor::poly_layer& _layer) {
			polys.push_back(&_layer);
		}
	} visitor;

	for(auto& layer : _map.layers) {
		layer->accept(visitor);
	}
	
	//inflate things...
	for(auto& ptr : visitor.things) {

		for(auto& thing : ptr->data) {

			const auto blueprint=thingsets.at(ptr->set).table.at(thing.type);
			thing.w=blueprint.w;
			thing.h=blueprint.h;
			thing.color=blueprint.color;
		
			for(const auto& prop : blueprint.properties.int_properties) {

				if(!thing.properties.int_properties.count(prop.second.name)) {

					//TODO: What if the properties are removed from the file????				
				}

				switch(prop.second.linked_to) {

					case tile_editor::property_links::w:
						thing.w=thing.properties.int_properties.at(prop.second.name); break;
					case tile_editor::property_links::h:
						thing.h=thing.properties.int_properties.at(prop.second.name); break;
					case tile_editor::property_links::color_red:
						thing.color.r=thing.properties.int_properties.at(prop.second.name); break;
					case tile_editor::property_links::color_green:
						thing.color.g=thing.properties.int_properties.at(prop.second.name); break;
					case tile_editor::property_links::color_blue:
						thing.color.b=thing.properties.int_properties.at(prop.second.name); break;
					case tile_editor::property_links::color_alpha:
						thing.color.a=thing.properties.int_properties.at(prop.second.name); break;
					case tile_editor::property_links::nothing: break;
				}
			}
		}
	}

	//inflate polys.
	for(auto& ptr : visitor.polys) {

		for(auto& poly : ptr->data) {

			const auto blueprint=polysets.at(ptr->set).table.at(poly.type);
			poly.color=blueprint.color;
		
			for(const auto& prop : blueprint.properties.int_properties) {

				if(!poly.properties.int_properties.count(prop.second.name)) {

					//TODO: What if the properties are removed from the file????				
				}

				switch(prop.second.linked_to) {

					case tile_editor::property_links::color_red:
						poly.color.r=poly.properties.int_properties.at(prop.second.name); break;
					case tile_editor::property_links::color_green:
						poly.color.g=poly.properties.int_properties.at(prop.second.name); break;
					case tile_editor::property_links::color_blue:
						poly.color.b=poly.properties.int_properties.at(prop.second.name); break;
					case tile_editor::property_links::color_alpha:
						poly.color.a=poly.properties.int_properties.at(prop.second.name); break;
					case tile_editor::property_links::w:
					case tile_editor::property_links::h:
					case tile_editor::property_links::nothing: break;
				}
			}
		}
	}
}
