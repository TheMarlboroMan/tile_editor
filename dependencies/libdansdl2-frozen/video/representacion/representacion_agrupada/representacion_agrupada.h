#ifndef REPRESENTACION_AGRUPADA_H
#define REPRESENTACION_AGRUPADA_H

#include "../representacion.h"
#include "../../../herramientas/herramientas_sdl/herramientas_sdl.h"
#include <vector>

namespace DLibV
{

/*Tiene un vector de varias representaciones. Cuando se lo pasamos a la pantalla
las vuelca en el orden en que se han insertado. Las representacions deben
ser estáticas  o dinámicas, según el tipo de grupo. 

Una vez insertadas, el grupo toma posesión de las representaciones y se encarga 
de destruirlas al finalizar su tiempo de vida.*/

class Representacion_agrupada:public Representacion
{
	public:

				Representacion_agrupada(bool=true);
				Representacion_agrupada(const Representacion_agrupada&);
	virtual 		~Representacion_agrupada();
	virtual bool 		insertar_representacion(Representacion *);

	bool 			es_preparada() const {return true;}

	void 			imponer_alpha() {impone_alpha=true;}
	void 			no_imponer_alpha() {impone_alpha=false;}
	
	void 			imponer_modo_blend() {impone_modo_blend=true;}
	void 			no_imponer_modo_blend() {impone_modo_blend=false;}

	void 			poseer_representaciones() {this->posee_las_representaciones=true;}
	void 			no_poseer_representaciones() {this->posee_las_representaciones=false;}
	
	void 			vaciar_grupo();

	unsigned int 		obtener_cuenta() const {return grupo.size();}

	std::vector<Representacion *>&	obtener_grupo() {return grupo;}

	private:

	bool			posee_las_representaciones;
	bool 			impone_alpha;
	bool 			impone_modo_blend;

	Representacion_agrupada& 	operator=(const Representacion_agrupada &);	//OJO: Es incopiable!.

	protected:

	std::vector<Representacion *> 	grupo;

	bool 				volcado(SDL_Renderer *);
	bool 				volcado(SDL_Renderer *, const SDL_Rect&, const SDL_Rect&, double);


};

}

#endif
