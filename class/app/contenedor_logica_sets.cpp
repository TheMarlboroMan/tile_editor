#include "contenedor_logica_sets.h"
#include <class/lector_txt.h>
#include <herramientas/log_base/log_base.h>
#include <herramientas/herramientas/herramientas.h>

using namespace DLibH;
extern Log_base LOG;

/* La estructura de un fichero de tipos de objeto de lógica es:

#tipo	nombre	w_editor h_editor
	#id_param	#nombre	
	#id_param	#nombre	
	#id_param	#nombre	

Atención a los tabuladores para los parámetros de los tipos. Deben estar ahí
para diferenciar un tipo de su propiedad.
*/

void Contenedor_logica_sets::insertar_set(const std::string ruta)
{
	using namespace Herramientas_proyecto;
	Lector_txt L(ruta, '#');

	if(L)
	{		
		Logica_set G;
		Logica TOL(0, 0, 0, false, 0, 0, 0, "");

		while(true)
		{
			const std::string c=L.leer_linea();
			if(!L) break;

			if(!c.size())
			{
				LOG<<"WARNING: Se intenta crear objeto de lógica desde cadena vacía\n";
			}
			else
			{
				auto ex=Herramientas::explotar(c, SEPARADOR);
				auto toi=[](const std::string& c) -> int {return std::atoi(c.c_str());}; 

				if(c[0]!=SEPARADOR)	//Definición de objeto... #tipo	nombre	w_editor h_editor
				{
					if(TOL.acc_tipo()) G.insertar(TOL);	//Insertar cuando tiene id...

					if(ex.size()!=6) LOG<<"WARNING: Detectada cantidad de parámetros incorrectos para inicio de tipo de objeto en ["<<L.obtener_numero_linea()<<"] "<<c<<std::endl;
					else 
					{
						auto str_resizable=ex[4];
						bool resizable=true;

						if(str_resizable=="resizable") resizable=true;
						else if(str_resizable=="fixed") resizable=false;
						else throw std::runtime_error("Valor incorrecto para propiedad resizable");

						auto color=Herramientas::explotar(ex[5], ',');
						if(color.size()!=3) LOG<<"WARNING: Color incorrecto en ["<<L.obtener_numero_linea()<<"] "<<c<<std::endl;

						else TOL=Logica(toi(ex[0]), toi(ex[2]), toi(ex[3]), resizable, toi(color[0]), toi(color[1]), toi(color[2]), ex[1]);
					}
				}
				else	//Parámetro de objeto... #\t	id	nombre
				{
					//Saltamos el tabulado inicial.
					if(ex.size()!=3) LOG<<"WARNING: Detectada cantidad de parámetros incorrectos para propiedad de tipo de objeto en ["<<L.obtener_numero_linea()<<"] "<<c<<std::endl;
					else TOL.insertar_propiedad(ex[1], ex[2]);
				}
			}
		}

		//Insertar el último...
		if(TOL.acc_tipo()) G.insertar(TOL);	

		insertar(G);
	}
	else
	{
		throw new Contenedor_logica_sets_exception("No se ha localizado el fichero "+ruta+" para cargar objetos de lógica");
	}
}
