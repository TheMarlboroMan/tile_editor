#ifndef OBJETO_LOGICA_H
#define OBJETO_LOGICA_H

/*Un objeto_logica se coloca en su propio contenedor, separado de los de las
celdas. Su finalidad es ser cualquier cosa que no cabe dentro de las tiles
como entradas y salidas de niveles, objetos de juego y demás.

Todos tienen un tipo y una posición x e y que se guardan siempre. A partir
de ahí tienen un vector de propiedades personalizadas que irían en función 
del tipo.
*/

#include <map>
#include <vector>
#include <string>

class Objeto_logica
{
	private:

	static unsigned int id_actual;

	unsigned int id;	//Su única utilidad es comparar.
	int tipo, x, y;

	std::map<std::string, std::string> propiedades;

	public:

	Objeto_logica(int pt, int px, int py):
		id(id_actual++), tipo(pt), x(px), y(py)
	{}

	bool operator==(const Objeto_logica& o) const {return id==o.id;}

	unsigned int acc_tipo() const {return tipo;}
	int acc_x() const {return x;}
	int acc_y() const {return y;}

	std::string como_cadena() const;
	void serializar(std::ostream &stream, const char separador) const;

	void mut_x(int v) {x=v;}
	void mut_y(int v) {y=v;}

	void reservar_propiedades(const std::map<std::string, std::string>& valores);

	const std::map<std::string, std::string>& acc_propiedades() const {return propiedades;}
	void asignar_propiedad(const std::string&, const std::string&);
	void movimiento_relativo(int px, int py)
	{
		x+=px;
		y+=py;
	}
};

#endif
