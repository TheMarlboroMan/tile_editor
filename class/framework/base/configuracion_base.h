#ifndef PROYECTO_SDL2_BASE_CONFIGURACION_H
#define PROYECTO_SDL2_BASE_CONFIGURACION_H

/*
Esta clase lee y escribe en un archivo los datos de configuración de la 
aplicación. Al leer los datos de configuración los almacena entre sus 
propiedades. Podemos (y debemos) extenderla para la configuración de cada
proyecto implementando todos los métodos virtuales puros que tiene.
*/

#include <fstream>

class Configuracion_base
{
	////////////////////////////////////
	// A implementar por clases que la extiendan...

	protected: 

		//Estos tienen que hacer la misma función que los métodos _base.
	virtual void grabar_valores_configuracion(std::ofstream&, const std::string&)=0;
	virtual void asignar_valores_por_defecto()=0;
	virtual void procesar_clave_y_valor(const std::string&, const std::string&)=0;

		//Estos simplemente devuelven claves de configuración.
	virtual std::string obtener_ruta_archivo() const=0;
	virtual std::string obtener_separador_archivo() const=0;
	virtual std::string obtener_clave_version_archivo() const=0;
	virtual std::string obtener_version_archivo() const=0;
	virtual std::string obtener_clave_pantalla_completa() const=0;
	virtual std::string obtener_clave_modo_hardware() const=0;
	virtual std::string obtener_clave_pantalla_doble_buffer() const=0;
	virtual std::string obtener_clave_pantalla_anyformat() const=0;
	virtual std::string obtener_clave_volumen_audio() const=0;
	virtual std::string obtener_clave_volumen_musica() const=0;
	virtual std::string obtener_clave_audio_ratio() const=0;
	virtual std::string obtener_clave_audio_buffers() const=0;
	virtual std::string obtener_clave_audio_salidas() const=0;
	virtual std::string obtener_clave_audio_canales() const=0;

		//Y estos valores de configuración por defecto.
	virtual unsigned short int valor_pantalla_completa_defecto() const=0; //0 -> No / 1 -> Si
	virtual unsigned short int valor_modo_hardware_defecto() const=0; //0 -> No / 1 -> Si
	virtual unsigned short int valor_pantalla_doble_buffer_defecto() const=0;	//0 -> No / 1 -> Si
	virtual unsigned short int valor_pantalla_anyformat_defecto() const=0;	//0 -> No / 1 -> Si
	virtual unsigned short int valor_volumen_audio_defecto() const=0;	//0-128...
	virtual unsigned short int valor_volumen_musica_defecto() const=0;	//0-128...
	virtual int valor_audio_ratio_defecto() const=0; //Por ejemplo... 44100.
	virtual int valor_audio_salidas_defecto() const=0; //1 -> mono, 2 -> stereo.
	virtual int valor_audio_buffers_defecto() const=0; //Por ejemplo... 1024;
	virtual int valor_audio_canales_defecto() const=0; //Por ejemplo... 8.

	///////////////////////////////////
	// Propiedades.

	private:

	unsigned short int version_archivo;
	unsigned short int pantalla_completa;
	unsigned short int modo_hardware;
	unsigned short int pantalla_doble_buffer;
	unsigned short int pantalla_anyformat;
	int volumen_audio;
	int volumen_musica;
	int audio_ratio;
	int audio_salidas;
	int audio_buffers;
	int audio_canales;

	////////////////////////////////
	// Métodos internos.
	
	private:	
	void grabar_valores_configuracion_base(std::ofstream&, const std::string&);
	void asignar_valores_por_defecto_base();
	bool procesar_clave_y_valor_base(const std::string&, const std::string&);

	////////////////////////////////
	// Interface pública.

	public:

	unsigned short int acc_pantalla_completa() const {return this->pantalla_completa;}
	unsigned short int acc_modo_hardware() const {return this->modo_hardware;}
	unsigned short int acc_pantalla_doble_buffer() const {return this->pantalla_doble_buffer;}
	unsigned short int acc_pantalla_anyformat() const {return this->pantalla_anyformat;}
	int acc_volumen_audio() const {return this->volumen_audio;}
	int acc_volumen_musica() const {return this->volumen_musica;}
	int acc_audio_ratio() const {return this->audio_ratio;}
	int acc_audio_salidas() const {return this->audio_salidas;}
	int acc_audio_buffers() const {return this->audio_buffers;}
	int acc_audio_canales() const {return this->audio_canales;}
	int acc_version_archivo() const {return this->version_archivo;}

	void mut_modo_hardware(unsigned short int p_valor) {this->modo_hardware=p_valor;}
	void mut_pantalla_completa(unsigned short int p_valor) {this->pantalla_completa=p_valor;}
	void mut_pantalla_doble_buffer(unsigned short int p_valor) {this->pantalla_doble_buffer=p_valor;}
	void mut_pantalla_anyformat(unsigned short int p_valor) {this->pantalla_anyformat=p_valor;}
	void mut_volumen_audio(int p_valor) {this->volumen_audio=p_valor;}
	void mut_volumen_musica(int p_valor) {this->volumen_musica=p_valor;}
	void cargar();
	void grabar();
	void iniciar();

	Configuracion_base();
	virtual ~Configuracion_base();
};

#endif
