#ifndef TILE_H
#define TILE_H

#include "base_set.h"
#include <herramientas/herramientas/herramientas.h>

class Tile
{
	private:
	int tipo, x, y, w, h;

	public:

	Tile()
		:tipo(0), x(0), y(0), w(0), h(0)
	{}

	Tile(int pt, int px, int py, int pw, int ph)
		:tipo(pt), x(px), y(py), w(pw), h(ph)
	{}

	unsigned int acc_x() const {return x;}
	unsigned int acc_y() const {return y;}
	unsigned int acc_w() const {return w;}
	unsigned int acc_h() const {return h;}
	unsigned int acc_tipo() const {return tipo;}
};

/*El gestor de tiles guarda una colección completa de tiles y el índice del 
recurso asociado con la misma.*/

class Tile_set:public Base_set<Tile>
{
	private:

	size_t indice_recurso;

	public:

	Tile_set(size_t pi):
		indice_recurso(pi)
	{}

	void insertar_tile_desde_cadena(const std::string& s);
	size_t acc_indice_recurso() const {return indice_recurso;}
};

#endif
