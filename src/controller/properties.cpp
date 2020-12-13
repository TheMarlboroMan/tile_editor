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

	menu.set_wrap(false);
}

void properties::awake(dfw::input& /*input*/) {

	if(!exchange_data.has(state_properties)) {

		throw std::runtime_error("could not find data for state_properties");
	}

	exchange_data.recover(state_properties);
	property_manager=exchange_data.properties;


	bool key_set=false;

	auto add_to_menu=[&key_set, this](
		const auto& _pair,
		datatypes _type
	) {
		if(!key_set) {

			current_key={_type, _pair.first};
			key_set=true;
		}

		menu.insert(keytype{_type, _pair.first}, _pair.second);
	};

	auto add=[add_to_menu](
		const auto& _container,
		datatypes _type
	) {
		for(const auto& pair : _container) {
			add_to_menu(pair, _type);
		}
	};

	menu.clear();

	//TODO: ints need min-max.
//	add(property_manager->int_properties, datatypes::t_int);
	add(property_manager->string_properties, datatypes::t_string);
	//TODO: doubles need min-max.
//	add(property_manager->double_properties, datatypes::t_double);
	menu.insert(keytype{datatypes::t_special, "exit"});
	menu.insert(keytype{datatypes::t_special, "cancel"});
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

	//run through the menu and draw.

	//TODO: Always draw the description for the current thing!
}
