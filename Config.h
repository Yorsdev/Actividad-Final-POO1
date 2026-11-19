#ifndef CONFIG_H
#define CONFIG_H

#include <string>

using namespace std;

const int DIAS_SEMANA = 7;
const int HORAS_DIA = 24;
const int MAX_CARGAS = 5;

const string NOMBRES_DIAS[DIAS_SEMANA] = {
    "Lunes 21", "Martes 22", "Miercoles 23", "Jueves 24",
    "Viernes 25", "Sabado 26", "Domingo 27"};

// Parametros Panaderia (Digitos 0 - 1)
const float POTENCIA_REFRIGERADOR = 1.2f;
const float POTENCIA_HORNO = 7.0f;
const float POTENCIA_AMASADORA = 2.2f;
const float POTENCIA_CONTRATADA = 9.0f;

const float PRECIO_CARO = 800.0f;
const float PRECIO_ESCASEZ = 1500.0f;
const float EMBALSES_ALERTA = 40.0f;

const int PICO_INICIO = 18;
const int PICO_FIN = 20;

const int HORAS_MIN_HORNO = 6;
const int HORAS_MIN_AMASADORA = 4;

const int FALLA_CADA = 20;

#endif