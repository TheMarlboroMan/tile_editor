#ifndef EXPORTADOR_DNOT_H
#define EXPORTADOR_DNOT_H

#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>
#include "rejilla.h"
#include "propiedad_meta.h"
#include "contenedor_tilesets.h"
#include "contenedor_logica_sets.h"
#include "capa_logica.h"

class Exportador_dnot_exception:public std::runtime_error
{
	public:
	Exportador_dnot_exception(const std::string& e):std::runtime_error(e) {}
};

//This is pretty stupid... A class with a single method. Hooray.

class Exportador_dnot
{
	public:

	void exportar(const std::vector<Rejilla>& rejillas, const std::vector<Capa_logica>& capas_logica, const std::vector<Propiedad_meta>& propiedades_meta, 
		const Contenedor_tilesets& contenedor_tilesets, const Contenedor_logica_sets& contenedor_logica_sets, const std::string nombre_fichero);

//	void serializar_rejilla(const Rejilla&);
//	void serializar 
};

#endif
