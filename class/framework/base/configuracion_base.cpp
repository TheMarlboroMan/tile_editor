#include "configuracion_base.h"
#include <herramientas/herramientas/herramientas.h>

Configuracion_base::Configuracion_base()
{
}

Configuracion_base::~Configuracion_base()
{

}

void Configuracion_base::iniciar()
{
	this->asignar_valores_por_defecto_base();
	this->asignar_valores_por_defecto();
	this->cargar();
}

void Configuracion_base::asignar_valores_por_defecto_base()
{
	this->pantalla_completa=valor_pantalla_completa_defecto();
	this->modo_hardware=valor_modo_hardware_defecto();
	this->pantalla_doble_buffer=valor_pantalla_doble_buffer_defecto();
	this->pantalla_anyformat=valor_pantalla_anyformat_defecto();
	this->volumen_audio=valor_volumen_audio_defecto();
	this->volumen_musica=valor_volumen_musica_defecto();
	this->audio_ratio=valor_audio_ratio_defecto();
	this->audio_salidas=valor_audio_salidas_defecto();
	this->audio_buffers=valor_audio_buffers_defecto();
	this->audio_canales=valor_audio_canales_defecto();
}

void Configuracion_base::cargar()
{
	std::ifstream archivo(obtener_ruta_archivo().c_str(), std::ios::in);
	const std::string SEPARADOR_ARCHIVO=obtener_separador_archivo();

	if(!archivo.is_open())
	{
		this->grabar();
		this->cargar();
		return;
	}

	while(!archivo.eof())
	{
		std::string cadena=DLibH::Herramientas::obtener_siguiente_linea_archivo(archivo);

		if(cadena.size())
		{
			int pos=cadena.find(SEPARADOR_ARCHIVO);
	
			if(pos!=-1)
			{
				const std::string clave=cadena.substr(0, pos);
				const std::string valor=cadena.substr(pos+1);
				//Si el valor no existe el método base, usamos el método derivado (que puede ser el mismo...).
				//Lo hacemos para evitar tener que llamar al método base desde la derivada.
				if(!Configuracion_base::procesar_clave_y_valor_base(clave, valor))
				{
					procesar_clave_y_valor(clave, valor);
				}
			}						
		}
	}
}

bool Configuracion_base::procesar_clave_y_valor_base(const std::string& clave, const std::string& valor)
{
	if(clave.compare(obtener_clave_version_archivo())==0)
	{
		this->version_archivo=std::atoi(valor.c_str());
		return true;
	}
	else if(clave.compare(obtener_clave_pantalla_completa())==0)
	{
		this->pantalla_completa=std::atoi(valor.c_str());
		return true;
	}
	else if(clave.compare(obtener_clave_modo_hardware())==0)
	{
		this->modo_hardware=std::atoi(valor.c_str());
		return true;
	}
	if(clave.compare(obtener_clave_pantalla_doble_buffer())==0)
	{
		this->pantalla_doble_buffer=std::atoi(valor.c_str());
		return true;
	}
	if(clave.compare(obtener_clave_pantalla_anyformat())==0)
	{
		this->pantalla_anyformat=std::atoi(valor.c_str());
		return true;
	}
	else if(clave.compare(obtener_clave_volumen_audio())==0)
	{
		this->volumen_audio=std::atoi(valor.c_str());
		return true;
	}
	else if(clave.compare(obtener_clave_volumen_musica())==0)
	{
		this->volumen_musica=std::atoi(valor.c_str());
		return true;
	}
	else if(clave.compare(obtener_clave_audio_ratio())==0)
	{
		this->audio_ratio=std::atoi(valor.c_str());
		return true;
	}
	else if(clave.compare(obtener_clave_audio_salidas())==0)
	{
		this->audio_salidas=std::atoi(valor.c_str());
		return true;
	}
	else if(clave.compare(obtener_clave_audio_buffers())==0)
	{
		this->audio_buffers=std::atoi(valor.c_str());
		return true;
	}
	else if(clave.compare(obtener_clave_audio_canales())==0)
	{
		this->audio_canales=std::atoi(valor.c_str());
		return true;
	}
	else 
	{
		return false;
	}
}

void Configuracion_base::grabar()
{
	std::ofstream archivo(obtener_ruta_archivo().c_str(), std::ios::out);
	const std::string SEPARADOR_ARCHIVO=obtener_separador_archivo();

	if(!archivo.is_open()) return;	//Fallo catastrófico...
	grabar_valores_configuracion_base(archivo, SEPARADOR_ARCHIVO);
	grabar_valores_configuracion(archivo, SEPARADOR_ARCHIVO);
	archivo.close();	
}

void Configuracion_base::grabar_valores_configuracion_base(std::ofstream& archivo, const std::string& SEPARADOR_ARCHIVO)
{
	archivo<<obtener_clave_version_archivo()<<SEPARADOR_ARCHIVO<<obtener_version_archivo()<<std::endl;
	archivo<<obtener_clave_pantalla_completa()<<SEPARADOR_ARCHIVO<<this->pantalla_completa<<std::endl;
	archivo<<obtener_clave_modo_hardware()<<SEPARADOR_ARCHIVO<<this->modo_hardware<<std::endl;
	archivo<<obtener_clave_pantalla_doble_buffer()<<SEPARADOR_ARCHIVO<<this->pantalla_doble_buffer<<std::endl;
	archivo<<obtener_clave_pantalla_anyformat()<<SEPARADOR_ARCHIVO<<this->pantalla_anyformat<<std::endl;
	archivo<<obtener_clave_volumen_audio()<<SEPARADOR_ARCHIVO<<this->volumen_audio<<std::endl;
	archivo<<obtener_clave_volumen_musica()<<SEPARADOR_ARCHIVO<<this->volumen_musica<<std::endl;
	archivo<<obtener_clave_audio_ratio()<<SEPARADOR_ARCHIVO<<this->audio_ratio<<std::endl;
	archivo<<obtener_clave_audio_salidas()<<SEPARADOR_ARCHIVO<<this->audio_salidas<<std::endl;
	archivo<<obtener_clave_audio_buffers()<<SEPARADOR_ARCHIVO<<this->audio_buffers<<std::endl;
	archivo<<obtener_clave_audio_canales()<<SEPARADOR_ARCHIVO<<this->audio_canales<<std::endl;
}
