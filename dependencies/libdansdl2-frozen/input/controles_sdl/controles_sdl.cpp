#include "controles_sdl.h"

#include <algorithm>

using namespace DLibI;

Controles_SDL::Controles_SDL():
	senal_salida(false), cantidad_joysticks(0)
{
	SDL_StopTextInput();

	//Esto traga memoria, pero que le den.
	this->teclas_up=new char[SDL_NUM_SCANCODES];
	this->teclas_down=new char[SDL_NUM_SCANCODES];
	this->teclas_down_bloqueo=new char[SDL_NUM_SCANCODES];

	this->inicializar_teclas(true);
	this->inicializar_joysticks();

	hay_eventos_texto=false;
	this->hay_eventos_movimiento_raton=false;
	this->hay_eventos_boton_raton=false;
	this->hay_eventos_teclado_up=false;
	this->hay_eventos_teclado_down=false;
	this->hay_eventos_eje_joystick=false;
	this->hay_eventos_hat_joystick=false;
	this->hay_eventos_boton_joystick_up=false;
	this->hay_eventos_boton_joystick_down=false;
}

Controles_SDL::~Controles_SDL()
{
	if(this->teclas_up) delete[] this->teclas_up;
	if(this->teclas_down) delete[] this->teclas_down;
	if(this->teclas_down_bloqueo) delete[] this->teclas_down_bloqueo;
	this->cerrar_joysticks();

}

void Controles_SDL::inicializar_joysticks()
{
	this->cantidad_joysticks=SDL_NumJoysticks();

	if(this->cantidad_joysticks > 0)
	{
		SDL_JoystickEventState(SDL_ENABLE);

		DLibH::Log_motor::L()<<"Localizados "<<cantidad_joysticks<<" joysticks"<<std::endl;

		for(int i=0; i<cantidad_joysticks; i++)
		{
			SDL_Joystick * joy=SDL_JoystickOpen(i);	//De alguna manera el valgrind saca aquí una pérdida.
			inicializar_joystick(joy, i);
		}
	}
}

void Controles_SDL::inicializar_joystick(SDL_Joystick * estructura, int indice)
{
	SDL_JoystickID id=SDL_JoystickInstanceID(estructura);
	joysticks.insert(std::pair<int, Joystick>(indice, Joystick(id, indice) ) ) ;
	joysticks.at(indice).inicializar(estructura);
	id_joystick_a_indice[id]=indice;

	auto& j=joysticks.at(indice);

	DLibH::Log_motor::L()<<"Inicializado joystick "<<indice<<" con "<<
		j.botones<<" botones, "<<
		j.cantidad_hats<<" hats, "<<
		j.cantidad_ejes<<" ejes."<<std::endl;

}

void Controles_SDL::cerrar_joysticks()
{
	joysticks.clear();
}

void Controles_SDL::inicializar_teclas(bool con_bloqueo)
{
	memset(this->teclas_up, 0, SDL_NUM_SCANCODES);
	memset(this->teclas_down, 0, SDL_NUM_SCANCODES);
	if(con_bloqueo) memset(this->teclas_down_bloqueo, 0, SDL_NUM_SCANCODES);
}

bool Controles_SDL::bombear_eventos_manual(SDL_Event &p_evento, bool p_procesar)
{
	bool resultado=SDL_PollEvent(&p_evento);
	if(p_procesar && resultado) this->procesar_evento(p_evento);
	return resultado;
}

void Controles_SDL::recoger()
{
	this->limpiar_para_nueva_recogida();
			
	//Eventos...
	while(SDL_PollEvent(&eventos))
	{
		this->procesar_evento(eventos);
	}

	//TODO: Actualizar stuff...
	//for(auto& j: joysticks) j.second.debug();

//	raton.manejador();
}

/*
A esto se le llama una vez por cada evento que haya en "recoger". Lo hemos
incluido para poderlo llamar desde otros puntos como el "Escritor_texto" y 
poder seguir usando la clase de Controles desde fuera de la última.
*/

