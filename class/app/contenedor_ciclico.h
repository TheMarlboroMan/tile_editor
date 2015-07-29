#ifndef CONTENEDOR_CICLICO_H
#define CONTENEDOR_CICLICO_H

#include <algorithm>
#include <string>
#include <vector>

template <typename T>
class Contenedor_ciclico
{
	private:

	std::vector<T> items;

	public:

	void insertar(const T& v) {items.push_back(v);}
	void insertar(const T&& v) {items.push_back(v);}

	size_t size() const {return items.size();}

	const std::vector<T>& acc_items() const {return items;}
	const T& at(size_t indice) const {return items.at(indice);}

	template <typename TipoFunc> 
	const T * buscar_unico_callback(TipoFunc& f) const
	{
		auto it=std::find_if(std::begin(items), std::end(items), f);
		if(it!=std::end(items)) 
		{	
			auto& resultado=*it;
			return &resultado;
		}
		else return nullptr;
	}

	const T& operator[](size_t i) const {return items[i];}
	T& operator[](size_t i) {return items[i];}

	const T& obtener_siguiente(const T& o) const
	{
		auto it=std::find_if(std::begin(items), std::end(items), [&o](const T& g) {return g==o;});

		if(it==std::end(items))
		{
			return *std::begin(items);
		}
		else
		{
			++it;
			if(it==std::end(items)) return *std::begin(items);
			else return *it;
		}
	}

	size_t obtener_indice_item(const T& o) const
	{
		size_t indice=0;
	
		for(auto& t : items) 
		{
			if(t==o) break;
			++indice;
		}

		return indice;
	}
};

#endif
