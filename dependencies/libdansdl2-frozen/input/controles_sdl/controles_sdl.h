#ifndef CONTROLES_SDL_H
#define CONTROLES_SDL_H

#include <SDL2/SDL.h>
#include <iostream>
#include <cstring> //Para memset.
#include <cstdlib> //Para abs en windows.
#include <vector>
#include <map>
#include <string>
#include "../../herramientas/log_base/log_base.h"

namespace DLibI
{

class Controles_SDL
{
	public:

	struct Posicion_raton
	{
		int x, y;
		Posicion_raton(): x(0), y(0) {}
	};

	private:

	//El bloque de control es quien interactúa realmente con la capa inferior de la SDL.
	class Teclado
	{
		private:
		const Uint8 * teclas_pulsadas; //Lo manejaremos todo manualmente...

		public:
		Teclado() {teclas_pulsadas=nullptr;}
		//TODO: Esto es un leak???
		~Teclado() {teclas_pulsadas=nullptr;}
		void mut_teclas_pulsadas(const Uint8 * teclas)  {teclas_pulsadas=teclas;}
		Uint8 const * acc_teclas_pulsadas() const {return teclas_pulsadas;}
		bool es_tecla_pulsada(int p_tecla) const {return this->teclas_pulsadas[p_tecla];}
	};

	//Aquí dentro se guarda todo lo que tiene que ver con el raton.

	class Raton
	{
		public:	//Public porque esto si se devuelve es como "const" y es más cómodo que andar con accedentes a este nivel.

		Posicion_raton posicion;

		int& x;	//Referencias, por compatibilidad de cuando x e y flotaban fuera de Posicion.
		int& y;

		//Para los puristas, versiones acc...
		int acc_x() const {return posicion.x;}
		int acc_y() const {return posicion.y;}
		Posicion_raton acc_posicion() const {return posicion;}

		bool movimiento;

		char * botones_up;
		char * botones_down;
		char * botones_pulsados;

		static const unsigned int MAX_BOTONES=6;	
	
		Raton():
			posicion(),
			x(posicion.x), y(posicion.y),
			movimiento(false)
		{
			this->botones_up=new char[MAX_BOTONES];	//0 no es nada, 5 es la última rueda...
			this->botones_down=new char[MAX_BOTONES];

			this->botones_pulsados=new char[MAX_BOTONES];
			memset(this->botones_pulsados, 0, MAX_BOTONES);
		}

		~Raton()
		{
			delete [] this->botones_up;
			delete [] this->botones_down;
			delete [] this->botones_pulsados;
		}

		void inicializar_estado()
		{
			memset(this->botones_up, 0, MAX_BOTONES);
			memset(this->botones_down, 0, MAX_BOTONES);
			this->movimiento=false;
		}
	};

	class Joystick
	{
		public:

		typedef std::vector<bool> vbotones;
		typedef std::vector<Sint16> vejes;
		typedef std::vector<int> vhats;

		SDL_Joystick * 	estructura;
		SDL_JoystickID	id;
		unsigned int	device_id,
			 	botones,
			 	cantidad_ejes,
			 	cantidad_hats;
		vbotones 	botones_up,
			 	botones_down,
			 	botones_pulsados,
			 	botones_soltados;
		vejes		ejes;
		vhats		hats;
		size_t 		hats_virtualizados,
				ejes_virtualizados;
		int		threshold_boton_virtual_eje;
				

		Joystick(SDL_JoystickID id, int device_id)
			:estructura(nullptr), id(id), device_id(device_id), 
			botones(0), cantidad_ejes(0), cantidad_hats(0),
			hats_virtualizados(0), ejes_virtualizados(0),
			threshold_boton_virtual_eje(0)
		{
		}

		/**
		* Los joysticks están metidos en un mapa que hace copia del objeto
		* de forma que se borra y se liberaría la estructura si la emplazamos
		* en el constructor. La solución de momento es pasar la estructura 
		* después de construido el objeto.
		* TODO: Probar deleter...
std::shared_ptr<SDL_Surface>(SDL_LoadBMP(....), [=](SDL_Surface* surface)
{
    SDL_FreeSurface(surface);
});
		*/

