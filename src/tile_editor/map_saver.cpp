#include "app/map_saver.h"
#include "serializer/map_serializer.h"
#include "editor_types/tile_layer.h"
#include "editor_types/thing_layer.h"
#include "editor_types/poly_layer.h"

#include <lm/sentry.h>
#include <ldt/polygon_2d.h>
#include <iostream>
#include <sstream>

using namespace tile_editor;

map_saver::map_saver(
	lm::logger& _logger
):
	log{_logger}
{

}

bool map_saver::save(
	tile_editor::map& _map,
	const std::string& _filename
) {

	lm::log(log, lm::lvl::info)<<"saving map into "<<_filename<<std::endl;

	pre_save(_map);

	tile_editor::map_serializer serializer;

	std::stringstream ss;
	ss<<MAJOR_VERSION<<"."<<MINOR_VERSION<<"."<<PATCH_VERSION;

	if(!serializer.to_file(_map, ss.str(), _filename)) {

		lm::log(log, lm::lvl::warning)<<"could not save into "<<_filename<<std::endl;
		return false;
	}

	//TODO: Errors and shit, log dem?
	lm::log(log, lm::lvl::info)<<"map saved into "<<_filename<<std::endl;
	return true;
}

void map_saver::pre_save(
	tile_editor::map& _map
) {

	lm::log(log, lm::lvl::info)<<"starting pre-save process..."<<std::endl;

	struct : public tile_editor::layer_visitor {
		map_saver * saver{nullptr};
		void visit(tile_editor::tile_layer& _layer) {saver->pre_save(_layer);}
		void visit(tile_editor::thing_layer& _layer) {saver->pre_save(_layer);}
		void visit(tile_editor::poly_layer& _layer) {saver->pre_save(_layer);}
	} dispatcher;
	dispatcher.saver=this;

	for(auto& layer : _map.layers) {

		//TODO: Check non repeated ids...

		layer->accept(dispatcher);
	}
}

void map_saver::pre_save(
	tile_editor::tile_layer& _layer
) {

	std::sort(
		std::begin(_layer.data),
		std::end(_layer.data),
		[](
			const tile_editor::tile& _a,
			const tile_editor::tile& _b
		) {
			//Top-down, left to right.
			if(_a.y > _b.y) {return true;}
			else if(_a.y < _b.y) {return false;}
			return _a.x < _b.x;
		}
	);
}

void map_saver::pre_save(
	tile_editor::thing_layer& _layer
) {

	std::sort(
		std::begin(_layer.data),
		std::end(_layer.data),
		[](
			const tile_editor::thing& _a,
			const tile_editor::thing& _b
		) {
			//Top-down, left to right.
			if(_a.y > _b.y) {return true;}
			else if(_a.y < _b.y) {return false;}
			return _a.x < _b.x;
		}
	);
}

void map_saver::pre_save(
	tile_editor::poly_layer& _layer
) {

	std::sort(
		std::begin(_layer.data),
		std::end(_layer.data),
		[](
			const tile_editor::poly& _a,
			const tile_editor::poly& _b
		) {
			auto ca=ldt::calculate_centroid(_a.points);
			auto cb=ldt::calculate_centroid(_b.points);

			//Top-down, left to right.
			if(ca.y > cb.y) {return true;}
			else if(ca.y < cb.y) {return false;}
			return ca.x < cb.x;
		}
	);
}
