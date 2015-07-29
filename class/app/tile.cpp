#include "tile.h"
#include <herramientas/log_base/log_base.h>

using namespace DLibH;

extern Log_base LOG;

void Tile_set::insertar_tile_desde_cadena(const std::string& s)
{
	if(!s.size())
	{
		LOG<<"WARNING: Se intenta insertar tile desde cadena vacía"<<std::endl;
	}
	else
	{
		const char SEPARADOR='\t';
		auto ex=Herramientas::explotar(s, SEPARADOR);

		if(ex.size() < 5)
		{
			LOG<<"WARNING: La cadena no tiene suficientes parámetros para generar un tile"<<std::endl;	
		}
		else
		{
			//Se ignora el primer parámetro.
			int tipo=std::atoi(ex[0].c_str()),
				x=std::atoi(ex[1].c_str()),
				y=std::atoi(ex[2].c_str()),
				w=std::atoi(ex[3].c_str()),
				h=std::atoi(ex[4].c_str());

			insertar(Tile(tipo, x, y, w, h));
		}
	}
}
