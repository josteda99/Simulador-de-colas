#include "iostream"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "lcgrand.cpp" /* Encabezado para el generador de numeros aleatorios */
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

int main(void)
{
  // decleration of int vector
  vector<int> vec = {1, 2, 3, 4, 5};
  int vecSize = vec.size(); // returns length of vector

  // decleration of vector iterator
  vector<int>::iterator iter = vec.begin();

  cout << "Vector: ";

  // run for loop from 0 to vecSize
  for (iter; iter < vec.end(); iter++)
  {
    // access value in the memory to which the pointer
    // is referencing
    cout << *iter << " ";
  }
  cout << endl;

  int test = vec.front();
  vec.erase(vec.begin());
  iter = vec.begin();
  // run for loop from 0 to vecSize
  for (iter; iter < vec.end(); iter++)
  {
    // access value in the memory to which the pointer
    // is referencing
    cout << *iter << " ";
  }
  cout << endl;

  // test[0] = 0.0f;
  // for (int i = 1; i < row + servers - 1; i++)
  // {
  //   test[i] = 3.0e+10;
  // }
  // for (int i = 0; i < row + servers - 1; i++)
  // {
  //   cout << test[i] << " ";
  // }

  // float min_tiempo_sig_evento = 1.0e+29;
  // for (int i = 0; i < row + servers - 1; ++i)
  // {
  //   if (tiempo_sig_evento[i] < min_tiempo_sig_evento)
  //   {
  //     min_tiempo_sig_evento = tiempo_sig_evento[i];
  //     sig_tipo_evento = (i > row) ? 1 : 0;
  //   }
  // }

  // int row = 2;

  // float **test;
  // test = new float *[row];
  // test[0] = new float;
  // test[1] = new float[3];

  // test[0][0] = 5.5f;
  // test[1][0] = 6.5f;
  // test[1][0] = 7.5f;
  // int length = sizeof(test[0]);
  // int prueba = sizeof(float);
  // cout << length << ' ' << prueba << endl;
  // // for (int i = 0; i < length; i++){

  // // }

  // for (int i = 0; i < row; i++)
  // {
  //   delete[] test[i];
  // }
  return 0;
}
