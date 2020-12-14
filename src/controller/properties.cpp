#include "controller/properties.h"
#include "input/input.h"

#include <ldv/ttf_representation.h>

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
	property_manager=*exchange_data.properties;
	blueprint=exchange_data.properties_blueprint;

	//Each entry will be assigned a numeric key. A separate vector will keep
	//track of the relationship between each key and the name-type of the
	//property for lookup purposes.
	auto add=[this](const auto& _props, option_types _type) {

		for(const auto& pair : _props) {
			lookup.push_back({_type, pair.first});
		}
	};

	lookup.clear();
	add(property_manager.string_properties, option_types::str);
	add(property_manager.int_properties, option_types::integer);
	add(property_manager.double_properties, option_types::decimal);

	current_index=0;
	exit_index=lookup.size();
	cancel_index=lookup.size()+1;
}

void properties::slumber(dfw::input& /*input*/) {

	exchange_data.properties=nullptr;
	exchange_data.properties_blueprint=nullptr;
}

void properties::loop(dfw::input& _input, const dfw::loop_iteration_data& /*lid*/) {

	if(_input().is_exit_signal()) {

		set_leave(true);
		return;
	}

	text_mode
		? input_text(_input)
		: input_traverse(_input);
}

void properties::input_traverse(dfw::input& _input) {

 	if(_input.is_input_down(input::escape)) {

		pop_state();
		return;
	}

	if(_input.is_input_down(input::down)) {
		if(current_index < cancel_index) {
			++current_index;
		}
		return;
	}
	else if(_input.is_input_down(input::up)) {

		if(current_index > 0) {
			--current_index;
		}
		return;
	}

	if(_input.is_input_down(input::enter)) {

		if(current_index==cancel_index) {
			pop_state();
			return;
		}

		if(current_index==exit_index) {
			save_changes();
			pop_state();
			return;
		}


		text_mode=true;
		current_value.clear();
		_input().start_text_input();

		auto is_numeric=[](const SDL_Event& _event) -> bool {
			try {
				int val=std::stoi(_event.text.text);
				return val >= 0 && val <= 9;
			}
			catch(std::invalid_argument&) {
				return false;
			}
		};

		//Set the filter...
		auto lookup_data=lookup.at(current_index);
		switch(lookup_data.type) {
			case option_types::str:

			break;
			case option_types::integer:
				_input().set_text_filter(is_numeric);
			break;
			case option_types::decimal:
				_input().set_text_filter([is_numeric](const SDL_Event& _event) -> bool {

					std::string txt{_event.text.text}; //_event.text.text is const char *, with no ==
					if(txt==".") {
						return true;
					}

					return is_numeric(_event);
				});
			break;
		}
		return;
	}
}

void properties::input_text(dfw::input& _input) {

	auto done=[&_input, this]() {

		_input().clear_text_filter();
		_input().stop_text_input();
		_input().clear_text_input();
		current_value="";
		text_mode=false;
	};

	if(_input.is_input_down(input::escape)) {

		done();
		return;
	}

	if(_input.is_input_down(input::enter)) {

		auto lookup_data=lookup.at(current_index);
		switch(lookup_data.type) {
			case option_types::str:
				property_manager.string_properties.at(lookup_data.key)=current_value;
			break;
			case option_types::integer:
				property_manager.int_properties.at(lookup_data.key)=std::stoi(current_value);
			break;
			case option_types::decimal:
				property_manager.double_properties.at(lookup_data.key)=std::stod(current_value);
			break;
		}

		done();
		return;
	}

	if(_input.is_input_down(input::backspace)) {

		if(current_value.length()) {
			current_value.pop_back();
		}

		_input().clear_text_input();
		return;
	}

	if(_input().is_text_input()) {

		current_value+=_input().get_text_input();
		_input().clear_text_input();
	}
}


void properties::draw(ldv::screen& _screen, int /*fps*/) {

	properties::printer pr{current_index, current_value, text_mode, std::stringstream{}};
	_screen.clear(ldv::rgba8(0, 0, 0, 255));

	const auto max=cancel_index+1; //Just alias these as const.

	for(int i=0; i<max; i++) {

		if(i==exit_index) {
			pr.special("exit", i);
		}
		else if(i==cancel_index) {
			pr.special("cancel", i);
		}
		else {
			auto lookup_data=lookup.at(i);
			switch(lookup_data.type) {
				case option_types::str:
					pr.print(blueprint->string_properties.at(lookup_data.key), property_manager.string_properties.at(lookup_data.key), "string", i);
				break;
				case option_types::integer:
					pr.print(blueprint->int_properties.at(lookup_data.key), property_manager.int_properties.at(lookup_data.key), "int", i);
				break;
				case option_types::decimal:
					pr.print(blueprint->double_properties.at(lookup_data.key), property_manager.double_properties.at(lookup_data.key), "double", i);
				break;
			}
		}
	}

	//Name...
	ldv::ttf_representation txt_menu{
		ttf_manager.get("main", 14),
		ldv::rgba8(255, 255, 255, 255),
		pr.ss.str()
	};

	txt_menu.go_to({0, 0});
	txt_menu.draw(_screen);
}

void properties::save_changes() {

	*(exchange_data.properties)=property_manager;
}
