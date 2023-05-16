#include "iostream"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "lcgrand.cpp" /* Encabezado para el generador de numeros aleatorios */
#include <fstream>
#include <string>
using namespace std;
float expon(float media);
int main(void)
{
  float mean = 10;
  for (int i = 0; i < 1000; i++)
  {
    cout << expon(mean) << ",";
  }
  return 0;
}

float expon(float media) /* Funcion generadora de la exponencias */
{
  /* Retorna una variable aleatoria exponencial con media "media"*/
  return -media * log(lcgrand(1));
}
