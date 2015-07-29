#ifndef BASE_SET_H
#define BASE_SET_H

#include "contenedor_ciclico.h"

template <typename T>
class Base_set:public Contenedor_ciclico<T>
{
	private:
	
	int id;
	static int cuenta_id;

	public:

	bool operator==(const Base_set<T>& o) const {return o.id==id;}

	Base_set():
		id(cuenta_id++)
	{}

};

template<typename T> int Base_set<T>::cuenta_id=1;

#endif
