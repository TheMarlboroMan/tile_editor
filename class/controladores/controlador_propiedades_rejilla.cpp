#include "controlador_propiedades_rejilla.h"
#include "../app/recursos.h"
#include <fstream>

using namespace DLibV;

Controlador_propiedades_rejilla::Controlador_propiedades_rejilla(Director_estados &DI, const Pantalla& p)
	:Controlador_base(DI),
	rep_txt(Gestor_superficies::obtener(Recursos_graficos::RS_FUENTE_BASE), "..."),
	w(0), h(0), old_w(0), old_h(0), 
	indice(0),
	nombre_fichero(""), old_nombre_fichero(""), 
	grabar(false), seleccion_actual(0)
{
	inputs={"", "", ""};
	generar_cadena();
}

void Controlador_propiedades_rejilla::generar_cadena()
{
	std::string sel_w=seleccion_actual==sw ? ">" : " ",
		sel_h=seleccion_actual==sh ? ">" : " ",
		sel_nombre_fichero=seleccion_actual==snombre_fichero ? ">" : " ",
		sel_salir=seleccion_actual==ssalir ? ">" : " ";

	std::string l="Actualizando propiedades de rejilla ["+std::to_string(indice)+"] y fichero:\n\n"+
		sel_w+" W ("+std::to_string(old_w)+"): "+inputs[iw]+"\n"+
		sel_h+" H ("+std::to_string(old_h)+"): "+inputs[ih]+"\n"+
		sel_nombre_fichero+" Nombre ("+std::to_string(old_h)+"): "+inputs[inombre_fichero]+"\n"+
		sel_salir+" Aplicar";

	rep_txt.asignar(l);
}

void Controlador_propiedades_rejilla::loop(Input_base& input, float delta)
{
	if(input.es_senal_salida())
	{
		abandonar_aplicacion();
	}
	else
	{
		if(input.es_input_down(Input::I_ESCAPE))
		{
			solicitar_cambio_estado(Director_estados::t_estados::REJILLA);
		}
		else if(input.es_input_down(Input::I_ABAJO))
		{
			++seleccion_actual;
			if(seleccion_actual >= smax) seleccion_actual=smax-1;
			generar_cadena();
		}
		else if(input.es_input_down(Input::I_ARRIBA))
		{
			--seleccion_actual;
			if(seleccion_actual <= smin) seleccion_actual=smin+1;
			generar_cadena();
		}
		else if(input.es_input_down(Input::I_ENTER))
		{
			if(seleccion_actual==ssalir)
			{
				grabar=true;
				w=std::atoi(inputs[iw].c_str());
				h=std::atoi(inputs[ih].c_str());
				nombre_fichero=inputs[inombre_fichero];
				solicitar_cambio_estado(Director_estados::t_estados::REJILLA);
			}
		}
		else if(input.es_input_down(Input::I_BACKSPACE))
		{
			switch(seleccion_actual)
			{
				case iw: inputs[iw].pop_back(); break;
				case ih: inputs[ih].pop_back(); break;
				case inombre_fichero: inputs[inombre_fichero].pop_back(); break;
			}

			input.vaciar_input_texto();
			generar_cadena();
		}
		else if(input.hay_input_texto())
		{
			switch(seleccion_actual)
			{
				case iw: inputs[iw]+=input.acc_input_texto(); break;
				case ih: inputs[ih]+=input.acc_input_texto(); break;
				case inombre_fichero: inputs[inombre_fichero]+=input.acc_input_texto(); break;
			}
			input.vaciar_input_texto();
			generar_cadena();
		}
	}
}

void Controlador_propiedades_rejilla::dibujar(DLibV::Pantalla& pantalla)
{
	pantalla.limpiar(0, 0, 0, 255);
	rep_txt.establecer_posicion(POS_TXT_X, POS_TXT_Y);
	rep_txt.volcar(pantalla);
}

void Controlador_propiedades_rejilla::ajustar_valores(size_t pindex, int pw, int ph, const std::string& pnombre)
{
	indice=pindex;
	w=pw;
	old_w=w;
	h=ph;
	old_h=h;
	nombre_fichero=pnombre;
	old_nombre_fichero=nombre_fichero;

	inputs[iw]=std::to_string(w);
	inputs[ih]=std::to_string(h);
	inputs[inombre_fichero]=nombre_fichero;

	seleccion_actual=0;
	grabar=false;

	generar_cadena();
}