void Controles_SDL::procesar_evento(SDL_Event& evento)
{
	switch(evento.type)
	{
		case SDL_QUIT:
			this->senal_salida=true; 
		break;

		case SDL_MOUSEMOTION:

			this->hay_eventos_movimiento_raton=true;

			raton.x=evento.motion.x; 
			raton.y=evento.motion.y; 
			raton.movimiento=true;
		break;

		case SDL_MOUSEBUTTONDOWN:

			this->hay_eventos_boton_raton=true;
			this->raton.botones_down[evento.button.button]=1;
			this->raton.botones_pulsados[evento.button.button]=1;
		break;

		case SDL_MOUSEBUTTONUP:

			this->hay_eventos_boton_raton=true;
			this->raton.botones_up[evento.button.button]=1;
			this->raton.botones_pulsados[evento.button.button]=0;
		break;

		case SDL_JOYBUTTONDOWN:
			hay_eventos_boton_joystick_down=true;
			joysticks.at(id_joystick_a_indice[evento.jbutton.which]).registrar_boton(0, evento.jbutton.button);
		break;

		case SDL_JOYBUTTONUP:
			hay_eventos_boton_joystick_up=true;
			joysticks.at(id_joystick_a_indice[evento.jbutton.which]).registrar_boton(1, evento.jbutton.button);
		break;

		case SDL_JOYAXISMOTION:
			if(evento.jaxis.value < -MIN_RUIDO || evento.jaxis.value > MAX_RUIDO)
			{
				hay_eventos_eje_joystick=true;
				joysticks.at(id_joystick_a_indice[evento.jbutton.which]).registrar_eje(evento.jaxis.axis, evento.jaxis.value);
//				std::cout<<"EJE "<<evento.jaxis.axis<<" VALOR "<<evento.jaxis.value<<std::endl;
			}
			else
			{
				joysticks.at(id_joystick_a_indice[evento.jbutton.which]).registrar_eje(evento.jaxis.axis, 0);
			}

			if(joysticks.at(id_joystick_a_indice[evento.jhat.which]).ejes_virtualizados)
			{
				establecer_input_virtualizado();
			}
		break;

		case SDL_JOYHATMOTION:
			hay_eventos_hat_joystick=true;
			joysticks.at(id_joystick_a_indice[evento.jhat.which]).registrar_hat(evento.jhat.hat, evento.jhat.value);

			if(joysticks.at(id_joystick_a_indice[evento.jhat.which]).hats_virtualizados)
			{
				establecer_input_virtualizado();
			}

		break;

		case SDL_JOYDEVICEADDED:
		case SDL_CONTROLLERDEVICEADDED:
			DLibH::Log_motor::L()<<"Nuevo joystick detectado..."<<std::endl;
		
			if(!es_joystick_registrado_por_device_id(evento.cdevice.which))
			{
				inicializar_joystick(SDL_JoystickOpen(evento.cdevice.which), joysticks.size());
				++cantidad_joysticks;
				nuevo_joystick_conectado=true;
			}
			else
			{
				DLibH::Log_motor::L()<<"El joystick había sido registrado anteriormente."<<std::endl;
			}
		break;

		case SDL_JOYDEVICEREMOVED:
		case SDL_CONTROLLERDEVICEREMOVED:
			DLibH::Log_motor::L()<<"Retirada de joystick detectada..."<<std::endl;
			joysticks.erase(id_joystick_a_indice[evento.cdevice.which]);
			--cantidad_joysticks;
		break;

		case SDL_TEXTINPUT:
			hay_eventos_texto=true;
			input_text+=evento.text.text;
		break;

//		case SDL_ACTIVEEVENT:
//			this->evento_actividad.recibir_input((bool) evento.active.gain, evento.active.state);
//		break;

		case SDL_KEYDOWN:
		{
			unsigned int indice=evento.key.keysym.scancode;

			if(SDL_IsTextInputActive())
			{
				switch(evento.key.keysym.sym)
				{
					case SDLK_BACKSPACE: 
						if(input_text.length() > 0) 
						{
							input_text.pop_back(); 
							hay_eventos_texto=true;
						}
					break;
					case SDLK_RETURN: input_text+="\n"; break;
				}
			}

			if(!this->teclas_down_bloqueo[indice])
			{
				hay_eventos_teclado_down=true;
				this->teclas_down[indice]=1;
				this->teclas_down_bloqueo[indice]=1;
			}
		}
		break;

		case SDL_KEYUP:
		{
			unsigned int indice=evento.key.keysym.scancode;

			hay_eventos_teclado_up=true;
			this->teclas_up[indice]=1;
			this->teclas_down_bloqueo[indice]=0;
		}
		break;

		default: break;
	}
}

void Controles_SDL::establecer_input_virtualizado()
{
	for(const auto& pj: joysticks)
	{	
		const auto& j=pj.second;

		if(j.hats_virtualizados || j.ejes_virtualizados)
		{
			if(std::any_of(std::begin(j.botones_up), std::end(j.botones_up), [](bool v) {return v;})) hay_eventos_boton_joystick_up=true; 
			if(std::any_of(std::begin(j.botones_down), std::end(j.botones_down), [](bool v) {return v;})) hay_eventos_boton_joystick_down=true; 
//			if(std::any_of(std::begin(j.botones_pulsados), std::end(j.botones_pulsados), [](bool v) {return v;}))
//			if(std::any_of(std::begin(j.botones_soltados), std::end(j.botones_soltados), [](bool v) {return v;}))
		}
	}
}

bool Controles_SDL::es_joystick_registrado_por_device_id(unsigned int d_id)
{
	for(const auto& j : joysticks)
	{
		if(j.second.device_id==d_id) return true;
	}

	return false;
}

void Controles_SDL::limpiar_estado_joysticks()
{
	for(auto& p: joysticks) p.second.inicializar_estado();
}

bool Controles_SDL::comprobacion_boton_joystick(unsigned int p_joystick, unsigned int p_boton) const
{
	if(!joysticks.size()) return false;
	else if(!joysticks.count(p_joystick)) return false;
	else if(p_boton > joysticks.at(p_joystick).botones) return false;
	else return true;
}