		void inicializar(SDL_Joystick * joy)
		{
			estructura=joy;
			botones=SDL_JoystickNumButtons(estructura);
			cantidad_ejes=SDL_JoystickNumAxes(estructura);
			cantidad_hats=SDL_JoystickNumHats(estructura);

			if(botones)
			{
				botones_up.reserve(botones);
				botones_down.reserve(botones);
				botones_pulsados.reserve(botones);
				botones_soltados.reserve(botones);
			}

			if(cantidad_ejes)
			{
				ejes.reserve(cantidad_ejes);
			}

			if(cantidad_hats)
			{
				hats.reserve(cantidad_hats);
			}

			inicializar_estado();	

			/*OJO: No vamos a mover esto a "inicializar estado!!!",
			si lo hacemos se perderá el estado del input de botones
			pulsados o movimientos de ejes!!!.*/

			if(botones)
			{
				botones_up.insert(std::begin(botones_up), botones, false);
				botones_pulsados.insert(std::begin(botones_pulsados), botones, false);
				botones_down.insert(std::begin(botones_down), botones, false);
				botones_soltados.insert(std::begin(botones_soltados), botones, true);
			}

			if(cantidad_ejes) 
			{
				ejes.insert(std::begin(ejes), cantidad_ejes, 0);
			}

			if(cantidad_hats) 
			{
				hats.insert(std::begin(hats), cantidad_hats, SDL_HAT_CENTERED);
			}
		}

		//Crea botones virtuales para los hats.
		void virtualizar_hats()
		{
			if(hats_virtualizados) return;
			int nbotones=4 * cantidad_hats;

			botones_up.insert(std::end(botones_up), nbotones, false);
			botones_pulsados.insert(std::end(botones_pulsados), nbotones, false);
			botones_down.insert(std::end(botones_down), nbotones, false);
			botones_soltados.insert(std::end(botones_soltados), nbotones, true);

			hats_virtualizados=botones; //Primer índice de los hats virtualizados.
			botones+=nbotones;
		}

		void virtualizar_ejes(int threshold_virtual)
		{
			if(ejes_virtualizados) return;

			threshold_boton_virtual_eje=threshold_boton_virtual_eje;
			int nbotones=2 * cantidad_ejes;

			botones_up.insert(std::end(botones_up), nbotones, false);
			botones_pulsados.insert(std::end(botones_pulsados), nbotones, false);
			botones_down.insert(std::end(botones_down), nbotones, false);
			botones_soltados.insert(std::end(botones_soltados), nbotones, true);

			ejes_virtualizados=botones; //Primer índice de los ejes virtualizados.
			botones+=nbotones;
		}

		void registrar_boton(unsigned int v_tipo, unsigned int v_boton)
		{
			if(v_tipo==0)
			{
				this->botones_down[v_boton]=true;
				this->botones_pulsados[v_boton]=true;
				this->botones_soltados[v_boton]=false;
			}
			else
			{
				this->botones_up[v_boton]=true;
				this->botones_soltados[v_boton]=true;
				this->botones_pulsados[v_boton]=false;
			}
		}
	
		void registrar_eje(unsigned int v_eje, Sint16 v_valor)
		{
			this->ejes[v_eje]=v_valor;

			if(ejes_virtualizados)
			{
				size_t indice=ejes_virtualizados + (2 * v_eje);

				if(abs(v_valor) > threshold_boton_virtual_eje)
				{
					std::fill(std::begin(botones_down)+indice, std::begin(botones_down)+indice+2, false);
					std::fill(std::begin(botones_pulsados)+indice, std::begin(botones_pulsados)+indice+2, false);
					std::fill(std::begin(botones_soltados)+indice, std::begin(botones_soltados)+indice+2, true);

					auto f=[this](size_t v_boton)
					{
						botones_down[v_boton]=true;
						botones_pulsados[v_boton]=true;
						botones_soltados[v_boton]=false;
					};

					if(v_valor > 0) f(indice);
					else f(indice+1);
				}
				else
				{
					auto f=[this](size_t v_boton)
					{
						if(botones_pulsados[v_boton]) 
						{
							botones_soltados[v_boton]=true;
							botones_up[v_boton]=true;
						}
					};

					f(indice);
					f(indice+1);

					std::fill(std::begin(botones_pulsados)+indice, std::begin(botones_pulsados)+indice+2, false);
				}
			}
		}

