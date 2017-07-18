#include "pantalla.h"

using namespace DLibV;

Pantalla::Pantalla(int p_w, int p_h, unsigned short int p_m):
	ventana(nullptr), renderer(nullptr), volcados(0), 
	w(p_w), h(p_h), modo_ventana(p_m), w_logico(w), h_logico(h)
{
	this->simulacro_caja.w=0;
 	this->simulacro_caja.h=0;
	this->simulacro_caja.x=0;
	this->simulacro_caja.y=0;
}

Pantalla::~Pantalla()
{
	if(this->ventana) SDL_DestroyWindow(this->ventana);
	if(this->renderer) SDL_DestroyRenderer(this->renderer);	 //Esto destruirá texturas asociadas al renderer.
}

void Pantalla::inicializar(int p_w, int p_h, int flags_ventana)
{
	w=p_w;
	h=p_h;
	configurar(flags_ventana);
}

void Pantalla::establecer_titulo(const char * p_cadena)
{
	SDL_SetWindowTitle(ventana, p_cadena);
}

void Pantalla::establecer_titulo(const std::string& c)
{
	SDL_SetWindowTitle(ventana, c.c_str());
}


void Pantalla::rellenar(Uint8 r, Uint8 g, Uint8 b, Uint8 a, SDL_Rect const& p_caja)
{
	//TODO: Si no hay renderer por estar en OpenGL no va a funcionar.
	SDL_SetRenderDrawColor(renderer, r, g, b, a);
	SDL_RenderDrawRect(renderer, &p_caja);
}

void Pantalla::rellenar(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	//TODO: Si no hay renderer por estar en OpenGL no va a funcionar.
	SDL_RenderSetClipRect(renderer, NULL);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);
	SDL_RenderDrawRect(renderer, NULL);
}

void Pantalla::limpiar(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	SDL_RenderSetClipRect(renderer, NULL);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);
	SDL_RenderClear(renderer);
}

void Pantalla::actualizar()
{
	//Uint32 SDL_GetWindowFlags(SDL_Window* window)
//	std::cout<<SDL_GetWindowFlags(ventana)<<std::endl;

//	if(SDL_GetWindowFlags(ventana) & SDL_WINDOW_OPENGL) 
//	{
//		std::cout<<"GL"<<std::endl;
//		SDL_GL_SwapWindow(ventana);
//	}
//	else 
//	{
//		std::cout<<"NO GL"<<std::endl;
		SDL_RenderPresent(renderer);
//	}
	this->volcados=0;
}

void Pantalla::cortar_caja_a_pantalla(SDL_Rect * p_caja)
{
	if(p_caja->x < 0) 
	{
		p_caja->w=p_caja->w-p_caja->x;
		p_caja->x=0;
	}

	if(p_caja->y < 0) 
	{
		p_caja->h=p_caja->h-p_caja->y;
		p_caja->y=0;
	}

	if(p_caja->x+p_caja->w > this->w)
	{
		p_caja->w=this->w-p_caja->w;
	}

	if(p_caja->y+p_caja->h > this->h)
	{
		p_caja->h=this->h-p_caja->h;
	}
}
/*
void Pantalla::preparar_para_camara(Camara const& p_camara)
{
	if(p_camara.es_con_clip()) this->establecer_clip_para_camara(p_camara);
}
*/
void Pantalla::configurar(int flags_ventana)
{
	if(!ventana)
	{	
		ventana=SDL_CreateWindow("", 
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			w, h, flags_ventana); //SDL_WINDOW_FULLSCREEN_DESKTOP);
	}
	else
	{
		SDL_SetWindowSize(ventana, w, h);
	}

	if(!renderer)
	{
		//TODO: NO crear renderer si la flag de OpenGL está activa...
		renderer=SDL_CreateRenderer(ventana, -1, 0);
		establecer_modo_ventana(modo_ventana);
	}
//	establecer_medidas_logicas();
	SDL_RenderSetLogicalSize(renderer, w, h);

	this->simulacro_caja.w=w;
	this->simulacro_caja.h=h;
	this->simulacro_caja.x=0;
	this->simulacro_caja.y=0;
}

void Pantalla::establecer_medidas_logicas(int w, int h)
{
	w_logico=w;
	h_logico=h;
	SDL_RenderSetLogicalSize(renderer, w_logico, h_logico);
}

void Pantalla::establecer_modo_ventana(unsigned int v)
{
	modo_ventana=v;
	switch(modo_ventana)
	{
		case M_VENTANA: SDL_SetWindowFullscreen(ventana, 0); break;
		case M_PANTALLA_COMPLETA_RESOLUCION: SDL_SetWindowFullscreen(ventana, SDL_WINDOW_FULLSCREEN_DESKTOP); break;
		case M_PANTALLA_COMPLETA_SIMULADA: SDL_SetWindowFullscreen(ventana, SDL_WINDOW_FULLSCREEN); break;
	}
}

void Pantalla::reiniciar_clip_completo()
{
	SDL_Rect caja;
	caja.x=0; 
	caja.y=0;
	caja.w=w;
	caja.h=h;

	SDL_RenderSetClipRect(renderer, &caja);
}

void Pantalla::establecer_clip_para_camara(Camara const& p_camara)
{
	//TODO: Si no hay renderer por estar en OpenGL no va a funcionar.
	SDL_Rect caja=p_camara.acc_caja_pos();
	SDL_RenderSetClipRect(renderer, &caja);
}

void Pantalla::establecer_clip(SDL_Rect p_caja)
{
	//TODO: Si no hay renderer por estar en OpenGL no va a funcionar.
	SDL_RenderSetClipRect(renderer, &p_caja);
}

void Pantalla::establecer_posicion(int x, int y)
{
	SDL_SetWindowPosition(ventana, x, y);
}
