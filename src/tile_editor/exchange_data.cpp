#include "app/exchange_data.h"

#include <stdexcept>

using namespace tile_editor;

exchange_data::exchange_data() {

	for(int i=controller::state_min; i<controller::state_max; i++) {

		controller_marks[i]=false;
	}
}

void exchange_data::put(int _controller_index) {

	if(controller_marks[_controller_index]) {

		throw std::runtime_error("controller was already marked!");
	}

	controller_marks[_controller_index]=true;
}

void exchange_data::recover(int _controller_index) {

	if(!controller_marks[_controller_index]) {

		throw std::runtime_error("controller was not marked!");
	}

	controller_marks[_controller_index]=false;
}
