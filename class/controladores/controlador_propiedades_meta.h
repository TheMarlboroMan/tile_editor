#ifndef CONTROLADOR_PROPIEDADES_META_H
#define CONTROLADOR_PROPIEDADES_META_H

#include "controlador_base.h"
#include "../app/propiedad_meta.h"
#include <libDan2.h>
#include <string>
#include <vector>

class Controlador_propiedades_meta:public Controlador_base
{
	//////////////
	//Propiedades
	private:

	enum class estados {LISTA, NOMBRE, VALOR};
	static const int POS_TXT_X=32;
	static const int POS_TXT_Y=32;

	DLibV::Representacion_texto_auto_estatica rep_txt;

	std::vector<Propiedad_meta> * propiedades;
	std::string cadena_datos;
	std::string valor_input;
	size_t propiedad_actual;
	size_t max_propiedad;
	estados estado;
	

	void actualizar_cadena_datos();

	void intercambiar_propiedad(size_t, size_t);
	void eliminar_propiedad(size_t);
	void insertar_propiedad(size_t);

	///////////////
	//Interface pública.
	public:
	
	Controlador_propiedades_meta(Director_estados &DI, const DLibV::Pantalla& p);
	void asignar_propiedades(std::vector<Propiedad_meta>& prop) 
	{
		estado=estados::LISTA;
		propiedades=&prop;
		propiedad_actual=0;
		max_propiedad=propiedades->size();
		valor_input="";
		actualizar_cadena_datos();
	}

	virtual void preloop(Input_base& input, float delta);
	virtual void postloop(Input_base& input, float delta);
	virtual void loop(Input_base& input, float delta);
	virtual void dibujar(DLibV::Pantalla& pantalla);
};

#endif