bool Controles_SDL::es_joystick_boton_down(unsigned int p_joystick, unsigned int p_boton) const
{
	if(!this->comprobacion_boton_joystick(p_joystick, p_boton)) return false;
	else 
	{
		return joysticks.at(p_joystick).botones_down[p_boton];
	}
}

bool Controles_SDL::es_joystick_boton_up(unsigned int p_joystick, unsigned int p_boton) const
{
	if(!this->comprobacion_boton_joystick(p_joystick, p_boton)) return false;
	else 
	{
		return joysticks.at(p_joystick).botones_up[p_boton];	
	}
}

bool Controles_SDL::es_joystick_boton_pulsado(unsigned int p_joystick, unsigned int p_boton) const
{
	if(!this->comprobacion_boton_joystick(p_joystick, p_boton)) return false;
	else 
	{
		return joysticks.at(p_joystick).botones_pulsados[p_boton];
	}
}

bool Controles_SDL::es_joystick_boton_soltado(unsigned int p_joystick, unsigned int p_boton) const
{
	if(!this->comprobacion_boton_joystick(p_joystick, p_boton)) return false;
	else 
	{
		return joysticks.at(p_joystick).botones_soltados[p_boton];	
	}
}

bool Controles_SDL::comprobacion_eje_joystick(unsigned int p_joystick, unsigned int p_eje) const
{
	if(!joysticks.size()) return false;
	else if(joysticks.count(p_joystick)) return false;
	else if(p_eje > joysticks.at(p_joystick).cantidad_ejes) return false;
	else return true;
}

bool Controles_SDL::comprobacion_hat_joystick(unsigned int p_joystick, unsigned int p_hat) const
{
	if(!joysticks.size()) return false;
	else if(joysticks.count(p_joystick)) return false;
	else if(p_hat > joysticks.at(p_joystick).cantidad_hats) return false;
	else return true;
}

Sint16 Controles_SDL::joystick_eje(unsigned int p_joystick, unsigned int p_eje) const
{
	if(!this->comprobacion_eje_joystick(p_joystick, p_eje)) return false;
	else
	{
		return joysticks.at(p_joystick).ejes[p_eje];
	}
}

int Controles_SDL::joystick_hat(unsigned int p_joystick, unsigned int p_hat) const
{
	if(!this->comprobacion_hat_joystick(p_joystick, p_hat)) return false;
	else
	{
		return joysticks.at(p_joystick).hats[p_hat];
	}
}

void Controles_SDL::limpiar_estado_eventos_actividad()
{
	this->evento_actividad.reiniciar();
}

void Controles_SDL::limpiar_para_nueva_recogida()
{
	hay_eventos_texto=false;
	hay_eventos_movimiento_raton=false;
	hay_eventos_boton_raton=false;
	hay_eventos_teclado_up=false;
	hay_eventos_teclado_down=false;
	hay_eventos_eje_joystick=false;
	hay_eventos_hat_joystick=false;
	hay_eventos_boton_joystick_up=false;
	hay_eventos_boton_joystick_down=false;
	nuevo_joystick_conectado=false;

	//Alimentamos las teclas pulsadas y trabajamos con ellas.
	this->teclado.mut_teclas_pulsadas(SDL_GetKeyboardState(NULL));
	this->inicializar_teclas(false);
	this->limpiar_estado_joysticks();
	this->limpiar_estado_eventos_actividad();
	this->raton.inicializar_estado();
}

bool Controles_SDL::es_evento_quit(SDL_Event &p_evento) const {return p_evento.type==SDL_QUIT;}
bool Controles_SDL::es_evento_mousemotion(SDL_Event &p_evento) const {return p_evento.type==SDL_MOUSEMOTION;}
bool Controles_SDL::es_evento_mousebuttondown(SDL_Event &p_evento) const {return p_evento.type==SDL_MOUSEBUTTONDOWN;}
bool Controles_SDL::es_evento_mousebuttonup(SDL_Event &p_evento) const {return p_evento.type==SDL_MOUSEBUTTONUP;}
bool Controles_SDL::es_evento_keydown(SDL_Event &p_evento) const {return p_evento.type==SDL_KEYDOWN;}
bool Controles_SDL::es_evento_keyup(SDL_Event &p_evento) const {return p_evento.type==SDL_KEYUP;}

int Controles_SDL::obtener_tecla_down() const
{
	int i=0;
	while(i < SDL_NUM_SCANCODES)
	{
		if(teclas_down[i]) return i;
		++i;
	}
	
	return 1;
}

int Controles_SDL::obtener_boton_down() const
{
	unsigned int i=0;
	while(i < Raton::MAX_BOTONES)
	{
		if(raton.botones_down[i]) return i;
		++i;
	}
	return -1;
}

int Controles_SDL::obtener_joystick_boton_down(int indice) const
{
	const Joystick& j=joysticks.at(indice);
	unsigned int i=0;

	while(i < j.botones)
	{
		if(j.botones_down[i]) 
		{
			return i;
		}
		++i;
	}

	return -1;
}
