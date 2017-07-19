#include "input.h"

void Input::configurar()
{
	//El input I_ESCAPE es la tecla SDLK_ESCAPE. Simple.
	mapa_teclado.insert(std::make_pair(I_ESCAPE, SDL_SCANCODE_ESCAPE));
	mapa_teclado.insert(std::make_pair(I_ESPACIO, SDL_SCANCODE_SPACE));

	mapa_teclado.insert(std::make_pair(I_IZQUIERDA, SDL_SCANCODE_LEFT));
	mapa_teclado.insert(std::make_pair(I_DERECHA, SDL_SCANCODE_RIGHT));
	mapa_teclado.insert(std::make_pair(I_ARRIBA, SDL_SCANCODE_UP));
	mapa_teclado.insert(std::make_pair(I_ABAJO, SDL_SCANCODE_DOWN));

	mapa_teclado.insert(std::make_pair(I_SHIFT, SDL_SCANCODE_LSHIFT));
	mapa_teclado.insert(std::make_pair(I_CONTROL, SDL_SCANCODE_LCTRL));
	mapa_teclado.insert(std::make_pair(I_RSHIFT, SDL_SCANCODE_RSHIFT));
	mapa_teclado.insert(std::make_pair(I_RCONTROL, SDL_SCANCODE_RCTRL));
	mapa_teclado.insert(std::make_pair(I_ALT, SDL_SCANCODE_LALT));

	mapa_teclado.insert(std::make_pair(I_RE_PAG, SDL_SCANCODE_PAGEUP));
	mapa_teclado.insert(std::make_pair(I_AV_PAG, SDL_SCANCODE_PAGEDOWN));
	mapa_teclado.insert(std::make_pair(I_TAB, SDL_SCANCODE_TAB));

	mapa_teclado.insert(std::make_pair(I_CICLAR_REJILLAS_1, SDL_SCANCODE_1));
	mapa_teclado.insert(std::make_pair(I_CICLAR_REJILLAS_2, SDL_SCANCODE_2));
	mapa_teclado.insert(std::make_pair(I_CICLAR_REJILLAS_3, SDL_SCANCODE_3));
	mapa_teclado.insert(std::make_pair(I_CICLAR_REJILLAS_4, SDL_SCANCODE_4));
	mapa_teclado.insert(std::make_pair(I_CICLAR_REJILLAS_5, SDL_SCANCODE_5));
	mapa_teclado.insert(std::make_pair(I_CICLAR_REJILLAS_6, SDL_SCANCODE_6));

	mapa_teclado.insert(std::make_pair(I_SWAP_REJILLAS, SDL_SCANCODE_C));
	mapa_teclado.insert(std::make_pair(I_CICLAR_TILESETS, SDL_SCANCODE_T));
	mapa_teclado.insert(std::make_pair(I_CICLAR_ZOOM, SDL_SCANCODE_Z));
	mapa_teclado.insert(std::make_pair(I_NUEVO, SDL_SCANCODE_N));
	mapa_teclado.insert(std::make_pair(I_DELETE, SDL_SCANCODE_DELETE));
	mapa_teclado.insert(std::make_pair(I_BORRAR_PARTE, SDL_SCANCODE_D));
	mapa_teclado.insert(std::make_pair(I_GUARDAR, SDL_SCANCODE_S));
	mapa_teclado.insert(std::make_pair(I_CARGAR, SDL_SCANCODE_L));
	mapa_teclado.insert(std::make_pair(I_ENTER, SDL_SCANCODE_RETURN));
	mapa_teclado.insert(std::make_pair(I_MODO_AYUDA, SDL_SCANCODE_F1));
	mapa_teclado.insert(std::make_pair(I_MODO_REJILLA, SDL_SCANCODE_F2));
	mapa_teclado.insert(std::make_pair(I_MODO_OBJETOS, SDL_SCANCODE_F3));
	mapa_teclado.insert(std::make_pair(I_MODO_META, SDL_SCANCODE_F4));

	mapa_teclado.insert(std::make_pair(I_SIGUIENTE_TILE, SDL_SCANCODE_RIGHTBRACKET));
	mapa_teclado.insert(std::make_pair(I_ANTERIOR_TILE, SDL_SCANCODE_LEFTBRACKET));

	mapa_raton.insert(std::make_pair(I_CLICK_I, SDL_BUTTON_LEFT));
	mapa_raton.insert(std::make_pair(I_CLICK_D, SDL_BUTTON_RIGHT));


}
