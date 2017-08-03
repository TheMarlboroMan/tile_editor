#include "controlador_propiedades_rejilla.h"
#include "../app/recursos.h"
#include <fstream>

using namespace DLibV;

Controlador_propiedades_rejilla::Controlador_propiedades_rejilla(Director_estados &DI, const Pantalla& p)
	:Controlador_base(DI),
	rep_txt(Gestor_superficies::obtener(Recursos_graficos::RS_FUENTE_BASE), "..."),
	indice(0),
	grabar(false), seleccion_actual(0)
{
	for(size_t i=iw; i<imax; i++) inputs.push_back("");
	old_valores=inputs;
	generar_cadena();
}

void Controlador_propiedades_rejilla::generar_cadena()
{
	std::string sel_w=seleccion_actual==sw ? ">" : " ",
		sel_h=seleccion_actual==sh ? ">" : " ",
		sel_wcell=seleccion_actual==swcell ? ">" : " ",
		sel_hcell=seleccion_actual==shcell ? ">" : " ",
		sel_wsep=seleccion_actual==swsep ? ">" : " ",
		sel_hsep=seleccion_actual==shsep ? ">" : " ",
		sel_alpha=seleccion_actual==salpha ? ">" : " ",
		sel_nombre_fichero=seleccion_actual==snombre_fichero ? ">" : " ",
		sel_salir=seleccion_actual==ssalir ? ">" : " ";

	std::string l="Actualizando propiedades de rejilla ["+std::to_string(indice)+"] y fichero:\n\n"+
		sel_w+" W ("+old_valores[iw]+"): "+inputs[iw]+"\n"+
		sel_h+" H ("+old_valores[ih]+"): "+inputs[ih]+"\n"+
		sel_wcell+" W celda ("+old_valores[iwcell]+"): "+inputs[iwcell]+"\n"+
		sel_hcell+" H celda ("+old_valores[ihcell]+"): "+inputs[ihcell]+"\n"+
		sel_wsep+" W separador ("+old_valores[iwsep]+"): "+inputs[iwsep]+"\n"+
		sel_hsep+" H separador ("+old_valores[ihsep]+"): "+inputs[ihsep]+"\n"+
		sel_alpha+" Alpha 0-255 ("+old_valores[ialpha]+"): "+inputs[ialpha]+"\n"+
		sel_nombre_fichero+" Nombre ("+old_valores[inombre_fichero]+"): "+inputs[inombre_fichero]+"\n"+
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
				solicitar_cambio_estado(Director_estados::t_estados::REJILLA);
			}
		}
		else if(input.es_input_down(Input::I_BACKSPACE))
		{
			if(seleccion_actual > smin && seleccion_actual < smax)
			{
				if(inputs[seleccion_actual].size()) inputs[seleccion_actual].pop_back();
			}

			input.vaciar_input_texto();
			generar_cadena();
		}
		else if(input.hay_input_texto())
		{
			if(seleccion_actual > smin && seleccion_actual < smax)
			{
				inputs[seleccion_actual]+=input.acc_input_texto();
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

void Controlador_propiedades_rejilla::ajustar_valores(size_t pindex, int pw, int ph, int pwcell, int phcell, int pwsep, int phsep, int palp, const std::string& pnombre)
{
	indice=pindex;

	inputs[iw]=std::to_string(pw);
	inputs[ih]=std::to_string(ph);
	inputs[iwcell]=std::to_string(pwcell);
	inputs[ihcell]=std::to_string(phcell);
	inputs[iwsep]=std::to_string(pwsep);
	inputs[ihsep]=std::to_string(phsep);
	inputs[ialpha]=std::to_string(palp);
	inputs[inombre_fichero]=pnombre;

	old_valores=inputs;

	seleccion_actual=0;
	grabar=false;

	generar_cadena();
}

Controlador_propiedades_rejilla::Datos_intercambio Controlador_propiedades_rejilla::generar_datos_intercambio() const
{
	return Datos_intercambio{
		std::atoi(inputs[iw].c_str()),
		std::atoi(inputs[ih].c_str()),
		std::atoi(inputs[iwcell].c_str()),
		std::atoi(inputs[ihcell].c_str()),
		std::atoi(inputs[iwsep].c_str()),
		std::atoi(inputs[ihsep].c_str()),
		std::atoi(inputs[ialpha].c_str()),
		inputs[inombre_fichero]
	};
}
