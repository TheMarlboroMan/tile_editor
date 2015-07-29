#ifndef IMPORTADOR_H
#define IMPORTADOR_H

#include <vector>
#include <string>
#include <stdexcept>
#include "propiedad_meta.h"
#include "capa_logica.h"
#include "rejilla.h"
#include "contenedor_tilesets.h"
#include "contenedor_logica_sets.h"

class Importador_exception:public std::runtime_error
{
	public:
	Importador_exception(const std::string& e):std::runtime_error(e) {}
};

class Importador
{
	private:

	void leer_como_info(const std::string& cadena, std::vector<Rejilla>& rejillas, std::vector<Capa_logica>& capas_logica);
	Rejilla * leer_como_rejilla(const std::string& cadena, std::vector<Rejilla>& rejillas, const Contenedor_tilesets& contenedor_tilesets);
	Capa_logica * leer_como_capa_logica(const std::string& cadena, std::vector<Capa_logica>& capas_logica, const Contenedor_logica_sets& contenedor_logica_sets);
	void leer_como_celdas(const std::string& cadena, Rejilla& rejilla);
	void leer_como_objeto_logica(const std::string& cadena, Capa_logica& capa);
	void leer_como_meta(const std::string& cadena, std::vector<Propiedad_meta>& propiedades);

	public:

	void importar(std::vector<Rejilla>& rejillas, std::vector<Capa_logica>& capas_logica, std::vector<Propiedad_meta>& propiedades_meta, const Contenedor_tilesets& contenedor_tilesets, const Contenedor_logica_sets& contenedor_logica_sets, const std::string nombre_fichero);
};

#endif
