#pragma once

#include <fstream>
#include <math.h>
#include <assert.h>     /* assert */
#include <random>
#include <vector>
using namespace std;

#include "Tortuga.h"
#include "Contador.h"

class Simulador
{
public:
	Simulador();
	~Simulador();



	/* 
	* INICIALIZADORES DE SIMULACIÓN 
	*/

	// EFE:inicializa las secciones en que se divide la playa con los datos en el archivo.
	void inicializarPlaya(const vector<vector<int>>& matriz_terrenos);

	// EFE: inicializa los cuadrantes con los datos en el archivo.
	//		Crea e inicializa los contadores correspondientes.
	void inicializarCuadrantes(const vector<vector<int>>& matriz_cuadrantes);

	// EFE: inicializa los transectos verticales sobre la berma con los datos en el archivo.
	//		Crea e inicializa los contadores correspondientes.
	void inicializarTransectosVerticales(const vector<vector<int>> matriz_trans_verticales);

	// EFE: inicializa el transecto paralelo a la berma con el ancho y largo indicado.
	//		Crea e inicializa los contadores correspondientes.
	void inicializarTransectoBerma(const vector<vector<int>>& matriz_trans_paralelo);

	// EFE: crea la cantidad indicada de tortugas y las inicializa usando la distribución normal 
	// con el promedio y desviación dados para la velocidad.
	void inicializarTortugas(int cantidad,const vector<double>& vector_comportamiento_tortugas);

	// EFE: distribuye la cantidad total de tortugas que arriban, minuto a minuto, durante 360 
	// minutos o 6 horas, siguiendo la distribución logística con parámetros u y s.
	void inicializarArribada(int rango_inferior, int rango_superior, double marea_actual);

	// EFE: guarda los parámetros de la función sinusoidal que se usa para generar la altura de la 
	// marea minuto a minuto por 360 minutos o 6 horas.
	// NOTA: se debe usar modelo sinusoidal con periodo en minutos.
	void inicializarMarea(const vector<double>& marea);

	// REQ: total_tics <= 360.
	// EFE: simula el movimiento de las tortugas y el conteo de los contadores durante 6 horas.
	// NOTA: la tarea de simular representada por este método es la que debe paralelizarse usando hilos.
	void simular(int total_tics, int threads_count);
	
	vector<double> calcular_rangos_mareas();

    vector<int> calcular_rangos_tortugas();
	
	void control_arribada(int estado_marea, double altura_marea_actual, vector<int> rango_tortugas);

	
	void inicializarContadores();

	/*
	* OBTENEDORES DE RESULTADOS DE LA SIMULACIÓN
	*/

	// RET: total de tortugas que arribaron en la simulación.
	long obtTotalTortugasArribaron();

	// RET: total de tortugas que anidaron en la simulación.
	long obtTotalTortugasOvopositaron();

	// RET: estimación del total de tortugas que anidaron con base en el método de transectos sobre la berma.
	double obtEstimacionXtransectosSobreBerma();

	// RET: estimación del total de tortugas que arribaron con base en el método de transecto paralelo a la berma.
	double obtEstimacionXtransectoHorizontal();

	// RET: estimación del total de tortugas que anidaron con base en el método de cuadrantes.
	double obtEstimacionXcuadrantes();
	
	long obtTotalTortugasInactivas();
	
	int calcular_formula_TVB(int x);
	
	bool calcular_paso_de_estado(const vector<int>& rangos_de_estado, int tic_actual, int& id_estado);
	
	vector<int> rangos_de_estado();
	
	

private:

//parametros para otros usos
	int cantidad_tortugas_total;

//matrices con los datos de los archivos
	vector<vector<int>> matriz_terrenos;
	vector<vector<int>> matriz_cuadrantes;
	vector<vector<int>> matriz_transectos_verticales;
	vector<vector<int>> matriz_transecto_paralelo;
	vector<double> marea;
	vector <double> vector_compor_tortugas;
	vector<Tortuga> tortugas;
	Contador contador_paralelo;
	vector<Contador> contadores_verticales;
	vector<Contador> contadores_cuadrante; 
};

