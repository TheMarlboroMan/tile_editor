#ifndef PROYECTO_SDL2_DERIVADA_INPUT_H
#define PROYECTO_SDL2_DERIVADA_INPUT_H

/*La clase que controla el input. La interfaz pública la sacamos del Input_base
y es esta:

bool es_senal_salida() const;
bool es_input_down(unsigned int) const;
bool es_input_up(unsigned int) const;
bool es_input_pulsado(unsigned int) const;

bool es_tecla_down(unsigned int i) const;
bool es_tecla_up(unsigned int i) const;
bool es_tecla_pulsada(unsigned int i);

bool hay_eventos_teclado_down();
*/

#include "../base/input_base.h"

class Input:public Input_base
{
	/////////////////
	//Definiciones...

	public:

	enum inputs{
I_ESCAPE=0,
I_ESPACIO,
I_IZQUIERDA,
I_DERECHA,
I_ARRIBA,
I_ABAJO,
I_SHIFT,
I_CONTROL,
I_RSHIFT,
I_RCONTROL,
I_AV_PAG,
I_RE_PAG,
I_TAB,
I_ENTER,
I_MODO_AYUDA,
I_MODO_REJILLA,
I_MODO_OBJETOS,
I_MODO_META,
I_SWAP_REJILLAS,
I_CICLAR_REJILLAS_1,
I_CICLAR_REJILLAS_2,
I_CICLAR_REJILLAS_3,
I_CICLAR_REJILLAS_4,
I_CICLAR_REJILLAS_5,
I_CICLAR_REJILLAS_6,
I_CICLAR_TILESETS,
I_CICLAR_ZOOM,
I_NUEVO,
I_DELETE,
I_GUARDAR,
I_CARGAR,
/*
I_DEL,
I_PASO,
I_NUEVO,
I_GRABAR,
I_CARGAR,
I_ZOOM,
I_DUPLICAR,
I_PRESENTACION*/
I_CLICK_I,
I_CLICK_D
};

	///////////////////
	// Métodos

	public:

	virtual void configurar();	
	Input():Input_base()/*, test(true)*/ {}
/*
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
};

#endif
