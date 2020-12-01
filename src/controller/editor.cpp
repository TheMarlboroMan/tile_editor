#include "../../include/controller/editor.h"

//local
#include "../../include/input/input.h"

using namespace controller;

editor::editor(
	lm::logger& _log,
	ldtools::ttf_manager& _ttf_manager,
	unsigned int _screen_w,
	unsigned int _screen_h
)
	:log(_log),
	ttf_manager(_ttf_manager),
	screen_rect{0, 0, _screen_w, _screen_h},
	camera{
		screen_rect, //pointing at world 0,0.
		{0,0}           //at 0.0 in the screen...
	},
	last_message_rep{
		ttf_manager.get("main", 12),
		ldv::rgba8(255, 255, 255, 255),
	},
	mouse_pos{0,0}
{

}

void editor::loop(dfw::input& _input, const dfw::loop_iteration_data& lid) {

	if(_input().is_exit_signal() || _input.is_input_down(input::escape)) {
		set_leave(true);
		return;
	}

	mouse_pos=get_mouse_position(_input);

	if(last_message.time > 0.f) {

		last_message.time-=lid.delta;
	}

	if(_input.is_input_down(input::zoom_in)) {

		zoom_in();
		return;
	}
	else if(_input.is_input_down(input::zoom_out)) {

		zoom_out();
		return;
	}

	/*
	TODO: Add input movement.
	*/
}

void editor::draw(ldv::screen& _screen, int /*fps*/) {

	_screen.clear(ldv::rgba8(0, 0, 0, 255));

	draw_messages(_screen);
	draw_hud(_screen);
}

void editor::draw_messages(ldv::screen& _screen) {

	if(last_message.time > 0.f) {

		last_message_rep.draw(_screen);
	}
}

void editor::draw_hud(ldv::screen& _screen) {

	ldv::ttf_representation txt_hud{
		ttf_manager.get("main", 12),
		ldv::rgba8(255, 255, 255, 192),
		""
	};

	std::string txt=std::to_string(mouse_pos.x)+","+std::to_string(mouse_pos.y);
	txt_hud.go_to({0,0});
	txt_hud.set_text(txt);
	txt_hud.draw(_screen);
}

void editor::add_message(const std::string& _msg) {

	last_message.message=_msg;
	last_message.time=30.0f;
	last_message_rep.set_text(last_message.message);

	last_message_rep.align(
		screen_rect,
		ldv::representation_alignment{
			ldv::representation_alignment::h::center,
			ldv::representation_alignment::v::inner_bottom
		}
	);
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
