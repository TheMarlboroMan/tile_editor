#include "kernel.h"
#include <fstream>

using namespace DLibH;

extern DLibH::Log_base LOG;

void Kernel_app::inicializar()
{
	using namespace Herramientas_proyecto;
	auto& carg=acc_controlador_argumentos();

	if(carg.size()==1)
	{
		std::ifstream fichero_ayuda("data/recursos/ayuda.txt");

		if(!fichero_ayuda)
		{
			std::cout<<"ERROR: No se localiza el fichero de ayuda..."<<std::endl;
		}
		else
		{
			for(std::string l; std::getline(fichero_ayuda, l); )
				std::cout<<l<<std::endl;
		}

		throw Kernel_exception("No se localizan los parámetros requeridos.");
	}
	try
	{
		try
		{
			LOG<<"setting screen size"<<std::endl;
			auto arg_res=carg.valor_argumento("res");
			auto ex=Herramientas::explotar(arg_res, 'x');

			if(ex.size()!=2)
			{
				throw Kernel_exception("El argumento res debe especficarse como wxh");
			}
			else
			{
				w_pantalla=std::atoi(ex[0].c_str());
				h_pantalla=std::atoi(ex[1].c_str());
			}
		}
		catch(Controlador_argumentos_exception& e)
		{
			w_pantalla=800;
			h_pantalla=600;
		}


		LOG<<"setting out file"<<std::endl;
		//Localizamos el fichero out, por si no se ha especificado poder lanzar la excepción.
		nombre_fichero_salida=carg.valor_argumento("out");

		//En este momento sólo nos interesa localizar los recursos gráficos del fichero de configuración, de modo
		//que lo parseamos someramente. Ya en el bootstrap de la aplicación vamos a intentar localizar información de otro tipo.
		LOG<<"setting config file"<<std::endl;
		nombre_fichero_config_sesion=carg.valor_argumento("cfg");

		LOG<<"opening config file"<<std::endl;
		std::ifstream fichero(nombre_fichero_config_sesion.c_str());

		if(!fichero)
		{
			throw Kernel_exception("El fichero de configuración no puede abrirse.");
		}
		else
		{
			//Una línea del fichero de configuración tiene este aspecto:
			//T	sheet1.dat	#	| data/graficos/tiles_1.png	1	0	0	0
			//Nombre hoja, char comentario, separador, recurso gráfico, transparencia, r g b transparencia.
			//La información la dividiremos en dos, información gráfica y de hojas. De momento vamos
			//sólo con la información gráfica... Sólo vamos a prestar aspecto a las líneas que empiecen
			//por "T", que es el delimitador de tile.

			const char delimitador_tile='T';

			LOG<<"reading config file"<<std::endl;

			for(std::string l; std::getline(fichero, l); )
			{
				if(l[0]==delimitador_tile)
				{
					l=l.substr(l.find("|\t")+2);
					cargador_recursos.asignar_entrada(l);
				}
			}
			fichero.close();
		}

		LOG<<"starting base kernel"<<std::endl;
		Kernel_base::inicializar();
	}
	catch(Controlador_argumentos_exception& e)
	{
		throw Kernel_exception(e.what());
	}
}
