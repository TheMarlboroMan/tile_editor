#include "controlador_propiedades_meta.h"
#include "../app/recursos.h"

using namespace DLibV;

Controlador_propiedades_meta::Controlador_propiedades_meta(Director_estados &DI, const DLibV::Pantalla& p)
	:Controlador_base(DI), 
	rep_txt(p.acc_renderer(), Gestor_superficies::obtener(Recursos_graficos::RS_FUENTE_BASE), "..."),
	propiedades(nullptr), cadena_datos(""), valor_input(""),
	propiedad_actual(0), max_propiedad(0), estado(estados::LISTA)
{
	rep_txt.establecer_posicion(POS_TXT_X, POS_TXT_Y);
}

void Controlador_propiedades_meta::actualizar_cadena_datos()
{
	cadena_datos="META\n-----------------\n\n";

	if(estado!=estados::LISTA && max_propiedad)
	{
		auto &p=(*propiedades)[propiedad_actual];

		cadena_datos+=estado==estados::NOMBRE ? "["+p.acc_nombre()+"]" : p.acc_nombre();
		cadena_datos+=":";
		cadena_datos+=estado==estados::VALOR ? "["+p.acc_valor()+"]" : p.acc_valor();

		cadena_datos+="\n\n-----------------\n\n"+valor_input+"\n\n-----------------\n\n";
	}

	if(propiedades) 
	{
		size_t pos=0;
		for(const auto p : *propiedades) 
		{
			if(pos==propiedad_actual) cadena_datos+=">> ";
			else cadena_datos+="   ";

			cadena_datos+=p.acc_nombre()+" : "+p.acc_valor()+"\n";
			++pos;
		}
	}	

	rep_txt.asignar(cadena_datos);
}

void Controlador_propiedades_meta::preloop(Input_base& input, float delta) 
{
	if(estado!=estados::LISTA) input.iniciar_input_texto();
}
void Controlador_propiedades_meta::postloop(Input_base& input, float delta) 
{
	if(estado!=estados::LISTA) input.finalizar_input_texto();
}

void Controlador_propiedades_meta::loop(Input_base& input, float delta)
{
	if(input.es_senal_salida())
	{
		abandonar_aplicacion();
	}
	else
	{
		if(estado==estados::LISTA)
		{
			if(input.es_input_pulsado(Input::I_CONTROL))
			{
				if(input.es_input_down(Input::I_ABAJO))
				{
					if(propiedad_actual < max_propiedad-1)
					{
						intercambiar_propiedad(propiedad_actual, propiedad_actual+1);
					}
				}
				else if(input.es_input_down(Input::I_ARRIBA))
				{
					if(propiedad_actual > 0) 
					{
						intercambiar_propiedad(propiedad_actual, propiedad_actual-1);
					}
				}
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
				else if(input.es_input_down(Input::I_DELETE))
				{
					eliminar_propiedad(propiedad_actual);
				}
				else if(input.es_input_down(Input::I_NUEVO))
				{
					insertar_propiedad(propiedad_actual);
				}
				else if(input.es_input_down(Input::I_ENTER) && max_propiedad)
				{
					estado=estados::NOMBRE;
					actualizar_cadena_datos();
				}
			}
		}
		else
		{
			if(input.es_input_down(Input::I_ENTER))
			{
				auto &p=(*propiedades)[propiedad_actual];
				if(estado==estados::NOMBRE) p.mut_nombre(valor_input);
				else p.mut_valor(valor_input);

				valor_input="";
				input.vaciar_input_texto();
				actualizar_cadena_datos();
			}
			else if(input.es_input_down(Input::I_ESCAPE))
			{
				estado=estados::LISTA;
				actualizar_cadena_datos();
			}
			else if(input.es_input_down(Input::I_IZQUIERDA) || input.es_input_down(Input::I_DERECHA))
			{
				estado=estado==estados::NOMBRE ? estados::VALOR : estados::NOMBRE;
				actualizar_cadena_datos();
			}
			else if(input.hay_input_texto())
			{
				valor_input=input.acc_input_texto();
				actualizar_cadena_datos();
			}
		}
	}
}

void Controlador_propiedades_meta::eliminar_propiedad(size_t indice)
{
	if(propiedades->size() < indice)
	{
		propiedades->erase(std::begin(*propiedades)+indice);
		if(indice > 0) --propiedad_actual;
		--max_propiedad;
		actualizar_cadena_datos();
	}
}

/*Indice indica el índice actual, que es justo donde se insertará, desplazando
al que hubiera ahí.*/

void Controlador_propiedades_meta::insertar_propiedad(size_t indice)
{
	propiedades->insert(std::begin(*propiedades)+indice, Propiedad_meta("nombre", "valor"));
	++max_propiedad;
	actualizar_cadena_datos();
}

void Controlador_propiedades_meta::dibujar(DLibV::Pantalla& pantalla)
{
	pantalla.limpiar(0, 0, 0, 255);
	rep_txt.volcar(pantalla);
}

void Controlador_propiedades_meta::intercambiar_propiedad(size_t a, size_t b)
{
	std::swap((*propiedades)[a], (*propiedades)[b]);
	propiedad_actual=b;
	actualizar_cadena_datos();
}
