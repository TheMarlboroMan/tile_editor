
#pragma once

#include "controller/states.h"

#include <string>
#include <map>

namespace tile_editor {

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
	int                     file_browser_invoker_id{0}; //int
	std::string             file_browser_choice, //out
							file_browser_title; //int

	private:

	std::map<int, int>      controller_marks;
};

}
