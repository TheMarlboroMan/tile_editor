#pragma once

#include "app/env.h"

#include <dfw/base_config.h>
#include <dfw/input.h>

#include <iostream>

namespace dfwimpl {

class config:
	public dfw::base_config {
	////////////////////////////////
	// Public interface.

	public:

	config(const tile_editor::env&);

	//Fullfillment of the kernel interface.

	virtual std::string generate_file_version() const {return "1";}
	virtual std::string get_key_file_version() const {return "meta:v";}
	virtual std::string get_key_screen_double_buffer() const {return "video:double_buffer";}
	virtual std::string get_key_screen_vsync() const {return "video:vsync";}
	virtual std::string get_key_sound_volume() const {return "audio:sound_volume";}
	virtual std::string get_key_music_volume() const {return "audio:music_volume";}
	virtual std::string get_key_audio_ratio() const {return "audio:audio_ratio";}
	virtual std::string get_key_audio_buffers() const {return "audio:audio_buffers";}
	virtual std::string get_key_audio_out() const {return "audio:audio_out";}
	virtual std::string get_key_audio_channels() const {return "audio:audio_channels";}

	///////////////////////////////////
	// Properties.

	private:

	std::string get_file_path(const tile_editor::env& _env) const {return _env.build_user_path("config.json");}
};
}
