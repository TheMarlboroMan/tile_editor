#pragma once

//local
#include "states.h"

//framework
#include <dfw/controller_interface.h>

//logger
#include <lm/logger.h>
#include <ldtools/view_composer.h>
#include <ldtools/ttf_manager.h>
#include <tools/pager.h>

//std
#include <filesystem>
#include <cmath>

namespace controller {

class file_browser:
	public dfw::controller_interface {

	public:

	                            file_browser(lm::logger&, ldtools::ttf_manager&, int);
	virtual void                loop(dfw::input&, const dfw::loop_iteration_data&);
	virtual void                draw(ldv::screen&, int);
	virtual void                awake(dfw::input& /*input*/) {}
	virtual void                slumber(dfw::input& /*input*/) {}
	virtual bool                can_leave_state() const {return true;}

	//!Sets the allow creation flag, which displays the "create" entry
	//!Reloads the data.
	void                        set_allow_create(bool _v);
	//!Sets the title that appears before the current directory.
	void                        set_title(const std::string& _title) {title=_title;}
	//!Returns if this controller exited with a chosen file.
	bool                        get_result() const {return result;}
	//!Returns the file that was chosen.
	std::string                 get_choice() const {return choice;}

	private:

	enum class working_modes{navigate, create};

	struct entry {
		enum class entry_type {file, dir, create};
		std::string             path_name;
		entry_type              type;
		bool                    is_dir() const {return entry_type::dir==type;}
		bool                    is_new() const {return entry_type::create==type;}
		bool                    is_file() const {return entry_type::file==type;}
		bool                    operator<(const entry& _other) const {

			if(_other.is_new() && !is_new()) {
				return false;
			}

			if(is_new() && !_other.is_new()) {
				return true;
			}

			if(_other.is_dir() && !is_dir()) {
				return false;
			}

			if(is_dir() && !_other.is_dir()) {
				return true;
			}

			return path_name < _other.path_name;
		}
	};

	//!Does the navigation input.
	void                        input_navigation(dfw::input&);
	//!Does the input for new names.
	void                        input_create(dfw::input&);
	//!Fills up "contents" with the contents of the current directory.
	void						extract_entries();
	//!Refreshes the ttf_representation.
	void						refresh_list_view();
	//!Positions the current selector representation.
	void                        position_selector();
	//!Void composes the title
	void                        compose_title();

	//references...
	lm::logger&                 log;
	ldtools::ttf_manager&       ttf_manager;

	//properties
	working_modes				mode;
	std::filesystem::path       current_directory;
	std::string                 title;
	std::vector<entry>          contents;
	int                         first_selection_y{0},
	                            y_selection_factor{0};
	bool                        allow_create{true}; //!< True if it allows the [new] entry.

	bool                        result{false};
	std::string                 choice;

	tools::pager                pager;

	//view properties.
	ldtools::view_composer		layout;

};

}
