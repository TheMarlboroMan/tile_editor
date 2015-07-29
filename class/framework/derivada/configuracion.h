#ifndef PROYECTO_SDL2_DERIVADA_CONFIGURACION_H
#define PROYECTO_SDL2_DERIVADA_CONFIGURACION_H

#include "../base/configuracion_base.h"

class Configuracion: public Configuracion_base
{
	////////////////////////////////////
	// Definiciones...

	static const std::string CLAVE_IDIOMA;

	////////////////////////////////////
	// Implementacion...

	protected: 

	virtual void grabar_valores_configuracion(std::ofstream&, const std::string&);
	virtual void asignar_valores_por_defecto();
	virtual void procesar_clave_y_valor(const std::string&, const std::string&);

	std::string obtener_ruta_archivo() const {return "data/config/configuracion.dat";}
	std::string obtener_separador_archivo() const {return ":";}
	std::string obtener_clave_version_archivo() const {return "v";}
	std::string obtener_version_archivo() const {return "1";}
	std::string obtener_clave_pantalla_completa() const {return "pantalla_completa";}
	std::string obtener_clave_modo_hardware() const {return "modo_hardware";}
	std::string obtener_clave_pantalla_doble_buffer() const {return "doble_buffer";}
	std::string obtener_clave_pantalla_anyformat() const {return "pantalla_anyformat";}
	std::string obtener_clave_volumen_audio() const {return "volumen_audio";}
	std::string obtener_clave_volumen_musica() const {return "volumen_musica";}
	std::string obtener_clave_audio_ratio() const {return "audio_ratio";}
	std::string obtener_clave_audio_buffers() const {return "audio_buffers";}
	std::string obtener_clave_audio_salidas() const {return "audio_salidas";}
	std::string obtener_clave_audio_canales() const {return "audio_canales";}
	unsigned short int valor_pantalla_completa_defecto() const {return 0;}
	unsigned short int valor_modo_hardware_defecto() const {return 1;}
	unsigned short int valor_pantalla_doble_buffer_defecto() const {return 1;}
	unsigned short int valor_pantalla_anyformat_defecto() const {return 1;}
	unsigned short int valor_volumen_audio_defecto() const {return 64;}
	unsigned short int valor_volumen_musica_defecto() const {return 64;}
	int valor_audio_ratio_defecto() const {return 44100;}
	int valor_audio_salidas_defecto() const {return 2;}
	int valor_audio_buffers_defecto() const {return 1024;}
	int valor_audio_canales_defecto() const {return 8;}

	///////////////////////////////////
	// Propiedades.

	private:

	unsigned int idioma;

	////////////////////////////////
	// Interface pública.

	public:

	unsigned short int acc_idioma() const {return this->idioma;}
	void mut_idioma(unsigned short int p_valor) {this->idioma=p_valor;}

	Configuracion();
	~Configuracion();
};

#endif
