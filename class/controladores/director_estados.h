#ifndef DIRECTOR_ESTADOS
#define DIRECTOR_ESTADOS

class Director_estados
{
	public:
	enum class t_estados {REJILLA, PROPIEDADES_OBJETO_LOGICA, PROPIEDADES_META, AYUDA};

	private:
	t_estados actual;
	t_estados deseado;

	public:

	Director_estados()
		:actual(t_estados::REJILLA), deseado(t_estados::REJILLA)
	{}

	bool es_cambio_estado() const {return actual!=deseado;}
	void solicitar_cambio_estado(t_estados v) {deseado=v;}
	void confirmar_cambio_estado() {actual=deseado;}
	t_estados acc_estado_deseado() {return deseado;}
	t_estados acc_estado_actual() {return actual;}
};

#endif
