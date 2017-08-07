#ifndef CONTROLADOR_PROPIEDADES_OBJETO_LOGICA_H
#define CONTROLADOR_PROPIEDADES_OBJETO_LOGICA_H

#include "controlador_base.h"
#include "../app/objeto_logica.h"
#include "../app/logica.h"
#include <video/representacion/representacion_grafica/representacion_texto/representacion_texto_auto.h>
#include <string>

class Controlador_propiedades_objeto_logica:public Controlador_base
{
	//////////////
	//Propiedades
	private:

	static const size_t PROPIEDADES_DEFECTO=4;
	static const size_t INDICE_X=0;
	static const size_t INDICE_Y=1;
	static const size_t INDICE_W=2;
	static const size_t INDICE_H=3;

	static const int POS_TXT_X=32;
	static const int POS_TXT_Y=32;
//	static const int ALTO_LINEA=16;

	Objeto_logica * objeto;
	const Logica * prototipo;
	DLibV::Representacion_texto_auto_estatica rep_txt;
	std::string valor_input;
	std::string cadena_datos;
	size_t propiedad_actual;
	size_t max_propiedad;
	int w;

	bool establecer_propiedad(size_t indice, const std::string& valor);
	void actualizar_cadena_datos();

	///////////////
	//Interface pública.
	public:
	
	Controlador_propiedades_objeto_logica(Director_estados &DI, const DLibV::Pantalla& p);

	virtual void preloop(Input_base& input, float delta) {input.iniciar_input_texto();}
	virtual void postloop(Input_base& input, float delta) {input.finalizar_input_texto();}
	virtual void loop(Input_base& input, float delta);
	virtual void dibujar(DLibV::Pantalla& pantalla);

	void establecer_valores(Objeto_logica& o, const Logica& t);

};

#endif
