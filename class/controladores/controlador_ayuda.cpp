#include "controlador_ayuda.h"
#include "../app/recursos.h"
#include <fstream>

using namespace DLibV;

Controlador_ayuda::Controlador_ayuda(Director_estados &DI, const Pantalla& p)
	:Controlador_base(DI),
	rep_txt(Gestor_superficies::obtener(Recursos_graficos::RS_FUENTE_BASE), "..."),
	x(0), y(0), max_x(0), max_y(0)
{
	generar_cadena();
}

void Controlador_ayuda::generar_cadena()
{
	std::ifstream fichero_ayuda("data/recursos/ayuda_interna.txt");

	std::string l;

	if(!fichero_ayuda)
	{
		l="ERROR: No se localiza el fichero de ayuda...";
	}
	else
	{
		for(std::string cad; std::getline(fichero_ayuda, cad); )
			l+=cad+"\n";
	}

	rep_txt.asignar(l);
	max_y=-rep_txt.acc_alto_area();
	max_x=-rep_txt.acc_ancho_area();
}

void Controlador_ayuda::loop(Input_base& input, float delta)
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
		else if(input.es_input_pulsado(Input::I_ABAJO))
		{
			--y;
			if(y < max_y) y=max_y;
		}
		else if(input.es_input_pulsado(Input::I_ARRIBA))
		{
			++y;
			if(y > 0) y=0;
		}
		else if(input.es_input_pulsado(Input::I_DERECHA))
		{
			--x;
			if(x < max_x) x=max_x;
		}
		else if(input.es_input_pulsado(Input::I_IZQUIERDA))
		{
			++x;
			if(x > 0) x=0;
		}
	}
}

void Controlador_ayuda::dibujar(DLibV::Pantalla& pantalla)
{
	pantalla.limpiar(0, 0, 0, 255);
	rep_txt.establecer_posicion(POS_TXT_X+x, POS_TXT_Y+y);
	rep_txt.volcar(pantalla);
}
