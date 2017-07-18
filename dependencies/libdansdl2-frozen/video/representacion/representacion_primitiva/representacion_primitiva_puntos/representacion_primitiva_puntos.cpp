#include "representacion_primitiva_puntos.h"
#include <algorithm>

using namespace DLibV;

Representacion_primitiva_puntos::Representacion_primitiva_puntos(int x, int y, Uint8 r, Uint8 g, Uint8 b)
	:Representacion_primitiva(r, g, b)
{
	insertar(x, y);	
	generar_posicion();
}

Representacion_primitiva_puntos::Representacion_primitiva_puntos(Uint8 r, Uint8 g, Uint8 b)
	:Representacion_primitiva(r, g, b)
{
	generar_posicion();
}

Representacion_primitiva_puntos::Representacion_primitiva_puntos(const Representacion_primitiva_puntos& p_otra)
	:Representacion_primitiva(p_otra)
{

}

Representacion_primitiva_puntos& Representacion_primitiva_puntos::operator=(const Representacion_primitiva_puntos& p_otro)
{
	Representacion_primitiva::operator=(p_otro);
	return *this;
}

Representacion_primitiva_puntos::~Representacion_primitiva_puntos()
{

}

void Representacion_primitiva_puntos::insertar(int x, int y)
{
	puntos.push_back(SDL_Point{x, y});
	generar_posicion();
}

void Representacion_primitiva_puntos::limpiar_puntos()
{
	puntos.clear();
	generar_posicion();
}	

void Representacion_primitiva_puntos::generar_posicion()
{
	if(!puntos.size())
	{
		establecer_posicion(0, 0, 0, 0);
	}
	else
	{
		//Del manual: 
		//The value returned indicates whether the element passed as first argument is considered less than the second.
		struct {bool operator() (SDL_Point a, SDL_Point b) {return a.x < b.x;}}fx;
		struct {bool operator() (SDL_Point a, SDL_Point b) {return a.y < b.y;}}fy;

		auto min_x=*std::min_element(std::begin(puntos), std::end(puntos), fx);
		auto max_x=*std::max_element(std::begin(puntos), std::end(puntos), fx);
		auto min_y=*std::min_element(std::begin(puntos), std::end(puntos), fy);
		auto max_y=*std::max_element(std::begin(puntos), std::end(puntos), fy);
		establecer_posicion(min_x.x, min_y.y, max_x.x-min_x.x, max_y.y-min_y.y);
	}
}

bool Representacion_primitiva_puntos::volcado(SDL_Renderer * p_renderer)
{
	if(!es_visible() || !puntos.size()) return false;

	Uint8 alpha=acc_alpha();
	if(alpha) SDL_SetRenderDrawBlendMode(p_renderer, SDL_BLENDMODE_BLEND);
	else SDL_SetRenderDrawBlendMode(p_renderer, SDL_BLENDMODE_NONE);

	SDL_SetRenderDrawColor(p_renderer, acc_r(), acc_g(), acc_b(), alpha);
	SDL_RenderSetClipRect(p_renderer, NULL);

	auto pt=puntos.begin();
	if(puntos.size()==1) SDL_RenderDrawPoint(p_renderer, pt->x, pt->y);
	else SDL_RenderDrawPoints(p_renderer, &(*pt), puntos.size());

	return true;
}

bool Representacion_primitiva_puntos::volcado(SDL_Renderer * p_renderer, const SDL_Rect& p_enfoque, const SDL_Rect& p_posicion, double zoom)
{
	if(!es_visible() || !puntos.size()) return false;

	Uint8 alpha=acc_alpha();
	if(alpha) SDL_SetRenderDrawBlendMode(p_renderer, SDL_BLENDMODE_BLEND);
	else SDL_SetRenderDrawBlendMode(p_renderer, SDL_BLENDMODE_NONE);

	SDL_SetRenderDrawColor(p_renderer, acc_r(), acc_g(), acc_b(), alpha);
	SDL_Rect pos=copia_posicion();
	SDL_Rect clip=p_posicion;

	if(this->es_estatica())
	{
		//Comprobar si, como estática, estaría dentro del enfoque de la cámara (básicamente 0,0 ancho y alto).
		SDL_Rect caja_cam=DLibH::Herramientas_SDL::nuevo_sdl_rect(0, 0, p_enfoque.w, p_enfoque.h);

		if(!DLibH::Herramientas_SDL::rectangulos_superpuestos(caja_cam, pos, true))
		{
			return false;
		}
		else
		{
			pos.x+=p_posicion.x;
			pos.y+=p_posicion.y;

			//Posición absoluta.
			SDL_RenderSetClipRect(p_renderer, &clip);

			//Proceso del zoom...					
/*			if(p_enfoque.w != p_posicion.w || p_enfoque.h != p_posicion.h)
			{
				procesar_zoom(pos, p_posicion, p_enfoque);
			}
*/

			auto pt=puntos.begin();
			if(puntos.size()==1) SDL_RenderDrawPoint(p_renderer, pt->x, pt->y);
			else SDL_RenderDrawPoints(p_renderer, &(*pt), puntos.size());

			return true;
		}
	}
	else
	{
		if(!DLibH::Herramientas_SDL::rectangulos_superpuestos(p_enfoque, pos, true))
		{
			return false;
		}
		else
		{
			int desp_x=p_posicion.x-p_enfoque.x;
			int desp_y=p_posicion.y-p_enfoque.y;

			SDL_RenderSetClipRect(p_renderer, &clip);

			//Proceso del zoom...
/*
			if(p_enfoque.w != p_posicion.w || p_enfoque.h != p_posicion.h)
			{
				float fx=(float) p_posicion.w / (float) p_enfoque.w;
				float fy=(float) p_posicion.h / (float) p_enfoque.h;
				pos.w*=fx;
				pos.h*=fy;
				pos.x*=fx;
				pos.y*=fy;
			}
*/

			auto copia=puntos;
			for(auto& c: copia)
			{
				c.x+=desp_x;
				c.y+=desp_y;
			}

			auto pt=copia.begin();
			if(puntos.size()==1) SDL_RenderDrawPoint(p_renderer, pt->x, pt->y);
			else SDL_RenderDrawPoints(p_renderer, &(*pt), copia.size());

			return true;
		}
	}
}

void Representacion_primitiva_puntos::preparar_posicion()
{
	//No hace nada.
}	
