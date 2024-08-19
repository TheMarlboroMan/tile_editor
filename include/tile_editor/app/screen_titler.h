#pragma once

#include <string>

namespace ldv {

	class screen;
}

namespace tile_editor {

class screen_titler {

	public:

	void        set_title(const std::string&);
	void        set_screen(ldv::screen*);

	private:

	ldv::screen*    screen{nullptr};
};

}