		void registrar_hat(unsigned int v_hat, int v_valor)
		{
			hats[v_hat]=v_valor;

			if(hats_virtualizados)
			{
				size_t indice=hats_virtualizados + (4 * v_hat);

				if(v_valor==SDL_HAT_CENTERED)
				{
					for(size_t helper=0; helper < 4; ++helper)
					{
						if(botones_pulsados[indice+helper]) 
						{
							botones_soltados[indice+helper]=true;
							botones_up[indice+helper]=true;
						}
					}

					std::fill(std::begin(botones_pulsados)+indice, std::begin(botones_pulsados)+indice+4, false);
				}
				else
		 		{
					std::fill(std::begin(botones_down)+indice, std::begin(botones_down)+indice+4, false);
					std::fill(std::begin(botones_pulsados)+indice, std::begin(botones_pulsados)+indice+4, false);
					std::fill(std::begin(botones_soltados)+indice, std::begin(botones_soltados)+indice+4, true);

					auto f=[this](size_t v_boton)
					{
						botones_down[v_boton]=true;
						botones_pulsados[v_boton]=true;
						botones_soltados[v_boton]=false;
					};

					if(v_valor & SDL_HAT_UP) f(indice);
					if(v_valor & SDL_HAT_RIGHT) f(indice+1);
					if(v_valor & SDL_HAT_DOWN) f(indice+2);
					if(v_valor & SDL_HAT_LEFT) f(indice+3);
				}
			}
		}

		void debug()
		{
			std::cout<<std::endl<<"UP\t";
			for(auto v : botones_up) std::cout<<(int(v));
			std::cout<<std::endl<<"PULSA\t";
			for(auto v : botones_pulsados) std::cout<<(int(v));
			std::cout<<std::endl<<"DOWN\t";
			for(auto v : botones_down) std::cout<<(int(v));
			std::cout<<std::endl<<"SOLTA\t";
			for(auto v : botones_soltados) std::cout<<(int(v));
			std::cout<<std::endl;
		}

		void inicializar_estado()
		{
			if(this->botones)
			{
				std::fill(std::begin(botones_up), std::end(botones_up), false);
				std::fill(std::begin(botones_down), std::end(botones_down), false);
			}
		}

		~Joystick()
		{
			if(estructura)
			{
				SDL_JoystickClose(estructura);
			}
		}		
	};

	/*Esta clase controla los eventos de actividad (minimizar, maximizar,
	pérdida de foco...). Será un miembro de la clase Controles de modo que
	siempre estará ahí con los últimos valores que tuviera. Para usarla
	correctamente preguntaremos siempre si hay algún evento de actividad
	y luego, si lo hay, preguntaremos ya por el estado y el foco.

	Conservamos una sana actitud escéptica a que se den varios eventos de
	actividad a la vez. Si lo hubiera tendríamos que cambiar toda la 
	interface pública por una cola de eventos.
	*/

	class Evento_actividad
	{
		private:

		Uint8 estado;
		bool foco;
		bool registrado_evento_actividad;

		public:

		Evento_actividad():estado(0), foco(false), registrado_evento_actividad(false){}
		~Evento_actividad(){}

		void recibir_input(bool p_foco, Uint8 p_estado)
		{
			this->foco=p_foco;
			this->estado=p_estado;
			this->registrado_evento_actividad=true;
		}

		void reiniciar()
		{
			this->registrado_evento_actividad=false;
			this->foco=false;
			this->estado=0;
		}
	
		bool es_registrado_evento_actividad() const {return this->registrado_evento_actividad;}
		bool es_foco() const {return this->foco;}
		Uint8 acc_estado() const {return this->estado;}
	};

	private:

	SDL_Event 			eventos; //Las cosas que ocurran, como cerrar la ventana
	Teclado		 		teclado;
	Raton 				raton;		//Siempre hay un ratón, no?.
	Evento_actividad	 	evento_actividad;

	std::map<int, Joystick> 	joysticks;
	std::map<SDL_JoystickID, int>	id_joystick_a_indice;
	std::string 			input_text;	//El input de texto...

	bool 				senal_salida; //SDL_QUIT; básicamente...
	unsigned short int 		cantidad_joysticks;

	//TODO: Esto debería ser cosa del teclado...
	char * 				teclas_down;
	char * 				teclas_down_bloqueo;
	char * 				teclas_up;

