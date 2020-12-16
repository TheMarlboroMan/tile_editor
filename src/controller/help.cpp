#include "controller/help.h"
#include "input/input.h"
#include "app/definitions.h"

#include <tools/file_utils.h>

using namespace controller;

help::help(
	lm::logger& _log,
	ldtools::ttf_manager& _ttf_manager,
	unsigned int _w,
	unsigned int _h
)
	:log(_log),
	ttf_manager{_ttf_manager},
	camera{
		{0,0, _w, _h},
		{0,0}
	},
	help_rep{
		ttf_manager.get(tile_editor::definitions::main_font_name, tile_editor::definitions::main_font_size),
		ldv::rgba8(255, 255, 255, 255),
	} {

	help_rep.set_max_width(_w);
	help_rep.set_text(tools::dump_file("data/help.txt"));
	help_rep.go_to({0,0});
	camera.set_limits(help_rep.get_view_position());

	camera.go_to({0,0});
}

void help::awake(dfw::input&) {

	camera.go_to({0,0});
}

void help::loop(dfw::input& _input, const dfw::loop_iteration_data& /*lid*/) {

	if(_input().is_exit_signal()) {
		set_leave(true);
		return;
	}

	if(_input.is_input_down(input::escape)) {
		pop_state();
		return;
	}

	if(_input.is_input_pressed(input::down)) {

		camera.move_by(0, 5);
	}
	else if(_input.is_input_pressed(input::up)) {

		camera.move_by(0, -5);
	}
}

void help::draw(ldv::screen& _screen, int /*fps*/) {

	_screen.clear(ldv::rgba8(0, 0, 0, 255));
	help_rep.draw(_screen, camera);
}
