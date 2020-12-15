
#pragma once

#include "controller/states.h"

#include <string>
#include <map>

namespace tile_editor {

struct layer;
struct map_blueprint;
struct property_manager;
struct property_table;
struct thing_definition;
struct poly_definition;
struct thing;
struct poly;
struct map;

class exchange_data {

	public:

	                        exchange_data();

	//! Returns true if a mark has been left for a controller.
	bool                    has(int _controller_index) const {return controller_marks.at(_controller_index);}

	//! Marks a controller so it can recover data on wakeup.
	void                    put(int);
	//! Unmarks a controller when data is recovered on wakeup.
	void                    recover(int);

	bool                    file_browser_allow_create{false}, //in
	                        file_browser_success{false}; //out
	int                     file_browser_invoker_id{0}; //in
	std::size_t *           current_layer; //in-out
	std::string             file_browser_choice, //out
	                        file_browser_title; //in
	tile_editor::layer *    layer{nullptr}; //in-out
	tile_editor::property_manager * properties{nullptr}; //in-out
	tile_editor::property_table * properties_blueprint{nullptr}; //in
	const tile_editor::map_blueprint * blueprint{nullptr}; //in
	tile_editor::map *      map{nullptr}; //in-out
	tile_editor::thing *    edited_thing{nullptr}; //in
	tile_editor::thing_definition * edited_thing_blueprint{nullptr}; //in
	tile_editor::poly *    edited_poly{nullptr}; //in
	tile_editor::poly_definition * edited_poly_blueprint{nullptr}; //in

	private:

	std::map<int, int>      controller_marks;
};

}
