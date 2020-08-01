#pragma once

#include <vector>
#include <cstdint>

namespace tile_editor {

//!Generic layer.
template<typename T>
struct layer {

	public:

	using               container=std::vector<T>;

	std::size_t         set;
	int                 alpha;
	container           data;
};

}
