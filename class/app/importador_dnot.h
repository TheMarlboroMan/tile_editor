#ifndef IMPORTADOR_DNOT_H
#define IMPORTADOR_DNOT_H

#include <vector>
#include <string>
#include <stdexcept>
#include "propiedad_meta.h"
#include "capa_logica.h"
#include "rejilla.h"
#include "contenedor_tilesets.h"
#include "contenedor_logica_sets.h"

class Importador_dnot_exception:public std::runtime_error
{
	public:
	Importador_dnot_exception(const std::string& e):std::runtime_error(e) {}
};

class Importador_dnot
{
	public:

	void importar(std::vector<Rejilla>& rejillas, std::vector<Capa_logica>& capas_logica, std::vector<Propiedad_meta>& propiedades_meta, const Contenedor_tilesets& contenedor_tilesets, const Contenedor_logica_sets& contenedor_logica_sets, const std::string nombre_fichero);
};

#endif
