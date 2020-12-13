#include "../../include/controller/properties.h"

//local
#include "../../include/input/input.h"

using namespace controller;

properties::properties(
	lm::logger& _log,
	ldtools::ttf_manager& _ttf_manager,
	tile_editor::exchange_data& _exchange_data
)
	:log{_log},
	ttf_manager{_ttf_manager},
	exchange_data{_exchange_data} {

}

void properties::awake(dfw::input& /*input*/) {

	if(!exchange_data.has(state_properties)) {

		throw std::runtime_error("could not find data for state_properties");
	}

	exchange_data.recover(state_properties);
	property_manager=exchange_data.properties;
		
	//TODO: Likely we have some kind of menu here...
	//TODO: load whatever kind of container we have here.

	//TODO: Add the cancel and return options to the menu.
}

void properties::slumber(dfw::input& /*input*/) {

	property_manager=nullptr;
}

void properties::loop(dfw::input& _input, const dfw::loop_iteration_data& /*lid*/) {

	if(_input().is_exit_signal()) {

		set_leave(true);
		return;
	}

	if( _input.is_input_down(input::escape)) {

		pop_state();
	}
}

void properties::draw(ldv::screen& screen, int /*fps*/) {

	screen.clear(ldv::rgba8(0, 0, 0, 255));

	//TODO: Always draw the description for the current thing!
}
