#include "importador.h"

#include <herramientas/lector_txt/lector_txt.h>
#include <herramientas/herramientas/herramientas.h>
#include "definiciones_importacion_exportacion.h"

using namespace DLibH;
typedef Definiciones_importacion_exportacion DEFS;

void Importador::importar(std::vector<Rejilla>& rejillas, std::vector<Capa_logica>& capas_logica, std::vector<Propiedad_meta>& propiedades_meta, 
	const Contenedor_tilesets& contenedor_tilesets, const Contenedor_logica_sets& contenedor_logica_sets, const std::string nombre_fichero)
{
	Lector_txt L(nombre_fichero, '#');
	if(!L)
	{
		throw Importador_exception("El fichero "+nombre_fichero+" no pudo ser abierto");
	}
	else
	{
		rejillas.clear();
		capas_logica.clear();
		propiedades_meta.clear();
		Rejilla * r=nullptr;
		Capa_logica * l=nullptr;

		enum class e {NADA, ACTIVO, INFO, REJILLA, CELDA, LOGICA, OBJETOS, META};
		e estado=e::NADA;

		while(true)
		{

			std::string linea=L.leer_linea();
			if(!L) break;

			if(linea==DEFS::ABRE_ESTRUCTURA) estado=e::ACTIVO;
			else if(linea==DEFS::CIERRA_ESTRUCTURA) estado=e::NADA;
			else if(linea==DEFS::ABRE_INFO) estado=e::INFO; 
			else if(linea==DEFS::ABRE_REJILLA) estado=e::REJILLA;
			else if(linea==DEFS::ABRE_CELDA) estado=e::CELDA;
			else if(linea==DEFS::ABRE_LOGICA) estado=e::LOGICA;
			else if(linea==DEFS::ABRE_OBJETOS) estado=e::OBJETOS;
			else if(linea==DEFS::ABRE_META) estado=e::META;
			else if(linea==DEFS::CIERRA_META) estado=e::NADA;
			else
			{
				if(linea==DEFS::CIERRA_ESTRUCTURA) break;

				switch(estado)
				{
					case e::NADA: break;
					case e::ACTIVO: break; //Simplemente reconocer que existe. Es más para las aplicaciones que usen estos ficheros que para el propio editor.
					case e::INFO: leer_como_info(linea, rejillas, capas_logica); break;
					case e::REJILLA: r=leer_como_rejilla(linea, rejillas, contenedor_tilesets); break;
					case e::CELDA: leer_como_celdas(linea, *r); break;
					case e::LOGICA: l=leer_como_capa_logica(linea, capas_logica, contenedor_logica_sets); break;
					case e::OBJETOS: leer_como_objeto_logica(linea, *l); break;
					case e::META: leer_como_meta(linea, propiedades_meta); break;
				}
			}
		}
	}
}

void Importador::leer_como_meta(const std::string& cadena, std::vector<Propiedad_meta>& propiedades)
{
	auto partes=Herramientas::explotar(cadena, DEFS::SEPARADOR_PROPIEDADES_META, 1);
	if(partes.size()==2) propiedades.emplace_back(partes[0], partes[1]);
}

void Importador::leer_como_info(const std::string& cadena, std::vector<Rejilla>& rejillas, std::vector<Capa_logica>& capas_logica)
{
	auto partes=Herramientas::explotar(cadena, DEFS::SEPARADOR);
	rejillas.reserve(std::atoi(partes[0].c_str()));
	capas_logica.reserve(std::atoi(partes[1].c_str()));
}

Rejilla * Importador::leer_como_rejilla(const std::string& cadena, std::vector<Rejilla>& rejillas, const Contenedor_tilesets& contenedor_tilesets)
{
	auto partes=Herramientas::explotar(cadena, DEFS::SEPARADOR);

	int w=std::atoi(partes[0].c_str()), 
		h=std::atoi(partes[1].c_str()),
		wc=std::atoi(partes[2].c_str()), 
		hc=std::atoi(partes[3].c_str()),
		ws=std::atoi(partes[4].c_str()), 
		hs=std::atoi(partes[5].c_str()),    
		tile=std::atoi(partes[6].c_str());

	rejillas.push_back(Rejilla(w, h, wc, hc, ws, hs, contenedor_tilesets[tile]));
	return &rejillas.back();
}

void Importador::leer_como_celdas(const std::string& cadena, Rejilla& rejilla)
{
	auto bloques=Herramientas::explotar(cadena, DEFS::SEPARADOR_ITEMS);

	for(auto& b : bloques)
	{
		auto partes=Herramientas::explotar(b, DEFS::SEPARADOR);
		int x=std::atoi(partes[0].c_str()), 
			y=std::atoi(partes[1].c_str()),  
			indice_tile=std::atoi(partes[2].c_str());

		rejilla.r(x, y, Celda(x, y, indice_tile));
	}
}

Capa_logica * Importador::leer_como_capa_logica(const std::string& cadena, std::vector<Capa_logica>& capas_logica, const Contenedor_logica_sets& contenedor_logica_sets)
{
	auto partes=Herramientas::explotar(cadena, DEFS::SEPARADOR);
	size_t set=std::atoi(partes[0].c_str());
	capas_logica.push_back(Capa_logica(contenedor_logica_sets[set]));
	return &capas_logica.back();
}

void Importador::leer_como_objeto_logica(const std::string& cadena, Capa_logica& capa)
{
	auto partes=Herramientas::explotar(cadena, DEFS::SEPARADOR);
	int tipo=std::atoi(partes[0].c_str()), 
		x=std::atoi(partes[1].c_str()),
		y=std::atoi(partes[2].c_str());

	Objeto_logica OBJ(tipo, x, y);

	size_t total=partes.size();
	if(total > 3)
	{
		std::vector<std::string> propiedades;
		
		size_t i=3;
		while(i < total) propiedades.push_back(partes[i++]);
		OBJ.reservar_propiedades(propiedades);
	}
	
	capa.insertar_objeto(OBJ);	
}
