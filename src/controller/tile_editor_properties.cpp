#include "../../include/controller/tile_editor_properties.h"
#include "../../include/input/input.h"
#include "blueprint_types/map_blueprint.h"

#include <ldv/ttf_representation.h>

#include <vector>
#include <sstream>

using namespace controller;

tile_editor_properties::tile_editor_properties(
	lm::logger& _log, 
	ldtools::ttf_manager& _ttf_manager, 
	tile_editor::exchange_data& _exchange_data
):log(_log),
	ttf_manager{_ttf_manager},
	exchange_data{_exchange_data} {

	std::vector<int> intsets{};

	menu.set_wrap(false);
	menu.insert(menu_layer_id, "");
	menu.insert(menu_layer_alpha, 0, 0, 255, false);
	menu.insert(menu_layer_set, intsets, true);
	menu.insert(menu_layer_exit);
	menu.insert(menu_layer_cancel);
}

void tile_editor_properties::awake(dfw::input&) {

	if(!exchange_data.has(state_tile_editor_properties)) {

		throw std::runtime_error("could not find data for tile_editor_properties");
	}

	exchange_data.recover(state_tile_editor_properties);
	layer=static_cast<tile_editor::tile_layer*>(exchange_data.layer);
	exchange_data.layer=nullptr;

	sets.clear();
	menu.clear_choice(menu_layer_set);

	for(const auto& pair : exchange_data.blueprint->tilesets) {

		int id=pair.first;
		sets[id]=pair.second.name;
		menu.add(menu_layer_set, id);
	}

	menu.set(menu_layer_id, layer->id);
	menu.set(menu_layer_alpha, layer->alpha);
	menu.set(menu_layer_set, (int)layer->set);
	current_key=0;
}

void tile_editor_properties::slumber(dfw::input&) {

	layer=nullptr;
}

void tile_editor_properties::loop(dfw::input& _input, const dfw::loop_iteration_data& /*lid*/) {

	if(_input().is_exit_signal()) {
		set_leave(true);
		return;
	}

 	if(_input.is_input_down(input::escape)) {

		pop_state();
		return;
	}

	text_mode 
		? input_text(_input)
		: input_traverse(_input);
}

void tile_editor_properties::input_text(dfw::input& _input) {

	//TODO: feed the value from the menu...

	if(_input.is_input_down(input::enter)) {

		//TODO: validate input data...

		text_mode=false;
		//TODO: clear and exit text mode
	}

}

void tile_editor_properties::input_traverse(dfw::input& _input) {

	if(_input.is_input_down(input::down)) {
		current_key=menu.adjacent_key(current_key, decltype(menu)::browse_dir::next);
		return;
	}
	else if(_input.is_input_down(input::up)) {
		current_key=menu.adjacent_key(current_key, decltype(menu)::browse_dir::previous);
		return;
	}

	if(_input.is_input_down(input::enter)) {
	
		switch(current_key) {

			case menu_layer_cancel:
				pop_state();
				return;

			case menu_layer_exit:
				save_changes();
				pop_state();
				return;

			case menu_layer_set:
				menu.browse(menu_layer_set, decltype(menu)::browse_dir::next);
				return;

			default:
				text_mode=true;
				//TODO: clear and start text mode
			break;
		}
	}
}

void tile_editor_properties::draw(ldv::screen& _screen, int /*fps*/) {

	_screen.clear(ldv::rgba8(0, 0, 0, 255));

	auto selected=[this](int index) -> const char *{

		return index==current_key ? "[>] " : "[ ] ";
	};

	std::stringstream ss;
	for(int i=0; i < menu_layer_end; i++) {

		switch(i) {
			case menu_layer_id:
				ss<<selected(i)<<"id (string): "<<menu.get_string(i)<<std::endl;
			break;
			case menu_layer_alpha:
				ss<<selected(i)<<"alpha (0-255): "<<menu.get_int(i)<<std::endl;
			break;
				//TODO: sets should be translated!!!
			case menu_layer_set:
				ss<<selected(i)<<"set (choice): "<<sets[menu.get_int(i)]<<std::endl;
			break;
			case menu_layer_exit:
				ss<<selected(i)<<"back"<<std::endl;
			break;
			case menu_layer_cancel:
				ss<<selected(i)<<"cancel"<<std::endl;
			break;
		}
	}

	//Name...
	ldv::ttf_representation txt_menu{
		ttf_manager.get("main", 14),
		ldv::rgba8(255, 255, 255, 255),
		ss.str()
	};

	txt_menu.go_to({0, 0});
	txt_menu.draw(_screen);
}

void tile_editor_properties::save_changes() {

	layer->id=menu.get_string(menu_layer_id);
	layer->alpha=menu.get_int(menu_layer_alpha);
	layer->set=(std::size_t)menu.get_int(menu_layer_set);
}