	void 				inicializar_teclas(bool con_bloqueo=false);
	void 				inicializar_joysticks();
	void 				limpiar_estado_joysticks();
	void 				limpiar_estado_eventos_actividad();
	void 				cerrar_joysticks();
	void				inicializar_joystick(SDL_Joystick *, int);
	bool 				comprobacion_boton_joystick(unsigned int, unsigned int) const;
	bool 				comprobacion_eje_joystick(unsigned int, unsigned int) const;
	bool 				comprobacion_hat_joystick(unsigned int, unsigned int) const;
	bool 				es_joystick_registrado_por_device_id(unsigned int);
	void				establecer_input_virtualizado();

	/*Esto registra simplemente si hay algún evento de este tipo por cada
	ciclo de eventos.*/

	bool 				hay_eventos_texto;
	bool 				hay_eventos_movimiento_raton;
	bool 				hay_eventos_boton_raton;
	bool				hay_eventos_teclado_down;
	bool 				hay_eventos_teclado_up;
	bool 				hay_eventos_eje_joystick;
	bool 				hay_eventos_hat_joystick;
	bool 				hay_eventos_boton_joystick_up;
	bool 				hay_eventos_boton_joystick_down;
	bool				nuevo_joystick_conectado;

	/*Registra el foco...*/

	public:

	//enum ESTADOS{E_FOCO_RATON=SDL_APPMOUSEFOCUS, E_FOCO_INPUT=SDL_APPINPUTFOCUS, E_FOCO_ACTIVIDAD=SDL_APPACTIVE};	
	enum VALORES_EJES{MIN_EJE=-32768, MAX_EJE=32767, MIN_RUIDO=-3200, MAX_RUIDO=3200};

				Controles_SDL();
				~Controles_SDL();

	void 			iniciar_input_texto() {SDL_StartTextInput();}
	void 			finalizar_input_texto() {SDL_StopTextInput();}
	void 			vaciar_input_texto() {input_text=std::string("");}
	const std::string 	acc_input_texto() const {return input_text;}
	bool 			es_input_texto_activo() const {return SDL_IsTextInputActive();}

	bool 			bombear_eventos_manual(SDL_Event &, bool=true);
	const Teclado& 		acc_teclado() const {return this->teclado;}
	const SDL_Event& 	acc_eventos() const {return this->eventos;}
	const Raton& 		acc_raton() const {return raton;}
	const Joystick& 	acc_joystick(int indice) const {return joysticks.at(indice);}
	Joystick& 		acc_joystick(int indice) {return joysticks.at(indice);}
	void			virtualizar_hats_joystick(int indice) {joysticks.at(indice).virtualizar_hats();}
	void			virtualizar_ejes_joystick(int indice, int threshold) {joysticks.at(indice).virtualizar_ejes(threshold);}
	void 			recoger();
	bool			es_nuevo_joystick_conectado() const {return nuevo_joystick_conectado;}

	/*Recibe como parámetro una functión f que toma como parámetro un evento y
	devuelve true o false... Si devuelve true se sale del loop. Es una forma 
	de obtener un control a bajo nivel sobre los eventos.

	Un ejemplo de un callback para poder leer manualmente el input que se recibe...  Se
	puede usar para, por ejemplo, aprender las teclas que se quieran usar.

	bool test;
	struct Cosa
	{
		int val;
		bool operator()(SDL_Event& ev)
		{
			if(ev.type==SDL_KEYDOWN)
			{
				std::cout<<"CODIGO DE TECLA ES "<<ev.key.keysym.scancode<<std::endl;
				return true;
			}

			++val;
			std::cout<<"HOLA "<<val<<std::endl;
			return false;
		}
		
		Cosa():val(0) {}		
	}c;

	virtual void turno()
	{
		if(!test)
		{
			Input_base::turno();
		}
		else
		{
			acc_controles_sdl().recoger_callback(c);
			std::cout<<c.val<<std::endl;
			if(c.val > 100) test=false;
		}
	}
	*/

	template <typename TipoFunc> void recoger_callback(TipoFunc& f)
	{
		this->limpiar_para_nueva_recogida();			
		while(SDL_PollEvent(&eventos))
		{
			if(f(eventos)) break;
		}
	}

