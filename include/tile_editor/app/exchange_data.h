
#pragma once

#include "controller/states.h"

#include <string>
#include <map>

namespace tile_editor {

struct layer;
struct map_blueprint;
struct property_manager;

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
	std::string             file_browser_choice, //out
	                        file_browser_title; //in
	tile_editor::layer *    layer{nullptr}; //in-out
	tile_editor::property_manager * properties; //in-out
	const tile_editor::map_blueprint * blueprint{nullptr}; //in

	private:

	std::map<int, int>      controller_marks;
};

}
