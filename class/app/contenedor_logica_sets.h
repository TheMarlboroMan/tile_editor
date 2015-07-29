#ifndef CONTENEDOR_LOGICA_SETS_H
#define CONTENEDOR_LOGICA_SETS_H

#include "logica.h"
#include <vector>
#include <stdexcept>

/* El controlador de las colecciones de objetos de lógica. Un vector de
colecciones de objetos de lógica con algún método propio.*/

class Contenedor_logica_sets_exception:public std::runtime_error
{
	public:

	Contenedor_logica_sets_exception(const std::string& c)
		:std::runtime_error(c)
	{}
};

class Contenedor_logica_sets:public Contenedor_ciclico<Logica_set>
{
	private:

	const char SEPARADOR='\t';

	public:

	void insertar_set(const std::string ruta);
};

#endif
