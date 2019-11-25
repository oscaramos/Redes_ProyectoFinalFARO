#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

/*Separa valores por una etiqueta*/
vector<string> separateValues(string input, char label);

/*Llena con 0s a la izquierda una cadena hasta alcanzar el numero ingresado*/
string zeroPadding(string input, int number);

/*Convierte un archivo a una cadena*/
string FileToString(string file_name);

/*Convierte una cadena a un archivo*/
void StringToFile(string file_name, string file_data);
