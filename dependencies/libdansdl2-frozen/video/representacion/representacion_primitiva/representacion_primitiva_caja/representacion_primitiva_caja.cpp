#include "representacion_primitiva_caja.h"

using namespace DLibV;

Representacion_primitiva_caja_base::Representacion_primitiva_caja_base(const SDL_Rect& p_pos, Uint8 pr, Uint8 pg, Uint8 pb)
	:Representacion_primitiva(pr, pg, pb)
{
	establecer_posicion(p_pos);
	this->preparar_posicion();
}

Representacion_primitiva_caja_base::Representacion_primitiva_caja_base(const Representacion_primitiva_caja_base& p_otra)
	:Representacion_primitiva(p_otra)
{

}

Representacion_primitiva_caja_base& Representacion_primitiva_caja_base::operator=(const Representacion_primitiva_caja_base& p_otro)
{
	Representacion_primitiva::operator=(p_otro);
	return *this;
}

Representacion_primitiva_caja_base::~Representacion_primitiva_caja_base()
{

}

bool Representacion_primitiva_caja_base::volcado(SDL_Renderer * p_renderer)
{
	if(!es_visible()) return false;
	SDL_Rect pos=acc_posicion();

	Uint8 alpha=acc_alpha();
	if(alpha) SDL_SetRenderDrawBlendMode(p_renderer, SDL_BLENDMODE_BLEND);
	else SDL_SetRenderDrawBlendMode(p_renderer, SDL_BLENDMODE_NONE);

	SDL_SetRenderDrawColor(p_renderer, acc_r(), acc_g(), acc_b(), alpha);
	SDL_RenderSetClipRect(p_renderer, NULL);

	if(es_rellena()) SDL_RenderFillRect(p_renderer, &pos);
	else SDL_RenderDrawRect(p_renderer, &pos);
	return true;
}

bool Representacion_primitiva_caja_base::volcado(SDL_Renderer * p_renderer, const SDL_Rect& p_enfoque, const SDL_Rect& p_posicion, double zoom)
{
	if(!es_visible()) return false;

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
	}
	else
	{
		if(!DLibH::Herramientas_SDL::rectangulos_superpuestos(p_enfoque, pos, true))
		{
			return false;
		}

		pos.x-=p_enfoque.x;
		pos.y-=p_enfoque.y;
	}

	pos.x+=p_posicion.x;
	pos.y+=p_posicion.y;

	SDL_RenderSetClipRect(p_renderer, &clip);

	//Proceso del zoom...					
	procesar_zoom(pos, zoom);

	if(es_rellena()) SDL_RenderFillRect(p_renderer, &pos);
	else SDL_RenderDrawRect(p_renderer, &pos);
	return true;
}

void Representacion_primitiva_caja_base::preparar_posicion()
{
	//No hace nada.
}	

Representacion_primitiva_caja& Representacion_primitiva_caja::operator=(const Representacion_primitiva_caja& p_otro)
{
	Representacion_primitiva_caja_base::operator=(p_otro);
	return *this;
}

Representacion_primitiva_caja::Representacion_primitiva_caja(const SDL_Rect& p_pos, Uint8 pr, Uint8 pg, Uint8 pb)
	:Representacion_primitiva_caja_base(p_pos, pr, pg, pb)
{

}

Representacion_primitiva_caja::Representacion_primitiva_caja(const Representacion_primitiva_caja& p_otra)
	:Representacion_primitiva_caja_base(p_otra) 
{

}

Representacion_primitiva_caja_lineas& Representacion_primitiva_caja_lineas::operator=(const Representacion_primitiva_caja_lineas& p_otro)
{
	Representacion_primitiva_caja_base::operator=(p_otro);
	return *this;
}

Representacion_primitiva_caja_lineas::Representacion_primitiva_caja_lineas(const SDL_Rect& p_pos, Uint8 pr, Uint8 pg, Uint8 pb)
	:Representacion_primitiva_caja_base(p_pos, pr, pg, pb)
{

}

Representacion_primitiva_caja_lineas::Representacion_primitiva_caja_lineas(const Representacion_primitiva_caja_lineas& p_otra)
	:Representacion_primitiva_caja_base(p_otra) 
{

}
