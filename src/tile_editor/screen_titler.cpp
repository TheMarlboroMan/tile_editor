#include "app/screen_titler.h"
#include "ldv/screen.h"
#include <stdexcept>

using namespace tile_editor;

void screen_titler::set_screen(
	ldv::screen* _scr
) {

	screen=_scr;
}

void screen_titler::set_title(
	const std::string& _title
) {

	if(nullptr==screen) {

		throw std::runtime_error("cannot set title when screen is not assigned");
	}

	screen->set_title(_title);
}