Simulador::Simulador()
{
}


Simulador::~Simulador()
{
}

void Simulador::inicializarPlaya(const vector<vector<int>>& matriz_terrenos)
{
	this->matriz_terrenos = matriz_terrenos;
}

void Simulador::inicializarCuadrantes(const vector<vector<int>>& matriz_cuadrantes)
{
	this->matriz_cuadrantes = matriz_cuadrantes;
}

	
void Simulador::inicializarTransectosVerticales(const vector<vector<int>> matriz_transectos_verticales)
{
	this->matriz_transectos_verticales = matriz_transectos_verticales;
}

	
void Simulador::inicializarTransectoBerma(const vector<vector<int>>& matriz_transecto_paralelo)
{
	this->matriz_transecto_paralelo = matriz_transecto_paralelo;
}

void Simulador::inicializarTortugas(int cantidad,const vector<double>& vector_comportamiento_tortugas)
{
	this->vector_compor_tortugas = vector_comportamiento_tortugas;
	vector<Tortuga> tortugas_n(cantidad);
	this->tortugas = tortugas_n;
	cantidad_tortugas_total = cantidad;
	std::default_random_engine generator;
	std::normal_distribution<double> velocidad(vector_comportamiento_tortugas[6],vector_comportamiento_tortugas[7]);
	for(int i = 0; i < cantidad; ++i)
	{
		tortugas[i].asgVelocidad(velocidad(generator));	
	}
}

void Simulador::inicializarContadores()
{
	int x;
	int y;
	vector<Contador> contadores_vertical(matriz_transectos_verticales[0][0]);
	for(int i = 1; i <= matriz_transectos_verticales[0][0]; ++i)
	{
		x = matriz_transectos_verticales[i][0] + 1; //porque seria la media del eje x
		y = (matriz_transectos_verticales[i][2] + matriz_transectos_verticales[i][1]) / 2; //la media del eje y 
		contadores_vertical[i - 1].asgPosicion(x,y);
		contadores_vertical[i - 1].asignar_reset_conteo(matriz_transectos_verticales[0][1]);
	}
	this->contadores_verticales = contadores_vertical;
	
	contador_paralelo.asgPosicion(0,7);
	
	contador_paralelo.asignar_reset_conteo(matriz_transecto_paralelo[0][1]);
	
	
	
	
	
	/*vector<Contador> contadores_cuadrante(matriz_cuadrantes[0][0]);
	for(int i = 0; i < matriz_cuadrantes[0][0]; ++i)
	{
		x = matriz_cuadrantes[i][0] + (matriz_cuadrantes[i][2] - matriz_cuadrantes[i][0]); // para colocarlos en el centro
		y = matriz_cuadrantes[i][1] + (matriz_cuadrantes[i][3] - matriz_cuadrantes[i][1]);
		contadores_cuadrante[i].asgPosicion(x,y);
	}
	this->contadores_cuadrante = contadores_cuadrante;
	vector<Contador> contadores_paralelo(matriz_transecto_paralelo[0][0]);
	for(int i = 0; i < matriz_transecto_paralelo[0][0]; ++i)
	{
		x = 
		y = 
	}
	contador_paralelo.asgPosicion()
	*/ 
}

void Simulador::inicializarArribada(int rango_inferior, int rango_superior, double marea_actual)
{
	std::default_random_engine generator;
    std::uniform_int_distribution<int> x(0,1500);
    std::uniform_int_distribution<int> y(marea[0],marea_actual);
	for(int i = rango_inferior; i < rango_superior; ++i)
	{
		if(!this->tortugas[i].logro_arribar())
		{
			this->tortugas[i].asgPosicion(x(generator),y(generator));
			this->tortugas[i].arribo();
		}
	}
}

