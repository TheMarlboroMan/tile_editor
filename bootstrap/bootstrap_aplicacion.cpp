#include "bootstrap_aplicacion.h"

#include "../class/controladores/controlador_rejilla.h"
#include "../class/controladores/controlador_propiedades_objeto_logica.h"
#include "../class/controladores/controlador_propiedades_meta.h"
#include "../class/controladores/controlador_ayuda.h"
#include "../class/controladores/controlador_propiedades_rejilla.h"
#include "../class/app/recursos.h"
#include "../class/app/contenedor_tilesets.h"
#include "../class/app/contenedor_logica_sets.h"


using namespace App;
using namespace DLibH;

extern Log_base LOG;

void App::loop_aplicacion(Kernel_app& kernel)
{
	Contenedor_tilesets T;
	Contenedor_logica_sets S;

	try
	{
		//Cargar tiles... Asumimos que el fichero funciona porque se ha abierto antes por el kernel...
		unsigned int indice=Cargador_recursos::PRIMER_INDICE;
		const std::string nombre_fichero_config=kernel.acc_nombre_fichero_config_sesion();
		std::ifstream cfg(nombre_fichero_config.c_str());

		//TODO: Esto aquí no debería estar...
		const char delimitador_tile='T';
		const char delimitador_objetos='O';

		for(std::string l; std::getline(cfg, l); )
		{
			if(l[0]=='#') continue;

			auto partes=Herramientas::explotar(l, '\t');

			if(partes.size())
			{
				if(l[0]==delimitador_tile)
				{
					T.insertar_tileset(indice++, partes[1], partes[2][0]);
				}
				else if(l[0]==delimitador_objetos)
				{
					S.insertar_set(partes[1]);
				}
			}
		}
		cfg.close();
	}
	catch(Contenedor_tilesets_exception& e)
	{
		std::cout<<"Error en la carga de tilesets: "<<e.what()<<"\n";
		return;
	}
	catch(Contenedor_logica_sets_exception& e)
	{
		std::cout<<"Error en la carga de objetos de lógica: "<<e.what()<<"\n";
		return;
	}

	if(!T.size())
	{
		LOG<<"ERROR: No hay tilesets disponibles\n";
		return;
	}

	if(!S.size())
	{
		LOG<<"ERROR: No hay objetos de lógica disponibles\n";
		return;
	}

	//Declaraciones de otros valores...
	Director_estados DI;
	const std::string nombre_fichero_salida=kernel.acc_nombre_fichero_salida();

	//Controladores e interfaces.
	Controlador_rejilla C_R(DI, kernel.acc_pantalla(), nombre_fichero_salida, T, S);
	Controlador_propiedades_objeto_logica CPOL(DI, kernel.acc_pantalla());
	Controlador_propiedades_meta CPM(DI, kernel.acc_pantalla());
	Controlador_ayuda CA(DI, kernel.acc_pantalla());
	Controlador_propiedades_rejilla CPR(DI, kernel.acc_pantalla());
	Interface_controlador * IC=&C_R;

	//Últimos pasos antes de arrancar...
	std::ifstream fichero(nombre_fichero_salida.c_str());
	if(fichero) 
	{
		LOG<<"Inicializando con fichero existente "<<nombre_fichero_salida<<"\n";
		C_R.cargar();
		fichero.close();
	}
	else 
	{
		LOG<<"Inicializando con nuevo fichero "<<nombre_fichero_salida<<"\n";
		C_R.inicializar_sin_fichero();
	}

	//Loop principal.
	while(kernel.loop(*IC))
	{
		if(DI.es_cambio_estado())
		{
			switch(DI.acc_estado_actual())
			{
				case Director_estados::t_estados::REJILLA: break;
				case Director_estados::t_estados::PROPIEDADES_OBJETO_LOGICA: 
					C_R.reconstruir_rep_info_con_objeto_logica(C_R.obtener_objeto_logica_actual());
				break;
				case Director_estados::t_estados::PROPIEDADES_META: break;
				case Director_estados::t_estados::AYUDA: break;
				case Director_estados::t_estados::PROPIEDADES_REJILLA:
					if(CPR.es_grabar())
					{
						const auto dat=CPR.generar_datos_intercambio();
						C_R.redimensionar_actual(dat.w, dat.h);
						C_R.establecer_alpha(dat.alpha);
						C_R.establecer_dimensiones_celda(dat.wcell, dat.hcell);
						C_R.establecer_separador(dat.wsep, dat.hsep);
						C_R.cambiar_nombre_fichero(dat.nombre_fichero);
					}
				break;
			}

			switch(DI.acc_estado_deseado())
			{
				case Director_estados::t_estados::REJILLA: IC=&C_R; break;
				case Director_estados::t_estados::PROPIEDADES_OBJETO_LOGICA: 
					CPOL.establecer_valores(C_R.obtener_objeto_logica_actual(), C_R.obtener_tipo_objeto_logica_actual());
					IC=&CPOL;
				break;
				case Director_estados::t_estados::PROPIEDADES_META: 
					CPM.asignar_propiedades(C_R.acc_propiedades_meta());
					IC=&CPM; break;
				break;
				case Director_estados::t_estados::AYUDA: 
					CA.reiniciar();
					IC=&CA; 
				break;
				case Director_estados::t_estados::PROPIEDADES_REJILLA:
					CPR.ajustar_valores(C_R.acc_indice_rejilla(), 
								C_R.acc_w(), 
								C_R.acc_h(),
								C_R.acc_w_celda(), 
								C_R.acc_h_celda(),  
								C_R.acc_w_separador(),
								C_R.acc_h_separador(),
								C_R.acc_alpha_rejilla(), 
								C_R.acc_nombre_fichero());
					IC=&CPR; 
				break;
			}

			DI.confirmar_cambio_estado();
		}
	};

	LOG<<"Finalizado loop de kernel \n";
}
