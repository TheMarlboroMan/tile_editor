#ifndef CONTENEDOR_TILESETS_H
#define CONTENEDOR_TILESETS_H

#include "contenedor_ciclico.h"
#include "tile.h"
#include <herramientas/lector_txt/lector_txt.h>
#include <stdexcept>

class Contenedor_tilesets_exception:public std::runtime_error
{
	public:

	Contenedor_tilesets_exception(const std::string& c)
		:std::runtime_error(c)
	{}
};

class Contenedor_tilesets:public Contenedor_ciclico<Tile_set>
{
	public:

	void insertar_tileset(size_t indice, const std::string& ruta, const char comentario);
};

#endif