void Simulador::inicializarMarea(const vector<double>& marea)
{
	this->marea = marea;
}

void Simulador::simular(int total_tics, int threads_count)
{	
	std::default_random_engine generator;
	//para asignar el cambio de estado a inactivo para las tortugas
    std::uniform_real_distribution<double> distribution(0.0,1.0);
	
	double p = (marea[1] - marea[0]) / total_tics;
	
	double altura_marea_actual;
	
	int estado_marea = 0; // 0 = baja  1 = media  2 = alta
	
	vector<double> rangos_mareas = calcular_rangos_mareas();
	
	vector<int> rangos_tortugas = calcular_rangos_tortugas();
	
	vector<int> rangos_estados = rangos_de_estado();
	
	int total_tortugas_contadas_V = 0;
	
	int total_tortugas_contadas_P = 0;
	
	int total_tortugas_contadas_C = 0;
	
	inicializarContadores();
	
	int id_estado = 0;
	
	for(int i = 0; i < total_tics; ++i)
	{
		altura_marea_actual = p * i + marea[0];
		//lo siguientes if son para ver si estamos en marea baja, media o alta
		if(altura_marea_actual >= rangos_mareas[1] && estado_marea == 0)
		{
			++estado_marea;  //pasaria a marea media 
		}
		if(altura_marea_actual > rangos_mareas[2] && estado_marea == 1)
		{
			++estado_marea; //pasaria a marea alta
		}
		
		control_arribada(estado_marea,altura_marea_actual,rangos_tortugas);
		
		contador_paralelo.avanzar();
		
		for(int j = 0; j < cantidad_tortugas_total; ++j)
		{	
			if(this->tortugas[j].logro_arribar())
			{
				this->tortugas[j].avanzar(calcular_paso_de_estado(rangos_estados, i, id_estado),vector_compor_tortugas,distribution(generator)); 
				if(!tortugas[j].fue_contada_V())
				{
					for(int k = 0; k < matriz_transectos_verticales[0][0]; ++k)
					{
						if(!contadores_verticales[k].esta_en_espera(i))
						{
							if(matriz_transectos_verticales[k+1][0] <= tortugas[j].obtPosicion().first && tortugas[j].obtPosicion().first <= matriz_transectos_verticales[k+1][0] + 2 && matriz_transectos_verticales[k+1][1] <= tortugas[j].obtPosicion().second && tortugas[j].obtPosicion().second <= matriz_transectos_verticales[k+1][2])
							{
								tortugas[j].contada_V();
								if(tortugas[j].fue_contada_V())
									++total_tortugas_contadas_V;
							}
						}		
					}
				}
				if(!tortugas[j].fue_contada_P())
				{ 
					if(!contador_paralelo.esta_en_espera(i))
					{
						// a este if no esta entrando 
						if(contador_paralelo.obtPosicion().first <= tortugas[j].obtPosicion().first && tortugas[j].obtPosicion().first <= contador_paralelo.obtPosicion().first + matriz_transecto_paralelo[0][2] && contador_paralelo.obtPosicion().second <= tortugas[j].obtPosicion().second && tortugas[j].obtPosicion().second <= contador_paralelo.obtPosicion().second + matriz_transecto_paralelo[0][2])
						{ 
							++total_tortugas_contadas_P;
							tortugas[j].contada_P();
						}
					}
				}	
			}	
		}
		
		
	}
	
	int formula_TVB = calcular_formula_TVB(total_tortugas_contadas_V);
	
	cout << "cantidad total de tortugas: " << cantidad_tortugas_total << "\n";
	cout << "cantidad total de tortugas inactivas: " << obtTotalTortugasInactivas() << "\n";
	cout << "cantidad total de tortugas contadas por TVB: " << formula_TVB << "\n";
	cout << "cantidad total de tortugas contadas por TPB: " << total_tortugas_contadas_P << "\n";
	//cout << "total de tortugas que anidaron: " << obtTotalTortugasOvopositaron() << "\n";
}

