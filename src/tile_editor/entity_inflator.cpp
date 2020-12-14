#include "app/entity_inflator.h"
#include "editor_types/thing.h"
#include "editor_types/poly.h"
#include "blueprint_types/thing_definition.h"
#include "blueprint_types/poly_definition.h"

using namespace app;

void entity_inflator::inflate(
	tile_editor::thing& _thing,
	const tile_editor::thing_definition& _blueprint
) {

	_thing.w=_blueprint.w;
	_thing.h=_blueprint.h;
	_thing.color=_blueprint.color;

	for(const auto& prop : _blueprint.properties.int_properties) {

		switch(prop.second.linked_to) {

			case tile_editor::property_links::w:
				_thing.w=_thing.properties.int_properties.at(prop.second.name); break;
			case tile_editor::property_links::h:
				_thing.h=_thing.properties.int_properties.at(prop.second.name); break;
			case tile_editor::property_links::color_red:
				_thing.color.r=_thing.properties.int_properties.at(prop.second.name); break;
			case tile_editor::property_links::color_green:
				_thing.color.g=_thing.properties.int_properties.at(prop.second.name); break;
			case tile_editor::property_links::color_blue:
				_thing.color.b=_thing.properties.int_properties.at(prop.second.name); break;
			case tile_editor::property_links::color_alpha:
				_thing.color.a=_thing.properties.int_properties.at(prop.second.name); break;
			case tile_editor::property_links::nothing: break;
		}
	}
}

void entity_inflator::inflate(
	tile_editor::poly& _poly,
	const tile_editor::poly_definition& _blueprint) {

	_poly.color=_blueprint.color;

	for(const auto& prop : _blueprint.properties.int_properties) {

		switch(prop.second.linked_to) {

			case tile_editor::property_links::color_red:
				_poly.color.r=_poly.properties.int_properties.at(prop.second.name); break;
			case tile_editor::property_links::color_green:
				_poly.color.g=_poly.properties.int_properties.at(prop.second.name); break;
			case tile_editor::property_links::color_blue:
				_poly.color.b=_poly.properties.int_properties.at(prop.second.name); break;
			case tile_editor::property_links::color_alpha:
				_poly.color.a=_poly.properties.int_properties.at(prop.second.name); break;
			case tile_editor::property_links::w:
			case tile_editor::property_links::h:
			case tile_editor::property_links::nothing: break;
		}
	}
}

