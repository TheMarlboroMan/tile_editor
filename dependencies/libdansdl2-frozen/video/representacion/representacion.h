#ifndef REPRESENTACION_H
#define REPRESENTACION_H

#include <SDL2/SDL.h>
#include "../camara/camara.h"
#include "../pantalla/pantalla.h"
#include "../../herramientas/herramientas_sdl/herramientas_sdl.h"

namespace DLibV
{

class Representacion
{
	public:

	enum blends {BLEND_NADA,BLEND_ALPHA,BLEND_SUMA,BLEND_MODULAR};

	private:

	bool 			estatica;
	 	//Indica si sería afectada por la cámara. Es la única diferencia entre algunas clases.
		//Hay que entenderlo en el contexto de que puede dibujarse "dentro" de una cámara
		//pero por más que esta mueva su enfoque se mantendrá siempre en el mismo lugar.
	bool 			visible;
	Uint8 			alpha;
	unsigned short int 	modo_blend;
	unsigned short int 	mod_color_r;
	unsigned short int 	mod_color_g;
	unsigned short int 	mod_color_b;

	SDL_Rect 		posicion; 	//Lugar en que se muestra de la pantalla.
	SDL_Rect 		recorte;	//Considerando la dimensión total de la representación, la parte que mostramos.

	protected:

	void 			reiniciar_posicion();
	void 			reiniciar_recorte();
	void 			reiniciar_rect(SDL_Rect&);


				Representacion();
	//			Representacion(Uint8);
				Representacion(const Representacion&);
	Representacion& 	operator=(const Representacion &);

	virtual bool 		volcado(SDL_Renderer *, const SDL_Rect&, const SDL_Rect&, double)=0;
	virtual bool 		volcado(SDL_Renderer *)=0;
	
	public:

	virtual 		~Representacion();
	enum FLAGS_RECT{FRECT_X=1, FRECT_Y=2, FRECT_W=4, FRECT_H=8};

	bool 			es_estatica() const {return estatica;}
	void			hacer_estatica() {estatica=true;}
	void			hacer_dinamica() {estatica=false;}

	bool 			en_toma(const SDL_Rect& p_caja) const {return DLibH::Herramientas_SDL::rectangulos_superpuestos(p_caja, this->posicion, true);} 
	bool 			es_en_posicion(Sint16 p_x, Sint16 p_y) const 
	{
		return this->posicion.x==p_x && 
		this->posicion.y==p_y;
	}

	const SDL_Rect& 	acc_posicion() const {return this->posicion;}
	const SDL_Rect 		copia_posicion() const 
	{
		SDL_Rect res;
		res.x=posicion.x;
		res.y=posicion.y;
		res.w=posicion.w;
		res.h=posicion.h;
		return res;
	}
	virtual void		establecer_posicion(int, int, int=-1, int=-1, int=15);
	virtual void 		establecer_posicion(SDL_Rect);

	const SDL_Rect& 	acc_recorte() const {return this->recorte;}
	const SDL_Rect 		copia_recorte() const 
	{
		SDL_Rect res;
		res.x=recorte.x;
		res.y=recorte.y;
		res.w=recorte.w;
		res.h=recorte.h;
		return res;
	}
	void 			establecer_recorte(Sint16, Sint16, Uint16, Uint16, int=15);
	void 			establecer_recorte(SDL_Rect);

	void 			establecer_dimensiones_posicion_por_recorte();

	virtual void 		ir_a(int x, int y){establecer_posicion(x,y);} //Es virtual porque algunas igual redefinen el comportamiento (especialmente las primitivas....
	void 			desplazar(Sint16 p_x, Sint16 p_y);					
	void 			hacer_invisible() {this->visible=false;}
	void 			hacer_visible() {this->visible=true;}
	void 			intercambiar_visibilidad() {this->visible=!this->visible;}
	void 			cambiar_visibilidad(bool p_valor) {this->visible=p_valor;}
	bool 			es_visible() const {return this->visible;}

	//Se pasa el rectángulo de pantalla... Básicamente se comprueba si está dentro. Estática o no.
	bool 			volcar(SDL_Renderer *, const SDL_Rect&);
	bool 			volcar(SDL_Renderer *, const Camara&);
	bool 			volcar(const Pantalla&, const Camara&);
	bool 			volcar(const Pantalla&);
	bool 			volcar(SDL_Renderer *, const SDL_Rect&, const SDL_Rect&, double);
	bool 			volcar(SDL_Renderer *);

	void 			establecer_alpha(Uint8 p_valor) {this->alpha=p_valor;}
	Uint8 			acc_alpha() const {return this->alpha;}
	void 			establecer_modo_blend(unsigned short int v) {this->modo_blend=v;}
	unsigned short int 	acc_modo_blend() const {return this->modo_blend;}
	static void 		procesar_zoom(SDL_Rect& pos, const SDL_Rect& p_posicion, const SDL_Rect& p_enfoque);
	static void 		procesar_zoom(SDL_Rect&, double);

	unsigned short int 	acc_mod_color_r() const {return mod_color_r;}
	unsigned short int 	acc_mod_color_g() const {return mod_color_g;}
	unsigned short int 	acc_mod_color_b() const {return mod_color_b;}
	void 			establecer_mod_color(unsigned int pr, unsigned int pg, unsigned int pb)
	{
		mod_color_r=pr;		
		mod_color_g=pg;
		mod_color_b=pb;
	}
};

} //Fin namespace DLibV

#endif