vector<double> Simulador::calcular_rangos_mareas()
{
	vector<double> rangos(3);
	rangos[0] = marea[0];
	rangos[1] = marea[1] * 0.33 + marea[0] * 0.33 + 0.150;
	rangos[2] = rangos[1] +  rangos[1] - 0.150;
	return rangos;
}

vector<int> Simulador::calcular_rangos_tortugas()
{
	vector<int> rangos(4);
	rangos[0] = 0;
	rangos[1] = cantidad_tortugas_total * 0.30;
	rangos[2] = rangos[1] + cantidad_tortugas_total * 0.45;
	rangos[3] = rangos[2] + cantidad_tortugas_total * 0.25;
	return rangos;
}

vector<int> Simulador::rangos_de_estado()
{
	vector<int> rangos(6);
	rangos[0] = marea[2] * 0.20;
	rangos[1] = rangos[0] + (int)marea[2] * 0.05;
	rangos[2] = rangos[1] + (int)marea[2] * 0.07;
	rangos[3] = rangos[2] + (int)marea[2] * 0.55;
	rangos[4] = rangos[3] + (int)marea[2] * 0.06;
	rangos[5] = rangos[4] + (int)marea[2] * 0.07;
	return rangos;
}

void Simulador::control_arribada(int estado_marea, double altura_marea_actual, vector<int> rango_tortugas)
{	
    switch(estado_marea)
    {
		case 0:
			inicializarArribada(rango_tortugas[estado_marea],rango_tortugas[estado_marea + 1], altura_marea_actual);
			break;
		case 1:
			inicializarArribada(rango_tortugas[estado_marea],rango_tortugas[estado_marea + 1], altura_marea_actual);
			break;
		case 2:	
			inicializarArribada(rango_tortugas[estado_marea],rango_tortugas[estado_marea + 1], altura_marea_actual);
			break;	
		default: 
			cerr << "error\n";
	}  
}


long Simulador::obtTotalTortugasArribaron()
{
	int total = 0;
	for(int i = 0; i < cantidad_tortugas_total; ++i)
	{
		if(this->tortugas[i].logro_arribar())
		{
			++total;
		}	
	}
	return total;		
}

long Simulador::obtTotalTortugasInactivas()
{
	int total = 0;
	for(int i = 0; i < cantidad_tortugas_total; ++i)
	{
		if(this->tortugas[i].esta_inactiva())
		{
			++total;
		}	
	}
	return total;	
}

long Simulador::obtTotalTortugasOvopositaron()
{
	int total = 0;
	for(int i = 0; i < cantidad_tortugas_total; ++i)
		if(tortugas[i].logro_ovopositar())
			++total;
	return total;		
}

double Simulador::obtEstimacionXtransectosSobreBerma()
{
	return 0.0; // agregue su propio codigo
}

double Simulador::obtEstimacionXtransectoHorizontal()
{
	return 0.0; // agregue su propio codigo
}

double Simulador::obtEstimacionXcuadrantes()
{
	return 0.0; // agregue su propio codigo
}

int Simulador::calcular_formula_TVB(int x)
{
	int A = 1500;
	int d = marea[2];
	int w = 2;
	int m = 1;
	int j = 0;
	for(int i = 1; i < matriz_transectos_verticales[0][0]; ++i)
		j += matriz_transectos_verticales[i][2] - matriz_transectos_verticales[i][1];
	int N = x;
	int pr = marea[2] / 6;	
	return (A * d / ( 2 * w * m * j) )*(N / pr);
}

bool Simulador::calcular_paso_de_estado(const vector<int>& rangos_de_estado, int tic_actual, int& id_estado)
{
	bool result = tic_actual == (int)rangos_de_estado[id_estado];
	if(result)
	{
		++id_estado;
	}	
	return result;
}

	
	


