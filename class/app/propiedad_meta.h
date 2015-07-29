#ifndef PROPIEDAD_META_H
#define PROPIEDAD_META_H

#include <string>

/*Las propiedades meta, si existen, se guardarán en un nodo "INFO" del nivel
y están pensadas para ser informaciones pequeñas, de una sóla línea con una
clave y un valor.
*/

class Propiedad_meta
{
	private:

	std::string nombre;
	std::string valor;

	public:

	Propiedad_meta(const std::string& n, const std::string& v)
		:nombre(n), valor(v)
	{}

	void mut_nombre(const std::string& v) {nombre=v;}
	void mut_valor(const std::string& v) {valor=v;}

	const std::string& acc_nombre() const {return nombre;}
	const std::string& acc_valor() const {return valor;}
};

#endif
