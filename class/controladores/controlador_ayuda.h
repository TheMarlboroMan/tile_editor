#ifndef CONTROLADOR_AYUDA_H
#define CONTROLADOR_AYUDA_H

#include "controlador_base.h"
#include <libDan2.h>

class Controlador_ayuda:public Controlador_base
{
	//////////////
	//Propiedades
	private:

	static const int POS_TXT_X=32;
	static const int POS_TXT_Y=32;

	DLibV::Representacion_texto_auto_estatica rep_txt;
	int x, y, max_x, max_y;

	void generar_cadena();

	///////////////
	//Interface pública.
	public:
	
	Controlador_ayuda(Director_estados &DI, const DLibV::Pantalla& p);

	void reiniciar() {x=0; y=0;}

	virtual void preloop(Input_base& input, float delta) {input.iniciar_input_texto();}
	virtual void postloop(Input_base& input, float delta) {input.finalizar_input_texto();}
	virtual void loop(Input_base& input, float delta);
	virtual void dibujar(DLibV::Pantalla& pantalla);
};

#endif
