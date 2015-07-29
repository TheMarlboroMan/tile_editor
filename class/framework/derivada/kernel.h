#ifndef KERNEL_APP_SDL2
#define KERNEL_APP_SDL2

#include "../base/kernel_base.h"

#include "../derivada/cargador_recursos.h"
#include "../derivada/configuracion.h"
#include "../derivada/input.h"
#include "../derivada/localizador.h"

#include "../../app/recursos.h"

class Kernel_app:public Kernel_base
{
	

	private:

	//Debemos declarar estas cosas, que hacen falta.

	Input input;
	Configuracion configuracion;
	Cargador_recursos cargador_recursos;

	int w_pantalla;
	int h_pantalla;
	std::string nombre_fichero_salida;
	std::string nombre_fichero_config_sesion;

	public:

	std::string acc_nombre_fichero_salida() const {return nombre_fichero_salida;}
	std::string acc_nombre_fichero_config_sesion() const {return nombre_fichero_config_sesion;}

	Kernel_app(DLibH::Controlador_argumentos& CARG)
		:Kernel_base(CARG), w_pantalla(0), h_pantalla(0) 
	{}

	virtual void inicializar();
	virtual void paso() {}		//Se llamará una vez por cada consumo de delta en el loop del kernel base.
	virtual void preloop() {}	//Se llamará antes del consumo de delta en el loop del kernel base.
	virtual void postloop() {}	//Idem, pero despúes.
	virtual std::string acc_titulo_inicial() const {return std::string(" -- TILE EDITOR -- ");}
	virtual bool acc_mostrar_cursor_inicial() const {return true;}
	virtual int acc_w_pantalla_inicial() const {return w_pantalla;}
	virtual int acc_h_pantalla_inicial() const {return h_pantalla;}
	virtual Input_base& acc_input() {return input;}
	virtual Configuracion_base& acc_configuracion() {return configuracion;}
	virtual Cargador_recursos_base& acc_cargador_recursos() {return cargador_recursos;}
	virtual unsigned int acc_recurso_fps() const {return Recursos_graficos::RS_FUENTE_BASE;}
};

#endif