	void 			limpiar_para_nueva_recogida();
	void 			procesar_evento(SDL_Event &);
	void 			mover_raton(SDL_Window * w, unsigned int p_x, unsigned int p_y) {SDL_WarpMouseInWindow(w, p_x, p_y);}
	bool 			es_tecla_pulsada(int p_tecla) const {return this->teclado.es_tecla_pulsada(p_tecla);}
	bool 			es_tecla_down(int p_tecla) const {return this->teclas_down[p_tecla];}
	bool 			es_tecla_up(int p_tecla) const {return this->teclas_up[p_tecla];}
	bool 			es_boton_up(int p_boton) const {return this->raton.botones_up[p_boton];}
	bool			es_boton_down(int p_boton) const {return this->raton.botones_down[p_boton];}
	bool 			es_boton_pulsado(int p_boton) const {return this->raton.botones_pulsados[p_boton];}
	bool 			es_movimiento_raton() const {return this->raton.movimiento;}
	bool 			es_joystick_boton_down(unsigned int, unsigned int) const;
	bool 			es_joystick_boton_up(unsigned int, unsigned int) const;
	bool 			es_joystick_boton_pulsado(unsigned int, unsigned int) const;
	bool 			es_joystick_boton_soltado(unsigned int, unsigned int) const;
	Sint16 			joystick_eje(unsigned int, unsigned int) const;
	int			joystick_hat(unsigned int, unsigned int) const;
	unsigned short int 	acc_cantidad_joysticks() const {return this->cantidad_joysticks;}

	bool 			es_evento_quit(SDL_Event &) const;
	bool 			es_evento_mousemotion(SDL_Event &) const;
	bool 			es_evento_mousebuttondown(SDL_Event &) const;
	bool 			es_evento_mousebuttonup(SDL_Event &) const;
	bool 			es_evento_keydown(SDL_Event &) const;
	bool 			es_evento_keyup(SDL_Event &) const; 
	bool 			es_senal_salida() const {return senal_salida;}
	Posicion_raton 		obtener_posicion_raton() const {return raton.acc_posicion();}

	bool 			recibe_eventos_texto() const {return this->hay_eventos_texto;}
	bool 			recibe_eventos_raton() const {return this->hay_eventos_movimiento_raton || this->hay_eventos_boton_raton;}
	bool 			recibe_eventos_movimiento_raton() const {return this->hay_eventos_movimiento_raton;}
	bool 			recibe_eventos_boton_raton() const {return this->hay_eventos_boton_raton;}
	bool 			recibe_eventos_teclado() const {return this->hay_eventos_teclado_up || hay_eventos_teclado_down;}
	bool 			recibe_eventos_teclado_down() const {return this->hay_eventos_teclado_down;}
	bool 			recibe_eventos_teclado_up() const {return this->hay_eventos_teclado_up;}
	bool 			recibe_eventos_joystick() const {return this->hay_eventos_eje_joystick || hay_eventos_hat_joystick || this->hay_eventos_boton_joystick_up || this->hay_eventos_boton_joystick_down ;}
	bool 			recibe_eventos_eje_joystick() const {return this->hay_eventos_eje_joystick;}
	bool 			recibe_eventos_hat_joystick() const {return hay_eventos_hat_joystick;}
	bool 			recibe_eventos_boton_joystick() const {return this->hay_eventos_boton_joystick_up || this->hay_eventos_boton_joystick_down;}
	bool 			recibe_eventos_boton_joystick_up() const {return this->hay_eventos_boton_joystick_up;}
	bool 			recibe_eventos_boton_joystick_down() const {return this->hay_eventos_boton_joystick_down;}
	bool 			recibe_eventos_input() const {return this->recibe_eventos_raton() || this->recibe_eventos_teclado() || this->recibe_eventos_joystick();}
	bool 			es_evento_actividad() const {return this->evento_actividad.es_registrado_evento_actividad();}
	bool 			es_foco_evento_actividad() const {return this->evento_actividad.es_foco();}
	Uint8 			acc_estado_evento_actividad() const {return this->evento_actividad.acc_estado();}

	int			obtener_tecla_down() const;
	int			obtener_boton_down() const;
	int			obtener_joystick_boton_down(int) const;
};

} //Fin namespace DLibI

#endif
