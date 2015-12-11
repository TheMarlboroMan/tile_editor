#include "contenedor_tilesets.h"
#include <algorithm>

void Contenedor_tilesets::insertar_tileset(size_t indice, const std::string& ruta, const char comentario)
{
	using namespace Herramientas_proyecto;
	Tile_set GT(indice);
	Lector_txt L(ruta, comentario);	

	if(L)
	{
		while(true)
		{
			const std::string c=L.leer_linea();
			if(!L) break;
			GT.insertar_tile_desde_cadena(c);
		}

		insertar(GT);
	}
	else 
	{
		throw Contenedor_tilesets_exception("No se ha localizado el archivo "+ruta+" para abrir tileset");
	}

}
