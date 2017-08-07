#include "controlador_propiedades_objeto_logica.h"
#include "../app/recursos.h"
#include <sstream>

using namespace DLibV;

Controlador_propiedades_objeto_logica::Controlador_propiedades_objeto_logica(Director_estados &DI, const Pantalla& p)
	:Controlador_base(DI), 
	rep_txt(Gestor_superficies::obtener(Recursos_graficos::RS_FUENTE_BASE), "..."),
	propiedad_actual(0), w(p.acc_w())
{
	rep_txt.establecer_posicion(POS_TXT_X, POS_TXT_Y);
}

void Controlador_propiedades_objeto_logica::loop(Input_base& input, float delta)
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
			if(propiedad_actual < max_propiedad-1) 
			{
				++propiedad_actual;
				actualizar_cadena_datos();
			}
		}
		else if(input.es_input_down(Input::I_ARRIBA))
		{
			if(propiedad_actual > 0) 
			{
				--propiedad_actual;
				actualizar_cadena_datos();
			}
		}
		else if(input.es_input_down(Input::I_ENTER))
		{
			if(establecer_propiedad(propiedad_actual, valor_input))
			{
				valor_input="";
			}

			input.vaciar_input_texto();
			actualizar_cadena_datos();
		}

		if(input.hay_input_texto())
		{
			valor_input=input.acc_input_texto();
			actualizar_cadena_datos();
		}
	}
}

bool Controlador_propiedades_objeto_logica::establecer_propiedad(size_t indice, const std::string& valor)
{
	switch(indice)
	{
		case INDICE_X: objeto->mut_x(std::atoi(valor.c_str())); break; //0
		case INDICE_Y: objeto->mut_y(std::atoi(valor.c_str())); break; //1
		case INDICE_W: 
			if(prototipo->es_resizable())
			{
				objeto->mut_w(std::atoi(valor.c_str())); break; //2
			}
			else
			{
				valor_input=prototipo->acc_w_editor();
				return false;
			}
		break;
		case INDICE_H:
			if(prototipo->es_resizable())
			{
				objeto->mut_h(std::atoi(valor.c_str())); break; //3
			}
			else
			{
				valor_input=prototipo->acc_h_editor();
				return false;
			}
		break;
		default: //4-...
			//This sucks balls
			objeto->asignar_propiedad(prototipo->nombre_propiedad_por_indice(indice-PROPIEDADES_DEFECTO), valor); 
		break;
	}

	return true;
}

void Controlador_propiedades_objeto_logica::actualizar_cadena_datos()
{
	std::stringstream ss;

	const std::string salto("\n\n");

	if(propiedad_actual==INDICE_X) ss<<"* X : ["<<objeto->acc_x()<<"] >> "<<valor_input<<salto;
	else ss<<"X : "<<objeto->acc_x()<<salto;

	if(propiedad_actual==INDICE_Y) ss<<"* Y : ["<<objeto->acc_y()<<"] >> "<<valor_input<<salto;
	else ss<<"Y : "<<objeto->acc_y()<<salto;

	if(prototipo->es_resizable())
	{
		if(propiedad_actual==INDICE_W) ss<<"* W : ["<<objeto->acc_w()<<"] >> "<<valor_input<<salto;
		else ss<<"W : "<<objeto->acc_w()<<salto;

		if(propiedad_actual==INDICE_H) ss<<"* H : ["<<objeto->acc_h()<<"] >> "<<valor_input<<salto;
		else ss<<"H : "<<objeto->acc_h()<<salto;
	}
	else
	{
		if(propiedad_actual==INDICE_W) ss<<"* W : "<<prototipo->acc_w_editor()<<" [protegido]"<<salto;
		else ss<<"W : "<<prototipo->acc_w_editor()<<" [protegido]"<<salto;

		if(propiedad_actual==INDICE_H) ss<<"* H : "<<prototipo->acc_h_editor()<<" [protegido]"<<salto;
		else ss<<"H : "<<prototipo->acc_h_editor()<<" [protegido]"<<salto;
	}

	size_t i=0;
	for(const auto& p : objeto->acc_propiedades()) 
	{
		if(i==propiedad_actual-PROPIEDADES_DEFECTO) ss<<"* "<<p.first<<" : ["<<p.second<<"] >> "<<valor_input<<salto;
		else ss<<p.first<<" : "<<p.second<<salto;
		++i;
	}

	cadena_datos=ss.str();
	rep_txt.asignar(cadena_datos);
}

void Controlador_propiedades_objeto_logica::dibujar(DLibV::Pantalla& pantalla)
{
	pantalla.limpiar(0, 0, 0, 255);
	rep_txt.volcar(pantalla);
}

void Controlador_propiedades_objeto_logica::establecer_valores(Objeto_logica& o, const Logica& t)
{
	objeto=&o;
	prototipo=&t;
	valor_input="";
	propiedad_actual=0;
	max_propiedad=PROPIEDADES_DEFECTO+prototipo->acc_total_propiedades();
	actualizar_cadena_datos();
}
