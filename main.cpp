//
//  main.cpp
//  Lee_csv
//
//  Created by administrador on 29/4/19.
//  Copyright © 2019 administrador. All rights reserved.
//
#pragma GCC diagnostic ignored "-Wdeprecated"    //sirve para quitar el warning del throw
#include <vector>
#include <sstream>
#include <fstream>
#include <string>
#include <exception>
#include <iostream>
#include <omp.h>
#include "Simulador.h"
using namespace std;

double stod_wrapper(string v) throw (invalid_argument,out_of_range) {return std::stod(v);}
int stoi_wrapper(string v)throw (invalid_argument,out_of_range) {return std::stoi(v);}

vector<vector<int>> leer_archivos_enteros(vector<string> archivo, int id);

vector<vector<double>> leer_archivos_dobles(vector<string> vector_archivos, int id);

void rellenar_vector_archivos(vector<string>& vector_archivos);

template < typename T, class F >
vector< vector< T > > carga_valida_datos(ifstream& archivo, F t) throw (invalid_argument,out_of_range);

void inicializar_simulador(vector<vector<vector<int>>>& master_matriz, vector<vector<double>>& matriz_mareas, vector<double>& vector_compor_tortugas);

int main(int argc, const char * argv[]) {
	double begin = omp_get_wtime();
	Simulador simulador;
	vector<string> vector_archivos(7);
	rellenar_vector_archivos(vector_archivos);
	vector<int> tamanyo_F(11);
	vector<double> tamanyo_F2(11);
	vector<vector<int>> tamanyo_C(31,tamanyo_F);
	vector<vector<vector<int>>> master_matriz(5,tamanyo_C);
	vector<vector<double>> matriz_mareas(3,tamanyo_F2);
	vector<vector<double>> vector_compor_tortugas(1,tamanyo_F2);
	matriz_mareas = leer_archivos_dobles(vector_archivos,5);
	vector_compor_tortugas = leer_archivos_dobles(vector_archivos,6);
	#pragma omp parallel num_threads(5)
	{
		int id = omp_get_thread_num();
		master_matriz[id] = leer_archivos_enteros(vector_archivos, id);	
	}
	inicializar_simulador(master_matriz,matriz_mareas, vector_compor_tortugas[0]);
	double end = omp_get_wtime();
	printf("\nEl tiempo total fue de: %fs\n", end - begin);
    return 0;
}

void rellenar_vector_archivos(vector<string>& vector_archivos)
{		
	vector_archivos[0] = "/home/thecromer/Repositorios Git/Tortugas Proyect/archivos/experimentos.csv";
	vector_archivos[1] = "/home/thecromer/Repositorios Git/Tortugas Proyect/archivos/terreno.csv";
	vector_archivos[2] = "/home/thecromer/Repositorios Git/Tortugas Proyect/archivos/cuadrantes.csv";
	vector_archivos[3] = "/home/thecromer/Repositorios Git/Tortugas Proyect/archivos/transectos_verticales.csv";
	vector_archivos[4] = "/home/thecromer/Repositorios Git/Tortugas Proyect/archivos/transecto_paralelo_berma.csv";
	vector_archivos[5] = "/home/thecromer/Repositorios Git/Tortugas Proyect/archivos/marea.csv";
	vector_archivos[6] = "/home/thecromer/Repositorios Git/Tortugas Proyect/archivos/comportamiento_tortugas.csv"; 
}

vector<vector<int>> leer_archivos_enteros(vector<string> vector_archivos, int id)
{
	ifstream archivo (vector_archivos[id], ios::in);
    if (!archivo)
        cout << "no encuentra el archivo de datos" << endl;
    vector< vector< int > > ve;
    try {
         ve = carga_valida_datos< int >(archivo,stoi_wrapper); // usa wrapper de stoi
    } catch (exception e){
        cout << "valor invalido o fuera de limite" << endl;
    }
    /*for(auto f : ve)
        for(auto x : f)
            cout << x << ',' <<endl;*/      
	return ve;
}

vector<vector<double>> leer_archivos_dobles(vector<string> vector_archivos, int id)
{
	ifstream archivo(vector_archivos[id], ios::in);
    if (!archivo)
        cout << "no encuentra el archivo de datos" << endl;
    vector< vector< double > > ve;
    try {
         ve = carga_valida_datos< double >(archivo,stod_wrapper); // usa wrapper de stod porque uso doubles
    } catch (exception e){
        cout << "valor invalido o fuera de limite" << endl;
    }
    /*for(auto f : ve)
        for(auto x : f)
            cout << x << ',' <<endl;*/    
	return ve;
}

template < typename T, class F >
vector< vector< T > > carga_valida_datos(ifstream& archivo, F t) throw (invalid_argument,out_of_range)
{
    vector< vector< T > > valores;
    vector< T > linea_valores;
    string linea;
    while (getline(archivo,linea)) {
        linea_valores.clear();
        stringstream ss(linea);
        string numero_S;
        T numero_T;
        while (getline(ss, numero_S, ',')) {
            try {
            numero_T = t(numero_S);
            } catch (exception e) {
                throw e;
            }
            linea_valores.push_back(numero_T);
        }
        valores.push_back(linea_valores);
    }
    return valores;
}

void inicializar_simulador(vector<vector<vector<int>>>& master_matriz, vector<vector<double>>& matriz_mareas, vector<double>& vector_compor_tortugas)
{
	vector<Simulador> simuladores(3);
	#pragma omp parallel for num_threads(3)
		for(int i = 0; i < 3; ++i)
		{
			simuladores[i].inicializarPlaya(master_matriz[1]); //inicializa los sectores
			simuladores[i].inicializarTransectosVerticales(master_matriz[3]); //inicializa los transectos verticales 
			simuladores[i].inicializarTransectoBerma(master_matriz[4]);
			simuladores[i].inicializarCuadrantes(master_matriz[2]);
			simuladores[i].inicializarMarea(matriz_mareas[0]);
			simuladores[i].inicializarTortugas(master_matriz[0][i][2], vector_compor_tortugas);
			// poner un futuro for aqui que e haga el numero respectivo de veces que diga 
			// experimentos un total de 
			simuladores[i].simular(matriz_mareas[0][2],master_matriz[0][i][3 + i]);
		}
}

