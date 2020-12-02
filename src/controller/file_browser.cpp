#include "controller/file_browser.h"

#include <tools/json.h>
#include <tools/file_utils.h>
#include <tools/string_utils.h>

#include <ldv/ttf_representation.h>

//local
#include "input/input.h"

#include <algorithm>

using namespace controller;

file_browser::file_browser(
	lm::logger& plog,
	ldtools::ttf_manager& _ttfman,
	int _window_height
):
log(plog),
ttf_manager{_ttfman},
mode{working_modes::navigate},
current_directory{std::filesystem::current_path()},
pager{0, 0} {

	//Mount layout...
	layout.map_font("default_font", ttf_manager.get("main", 14));

	auto root=tools::parse_json_string(
		tools::dump_file(
			"data/layouts.json"
		)
	);

	layout.parse(root["file_browser"]);
	first_selection_y=layout.get_int("first_selection_y");
	y_selection_factor=layout.get_int("y_selection_factor");

	//Calculate items per page based on window height...
	int excess_height=_window_height-first_selection_y;
	pager.set_items_per_page(excess_height / y_selection_factor);

	//Setup data...
	set_title("file browser");
	extract_entries();
	refresh_list_view();
	position_selector();
	compose_title();
}

void file_browser::loop(dfw::input& _input, const dfw::loop_iteration_data& /*lid*/) {

	if(_input().is_exit_signal()) {

		set_leave(true);
		return;
	}

/*
//TODO:
	if(_input.is_input_down(input::help)) {

		push_state(state_help);
	}
*/

	switch(mode) {
		case working_modes::navigate:
			input_navigation(_input);
		break;
		case working_modes::create:
			input_create(_input);
		break;
	}
}

void file_browser::draw(ldv::screen& screen, int /*fps*/) {

	screen.clear(ldv::rgba8(0, 0, 0, 255));
	layout.draw(screen);
}

void file_browser::extract_entries() {

	contents.clear();

	if(allow_create) {

		contents.push_back({
			"new",
			entry::entry_type::create
		});
	}

	if(current_directory!=current_directory.parent_path()) {
		contents.push_back({
			"..",
			entry::entry_type::dir
		});
	}

	for(const auto& dir_entry : std::filesystem::directory_iterator(current_directory)) {

		const auto& path=dir_entry.path();
		std::string filename=path.filename();

		if(std::filesystem::is_directory(path)) {

			contents.push_back({
				filename,
				entry::entry_type::dir
			});
		}
		else if(std::filesystem::is_regular_file(path)){

			contents.push_back({
				filename,
				entry::entry_type::file
			});
		}
	}

	std::sort(
		std::begin(contents),
		std::end(contents)
	);

	pager.set_item_count(contents.size());
	pager.reset();
}

void file_browser::refresh_list_view() {

	std::string files;
	long int ipp=pager.get_items_per_page();
	auto it=std::begin(contents)+(pager.get_current_page() * ipp);
	auto begin=it;

	//Read the next N items...
	for(; it!=std::end(contents) && std::distance(begin, it) < ipp; ++it) {

		files+=it->is_dir()
			? "["+it->path_name+"]\n"
			: it->is_new()
				? "("+it->path_name+")\n"
				: it->path_name+"\n";
	}

	static_cast<ldv::ttf_representation *>(
		layout.get_by_id("list")
	)->set_text(files);
}

void file_browser::position_selector() {

	int y_pos=first_selection_y+(pager.get_relative_index()*y_selection_factor);

	static_cast<ldv::ttf_representation *>(
		layout.get_by_id("selection")
	)->go_to({0, y_pos});
}

void file_browser::compose_title() {

	std::string final_title{title};

	final_title+=" : "
		+current_directory.string()
		+" ["
		+std::to_string(pager.get_current_page()+1)
		+"/"
		+std::to_string(pager.get_pages_count())
		+"] ipp: "
		+std::to_string(pager.get_items_per_page());

	static_cast<ldv::ttf_representation *>(
		layout.get_by_id("title")
	)->set_text(final_title);
}

void file_browser::input_navigation(dfw::input& _input) {

	bool check_change=false;

	//Cancelling...
	if(_input.is_input_down(input::escape)) {

		result=false;
		choice={};
		pop_state();
		return;
	}

	//Movement...
	if(_input.is_input_down(input::down)) {

		pager.cycle_item(decltype(pager)::dir::next);
		check_change=true;
	}
	else if(_input.is_input_down(input::up)) {

		pager.cycle_item(decltype(pager)::dir::previous);
		check_change=true;
	}
	else if(_input.is_input_down(input::pagedown)) {

		pager.turn_page(decltype(pager)::dir::next);
		check_change=true;
	}
	else if(_input.is_input_down(input::pageup)) {

		pager.turn_page(decltype(pager)::dir::previous);
		check_change=true;
	}

	if(check_change) {

		if(pager.is_item_cycled()) {
			position_selector();
		}

		if(pager.is_page_turned()) {
			refresh_list_view();
			compose_title();
		}

		return;
	}

	if(_input.is_input_down(input::enter)) {

		const auto item=contents[pager.get_current_index()];

		if(item.is_dir()) {

			current_directory/={item.path_name};
			current_directory=current_directory.lexically_normal();

			extract_entries();
			refresh_list_view();
			position_selector();
			compose_title();
		}
		else if(item.is_new()) {

			mode=working_modes::create;
			contents.front().path_name="";
			refresh_list_view();

			//TODO: Why is this not a parameter to start_text_input???
			_input().set_keydown_control_text_filter(true);
			_input().start_text_input();
			return;
		}
		else {

			result=true;
			auto final_path=current_directory/item.path_name;
			choice=final_path.string();
			pop_state();
			return;
		}
	}
}

void file_browser::input_create(dfw::input& _input) {

	if(_input.is_input_down(input::escape)) {

		mode=working_modes::navigate;
		_input().stop_text_input();
		_input().clear_text_input();

		//Restore the first item...
		contents.front().path_name="new";
		refresh_list_view();
		return;
	}

	if(_input().is_event_text()) {

		contents.front().path_name=_input().get_text_input();
		refresh_list_view();
	}

	if(_input.is_input_down(input::enter)) {

		if(!_input().get_text_input().size()) {
			return;
		}

		result=true;
		auto path=current_directory;

		path/=tools::str_trim({_input().get_text_input()});
		choice=path.string();

		_input().stop_text_input();
		_input().clear_text_input();
		pop_state();
		return;
	}
}

void file_browser::set_allow_create(bool _v) {

	allow_create=_v;
	extract_entries();
	refresh_list_view();
	position_selector();
	compose_title();
}
