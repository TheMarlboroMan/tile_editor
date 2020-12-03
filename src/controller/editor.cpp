#include "controller/editor.h"

//local
#include "input/input.h"
#include "app/map_loader.h"
#include "app/map_saver.h"
#include "app/map_loader.h"
#include "tile_editor/parser/blueprint_parser.h"


#include <lm/sentry.h>

using namespace controller;

editor::editor(
	lm::logger& _log,
	ldtools::ttf_manager& _ttf_manager,
	tools::message_manager& _message_manager,
	tile_editor::exchange_data& _exchange_data,
	unsigned int _screen_w,
	unsigned int _screen_h
)
	:log(_log),
	ttf_manager(_ttf_manager),
	message_manager{_message_manager},
	exchange_data{_exchange_data},
	screen_rect{0, 0, _screen_w, _screen_h},
	camera{
		screen_rect, //pointing at world 0,0.
		{0,0}           //at 0.0 in the screen...
	},
	last_message_rep{
		ttf_manager.get("main", 14),
		ldv::rgba8(255, 255, 255, 255),
	},
	mouse_pos{0,0}
{

	message_manager.subscribe("editor", [this](tools::message_manager::notify_event_type _type) {

		receive_message(_type);
	});
}

void editor::awake(dfw::input& /*_input*/) {

	lm::log(log, lm::lvl::info)<<"map editor controller awakens"<<std::endl;

	if(exchange_data.has(state_editor)) {

		exchange_data.recover(state_editor);
		if(exchange_data.file_browser_success) {

			//Save as requested...
			if(exchange_data.file_browser_allow_create) {

				current_filename=exchange_data.file_browser_choice;
				lm::log(log, lm::lvl::info)<<"map editor changes current filename to "<<current_filename<<std::endl;
				save_current();
			}
			//Load requested...
			else {

				tile_editor::map_loader ml{log, message_manager};
				//TODO: sure, where's the map???
				ml.load_from_file(exchange_data.file_browser_choice);
			}
		}
	}
}

void editor::loop(dfw::input& _input, const dfw::loop_iteration_data& /*_lid*/) {

	if(_input().is_exit_signal() || _input.is_input_down(input::escape)) {
		set_leave(true);
		return;
	}

	if(_input.is_input_down(input::load)) {

		exchange_data.file_browser_allow_create=false;
		exchange_data.file_browser_title="Load map file";
		exchange_data.file_browser_invoker_id=state_editor;
		exchange_data.put(state_file_browser);
		push_state(state_file_browser);
		return;
	}

	if(_input.is_input_down(input::save)) {

		if(_input.is_input_pressed(input::left_control)) {

			exchange_data.file_browser_allow_create=true;
			exchange_data.file_browser_title="Save map file as";
			exchange_data.file_browser_invoker_id=state_editor;
			exchange_data.put(state_file_browser);
			push_state(state_file_browser);
			return;
		}
		else {

			if(!current_filename.size()) {

				message_manager.add("cannot save without current filename, use ctrl+s");
				return;
			}
			save_current();
		}
	}

	mouse_pos=get_mouse_position(_input);

	if(_input.is_input_down(input::zoom_in)) {

		zoom_in();
		return;
	}
	else if(_input.is_input_down(input::zoom_out)) {

		zoom_out();
		return;
	}

	/*
	typedef  bool (dfw::input::*input_fn)(int) const;
	input_fn movement_fn=_input.is_input_pressed(input::left_control)
		? &dfw::input::is_input_down
		: &dfw::input::is_input_pressed;

	const int factor=_input.is_input_pressed(input::left_control) ? 1 : movement_factor;
	int movement_x=0,
		movement_y=0;

	if(std::invoke(movement_fn, _input, input::up)) {

		movement_y=-1*factor;
	}
	if(std::invoke(movement_fn, _input, input::down)) {

		movement_y=1*factor;
	}

	if(std::invoke(movement_fn, _input, input::left)) {

		movement_x=-1*factor;
	}
	if(std::invoke(movement_fn, _input, input::right)) {

		movement_x=1*factor;
	}

	if(movement_x || movement_y) {

		perform_movement(
			movement_x,
			movement_y,
			_input.is_input_pressed(input::resize),
			_input.is_input_pressed(input::align)
		);
		return;
	}
	*/
}

void editor::draw(ldv::screen& _screen, int /*fps*/) {

	_screen.clear(ldv::rgba8(0, 0, 0, 255));

	draw_messages(_screen);
	draw_hud(_screen);
}

void editor::draw_messages(ldv::screen& _screen) {

	last_message_rep.draw(_screen);
}

void editor::draw_hud(ldv::screen& _screen) {

	ldv::ttf_representation txt_hud{
		ttf_manager.get("main", 14),
		ldv::rgba8(255, 255, 255, 192),
		""
	};

	std::string txt=std::to_string(mouse_pos.x)+","+std::to_string(mouse_pos.y);
	txt_hud.go_to({0,0});
	txt_hud.set_text(txt);
	txt_hud.draw(_screen);
}

ldt::point_2d<int> editor::get_mouse_position(dfw::input& _input) const {

	auto pos=_input().get_mouse_position();

	pos.x/=camera.get_zoom();
	pos.y/=camera.get_zoom();

	pos.x+=camera.get_x();
	pos.y+=camera.get_y();

	return {pos.x, pos.y};
}

void editor::zoom_in() {

	auto zoom=camera.get_zoom();

	if(zoom > 0.1) {

		camera.set_zoom(zoom/2.);
	}
}

void editor::zoom_out() {

	auto zoom=camera.get_zoom();

	if(zoom < 4) {

		camera.set_zoom(zoom*2.);
	}
}

void editor::receive_message(tools::message_manager::notify_event_type /*_type*/) {

	if(message_manager.size()) {

		last_message_rep.set_text(message_manager.last());

		last_message_rep.align(
			screen_rect,
			ldv::representation_alignment{
				ldv::representation_alignment::h::center,
				ldv::representation_alignment::v::inner_bottom
			}
		);
	}
	else {
		last_message_rep.set_text("");
	}
}

void editor::save_current() {

	tile_editor::map_saver ms{log, message_manager};
	ms.save(map, current_filename);
}

void editor::load_map(const std::string& _path) {

	tile_editor::map_loader ml{log, message_manager};
	ml.load_from_file(_path);
	current_filename=_path;
}

void editor::load_session(const std::string& _path) {

	tile_editor::blueprint_parser cfp;
	session=cfp.parse_file(_path);
}
