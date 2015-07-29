#ifndef CAPA_LOGICA_H
#define CAPA_LOGICA_H

/*La capa de lógica es el contenedor de los objetos de lógica.*/

#include "objeto_logica.h"
#include "logica.h"

class Capa_logica
{
	private:

	std::vector<Objeto_logica> objetos;
	bool visible_siempre;
	const Logica_set * tipos;

	int indice_actual; //Indice del tipo de objeto seleccionado actualmente. 

	public:

	void intercambiar_visible_siempre() {visible_siempre=!visible_siempre;}
	bool es_visible_siempre() const {return visible_siempre;}
	void insertar_objeto(int pt, int px, int py);
	void insertar_objeto(int px, int py);
	void insertar_objeto(const Objeto_logica& o) {objetos.push_back(o);}

	const std::vector<Objeto_logica>& acc_objetos() const {return objetos;}
	void eliminar_objeto(int px, int py);
	Objeto_logica * obtener_objeto(int px, int py);
	
	void cambiar_gestor(const Logica_set& g) {tipos=&g; indice_actual=1;}

	const Logica_set& acc_gestor() const {return *tipos;}
	int acc_indice_actual() const {return indice_actual;}
	void mut_indice_actual(int val) {indice_actual=val;}

	Capa_logica(const Logica_set& t)
		:visible_siempre(false), tipos(&t), indice_actual(0)
	{}

	static Capa_logica copiar_sin_contenido(const Capa_logica& c)
	{
		return Capa_logica(*c.tipos);
	}
};

#endif
