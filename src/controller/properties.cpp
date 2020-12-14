#include "../../include/controller/properties.h"
#include "../../include/input/input.h"

#include <ldv/ttf_representation.h>
#include <sstream>


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

	//This is abhorrent, of course, but, enough for this tool: the type of the
	//value will be contained in the meny key. We might as well go get_type	
	//for this.
	
	auto add_str=[this](const auto& _props) {

		std::string key{"str-"};

		for(const auto& pair : _props) {

			menu.insert(key+pair.first, pair.second);
		}
	};

	auto add_numeric=[this](const auto& _props, const std::string& _key) {

		for(const auto& pair : _props) {

			auto min=(decltype(pair.second))-99999;
			auto max=(decltype(pair.second))99999;

			menu.insert(_key+pair.first, pair.second, min, max, false);
		}
	};

	menu.clear();
	add_str(property_manager->string_properties);
	add_numeric(property_manager->int_properties, "int-");
	add_numeric(property_manager->double_properties, "double-");
	menu.insert("exit");
	menu.insert("cancel");
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

/**
	auto is_current=[]() -> bool {

		//TODO:
		return false;
	};
*/
	auto cursor=[this](const std::string _key) -> std:string {

		return current_key==_key
			? "[>]"
			: "[ ]";
	};

	auto name=[this](const std::string& _key) -> std::string {

		if(_key=="exit" || _key=="cancel") {

			return _key;
		}

		const auto dash_pos=_key.find("-");
		const std::string& type=_key.substr(0, dash_posh);
		const std::string& key=_key.substr(dash_posh+1);

		if(type=="str") {

			return property_manager->string_properties.at(key).name;
		}
		else if(type=="int") {

			return property_manager->int_properties.at(key).name;
		}
		else if(type=="double") {

			return property_manager->double_properties.at(key).name;
		}
		else {

			throw std::runtime_error(std::string{"invalid type '"}+_type+"'");
		}
	};

	auto value=[this](
		std::stringstream& _ss,
		const std::string& _key
	) {

		const auto dash_pos=_key.find("-");
		const std::string& type=_key.substr(0, dash_posh);
		const std::string& key=_key.substr(dash_posh+1);

		if(type=="str") {

			return property_manager->string_properties.at(key).name;
		}
		else if(type=="int") {

			return property_manager->int_properties.at(key).name;
		}
		else if(type=="double") {

			return property_manager->double_properties.at(key).name;
		}
		else {

			throw std::runtime_error(std::string{"invalid type '"}+_type+"'");
		}
	};

	auto help=[this](const std::string& _key) -> std::string {
	
		return "";
	};

	std::stringstream ss;
	for(const auto& key : menu.get_keys()) {

		ss<<cursor(key)<<" "<<name(key)<<":"<<value(key);
	}

	ss<<help(current_key);

	//Name...
	ldv::ttf_representation txt_menu{
		ttf_manager.get("main", 14),
		ldv::rgba8(255, 255, 255, 255),
		ss.str()
	};

	txt_menu.go_to({0, 0});

	txt_menu.draw(_screen);
}

void properties::save_changes() {

	//TODO.
}
