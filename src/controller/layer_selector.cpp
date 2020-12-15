#include "controller/layer_selector.h"
#include "input/input.h"
#include "app/definitions.h"

#include <ldv/ttf_representation.h>
#include <ldv/box_representation.h>

#include <sstream>

using namespace controller;

layer_selector::layer_selector(
	lm::logger& plog,
	ldtools::ttf_manager& _ttf_manager,
	tile_editor::exchange_data& _exchange_data
)
	:log(plog),
	ttf_manager{_ttf_manager},
	exchange_data{_exchange_data}{

}

void layer_selector::awake(
	dfw::input& /*input*/
) {

	if(!exchange_data.has(state_layer_selector)) {

		throw std::runtime_error("could not find data for layer_selector");
	}

	exchange_data.recover(state_layer_selector);
	layers=exchange_data.layers;
	exchange_data.layers=nullptr;
}

void layer_selector::slumber(
	dfw::input& /*input*/
) {

	layers=nullptr;
	exchange_data.current_layer=nullptr;
}

void layer_selector::loop(dfw::input& _input, const dfw::loop_iteration_data& /*lid*/) {

	if(_input().is_exit_signal()) {
		set_leave(true);
		return;
	}

	if(_input.is_input_down(input::escape)
		|| _input.is_input_down(input::enter)) {

		pop_state();
		return;
	}

	//Layer selection...
	if(_input.is_input_down(input::up)) {

		if(*(exchange_data.current_layer) > 0) {
			--(*exchange_data.current_layer);
		}
		return;
	}
	else if(_input.is_input_down(input::down)) {

		if(*(exchange_data.current_layer) < layers->size()-1) {
			++(*exchange_data.current_layer);
		}
		return;
	}

	//Layer reordering...
	if(_input.is_input_down(input::pageup)) {

		if(*(exchange_data.current_layer) > 0) {
			std::swap( (*layers)[*(exchange_data.current_layer)], (*layers)[*(exchange_data.current_layer)-1]);
			--(*exchange_data.current_layer);
		}
		return;
	}
	else if(_input.is_input_down(input::pagedown)) {

		if(*(exchange_data.current_layer) < layers->size()-1) {
			std::swap( (*layers)[*(exchange_data.current_layer)], (*layers)[*(exchange_data.current_layer)+1]);
			++(*exchange_data.current_layer);
		}
		return;
	}

	if(_input.is_input_down(input::del)) {

		delete_layer();
		return;
	}

	if(_input.is_input_down(input::insert)) {

		insert_layer();
		return;
	}
}

void layer_selector::request_draw(
	dfw::controller_view_manager& _cvm
) {
	_cvm.add(state_editor);
	_cvm.add_ptr(this);
}

void layer_selector::draw(
	ldv::screen& _screen,
	int /*fps*/
) {

	//add text...
	std::stringstream ss;
	std::size_t index=0;
	for(const auto& layer : *layers) {

		ss<<(index++==(*exchange_data.current_layer) ? "[*] " : "[ ] ")
			<<layer->id<<" ["<<layer->alpha<<"]"<<std::endl;
	}

	//draw...
	ldv::ttf_representation txt_menu{
		ttf_manager.get(tile_editor::definitions::main_font_name, tile_editor::definitions::main_font_size),
		ldv::rgba8(255, 255, 255, 255),
		ss.str()
	};

	txt_menu.set_line_height_ratio(tile_editor::definitions::line_height_ratio);

	//centered alignment.
	ldv::representation_alignment center={
		ldv::representation_alignment::h::center,
		ldv::representation_alignment::v::center
	};

	//background box
	auto box=txt_menu.get_text_position();
	ldv::box_representation background(
		{
			{0, 0},
			box.w+tile_editor::definitions::property_box_padding,
			box.h+tile_editor::definitions::property_box_padding
		},
		ldv::rgba8(0,0,0,255)
	);
	background.align(_screen.get_rect(), center);
	background.draw(_screen);

	//border...
	background.set_filltype(ldv::polygon_representation::type::line);
	background.set_color(ldv::rgba8(255, 255, 255, 255));
	background.draw(_screen);

	//and finally the text.
	txt_menu.align(background, center);
	txt_menu.draw(_screen);
}

void layer_selector::delete_layer() {

	layers->erase(std::begin(*layers)+(*exchange_data.current_layer));
	if(*exchange_data.current_layer) {
		--(*exchange_data.current_layer);
	}

	if(!layers->size()) {

		pop_state();
	}
}

void layer_selector::insert_layer() {

	//TODO: For this, we need the whole map prototype...
}
