#pragma once

#include <utility>
#include <random>
#include <vector>
using namespace std;

class Tortuga
{
public:
	typedef pair< int, int > T_posicion;

	// Construye una tortuga con velocidad 1, en estado "inactiva", en posicion (0,0).
	Tortuga();

	// Construye una tortuga con velocidad v, en estado e, en posicion p.
	Tortuga(double v, T_posicion p);

	~Tortuga();

	// Obtenedores:
	double obtVelocidad();
	T_posicion obtPosicion();

	// Asignadores:
	void asgVelocidad(double nv);
	void asgPosicion(int x, int y);;

	// EFE: avanza *this según su velocidad y evoluciona su estado en el tic que le toque.
	void avanzar( bool pasar_estado,const vector<double>& proba_inactiva, double random);
	
	
	bool logro_ovopositar();
	bool logro_arribar();
	bool fue_contada_V();
	bool fue_contada_P();
	bool fue_contada_C();
	bool esta_inactiva();
	void arribo();
	void ovoposito();
	void contada_V();
	void contada_P();
	void contada_C();
	void inactivoS();
	

private:
	double velocidad;
	T_posicion posicion; // posicion.first == coordenada X, posicion.second = coordenada Y
	int id_de_estado; //para poder sacar cada vez la probabilidad de que la tortuga quede inactiva 
	
	bool si_ovoposito;
	bool si_arribo;
	bool ha_sido_contada_V;
	bool ha_sido_contada_P;
	bool ha_sido_contada_C;
	bool inactivo;
};

Tortuga::Tortuga()
{
	velocidad = 1;
	posicion.first = 0;
	posicion.second = 0;
	si_ovoposito = false;
	si_arribo = false;
	id_de_estado = 0; //0 = vagar 1 = camar 2 = excavar 3 = poner 4 = tapar 5 = camuflar 6 = inactiva
	ha_sido_contada_V = false;
	ha_sido_contada_P = false;
	ha_sido_contada_C = false;
	inactivo = false;
}

Tortuga::Tortuga(double v, T_posicion p)
{
	velocidad = v;
	posicion = p;
}

Tortuga::~Tortuga()
{
}

double Tortuga::obtVelocidad()
{
	return velocidad; 
}

Tortuga::T_posicion Tortuga::obtPosicion()
{
	return posicion;
}

void Tortuga::asgVelocidad(double nv)
{
	velocidad = nv;
}

void Tortuga::asgPosicion(int x, int y )
{
	posicion.first = x;
	posicion.second = y;
}

bool Tortuga::logro_ovopositar()
{
	return si_ovoposito;
}

void Tortuga::ovoposito()
{
	si_ovoposito = true;
}

bool Tortuga::logro_arribar()
{
	return si_arribo;
}

void Tortuga::arribo()
{
	si_arribo = true;
}

bool Tortuga::fue_contada_V()
{
	return ha_sido_contada_V;
}

void Tortuga::contada_V()
{
	if(si_ovoposito)
		ha_sido_contada_V = true;
}

bool Tortuga::fue_contada_P()
{
	return ha_sido_contada_P;
}

void Tortuga::contada_P()
{
	ha_sido_contada_P = true;
}

bool Tortuga::fue_contada_C()
{
	return ha_sido_contada_C;
}

void Tortuga::contada_C()
{
	ha_sido_contada_C = true;
}

void Tortuga::inactivoS()
{
	inactivo = true;
}

bool Tortuga::esta_inactiva()
{
	return inactivo;
}

void Tortuga::avanzar(bool pasar_estado,const vector<double>& proba_inactiva, double random)
{
	if(id_de_estado == 0)
	{
		posicion.second += velocidad * (50/3); // ese 5/3 pasa de km/h a m/min	
	}
    //aqui calculo mediante probabilidad si en ese estado epecifico va a pasar a ser inactiva
	if(1 - proba_inactiva[id_de_estado] < random && id_de_estado < 6 && pasar_estado) 
	{
		inactivoS();
		id_de_estado = 6; //representa inactiva en numeros 
	}
	if(id_de_estado < 6 && pasar_estado)	
		switch(id_de_estado)
		{
			case 0:
				++id_de_estado;
				break;
			case 1:
				++id_de_estado;
				break;
			case 2:
				++id_de_estado;
				break;
			case 3:
				ovoposito(); //le pone al bool si_anido true para indicar que finalizo su labor
				++id_de_estado;
				break;
			case 4:
				++id_de_estado;
				break;
		}
}
