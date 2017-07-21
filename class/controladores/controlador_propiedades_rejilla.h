#ifndef CONTROLADOR_PROPIEDADES_REJILLA_H
#define CONTROLADOR_PROPIEDADES_REJILLA_H

#include "controlador_base.h"
#include <libDan2.h>

class Controlador_propiedades_rejilla:public Controlador_base
{
	//////////////
	//Propiedades
	private:

	static const int POS_TXT_X=32;
	static const int POS_TXT_Y=32;

	DLibV::Representacion_texto_auto_estatica rep_txt;

	enum t_inputs{iw=0, ih=1, inombre_fichero=2};
	enum t_selecciones{smin=-1, sw, sh, snombre_fichero, ssalir, smax};

	int w, h, old_w, old_h;
	size_t indice;
	std::string nombre_fichero, old_nombre_fichero;
	std::vector<std::string> inputs;
	bool grabar;
	int seleccion_actual;

	void generar_cadena();

	///////////////
	//Interface pública.
	public:
	
	Controlador_propiedades_rejilla(Director_estados &DI, const DLibV::Pantalla& p);

	void ajustar_valores(size_t, int, int, const std::string&);

	virtual void preloop(Input_base& input, float delta) {input.iniciar_input_texto(); input.vaciar_input_texto();}
	virtual void postloop(Input_base& input, float delta) {input.finalizar_input_texto();}
	virtual void loop(Input_base& input, float delta);
	virtual void dibujar(DLibV::Pantalla& pantalla);
	int acc_w() const {return w;}
	int acc_h() const {return h;}
	const std::string& acc_nombre_fichero() const {return nombre_fichero;}
	bool es_grabar() const {return grabar;}
};

#endif
