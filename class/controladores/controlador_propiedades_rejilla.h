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

	enum t_inputs{iw=0, ih, iwcell, ihcell, iwsep, ihsep, ialpha, inombre_fichero, imax};
	enum t_selecciones{smin=-1, sw, sh, swcell, shcell, swsep, shsep, salpha, snombre_fichero, ssalir, smax};

	size_t indice;
	std::vector<std::string> inputs;
	std::vector<std::string> old_valores;
	bool grabar;
	int seleccion_actual;

	void generar_cadena();

	///////////////
	//Interface pública.
	public:
	
	struct Datos_intercambio
	{
		int w, h, wcell, hcell, wsep, hsep, alpha;
		std::string nombre_fichero;
	};

	Controlador_propiedades_rejilla(Director_estados &DI, const DLibV::Pantalla& p);

	void ajustar_valores(size_t, int, int, int, int, int, int, int, const std::string&);

	virtual void preloop(Input_base& input, float delta) {input.iniciar_input_texto(); input.vaciar_input_texto();}
	virtual void postloop(Input_base& input, float delta) {input.finalizar_input_texto();}
	virtual void loop(Input_base& input, float delta);
	virtual void dibujar(DLibV::Pantalla& pantalla);
	bool es_grabar() const {return grabar;}
	Datos_intercambio generar_datos_intercambio() const;
};

#endif
