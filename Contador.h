#pragma once

#include <utility>
using namespace std;

class Contador
{
public:
	typedef pair< int, int > T_posicion;

	// Construye un contador con velocidad 1.0, en posicion (0,0), en estado esperar.
	Contador();

	// Construye un contador con velocidad v, en posicion p, en estado p. 
	Contador(double v, T_posicion p);

	~Contador();

	// Obtenedores:
	double obtVelocidad();
	T_posicion obtPosicion();

	// Asignadores:
	void asgVelocidad(double nv);
	void asgPosicion(int x, int y);

	// EFE: avanza *this según su velocidad y evoluciona su estado en el tic que le toque.
	void avanzar();
	
	bool esta_en_espera(int tic);
	
	void asignar_reset_conteo(int x);

private:
	double velocidad;
	T_posicion posicion; // posicion.first == coordenada X, posicion.second = coordenada Y
	int reset_conteo;
};

Contador::Contador()
{
	velocidad = 6.0;
}

Contador::Contador(double v,  T_posicion p)
{
	velocidad = v;
	posicion = p;
}

Contador::~Contador()
{
}

double Contador::obtVelocidad()
{
	return velocidad;
}

Contador::T_posicion Contador::obtPosicion()
{
	return posicion; // agregue su propio codigo
}


void Contador::asgVelocidad(double nv)
{
	velocidad = velocidad;
}

void Contador::asgPosicion(int x, int y)
{
	posicion.first = x;
	posicion.second = y;
}


bool Contador::esta_en_espera(int tic)
{
	return tic % reset_conteo != 0;
}

void Contador::asignar_reset_conteo(int x)
{
	reset_conteo = x;
}

void Contador::avanzar()
{
	posicion.first += velocidad * (50/3);
}
