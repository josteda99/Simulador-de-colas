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
  for (int i = 0; i < 5; i++)
  {
    cout << i << endl;
  }
  // decleration of int vector
  // vector<int> vec = {1, 2, 3, 4, 5};
  // vector<float> test;
  // test.push_back(3.0);
  // test.push_back(221.0);
  // cout << test[0] << " " << test[1] << " " << test[2] << endl;
  // int vecSize = vec.size(); // returns length of vector

  // // decleration of vector iterator
  // vector<int>::iterator iter = vec.begin();

  // cout << "Vector: ";

  // // run for loop from 0 to vecSize
  // for (iter; iter < vec.end(); iter++)
  // {
  //   // access value in the memory to which the pointer
  //   // is referencing
  //   cout << *iter << " ";
  // }
  // cout << endl;

  // int test = vec.front();
  // vec.erase(vec.begin());
  // iter = vec.begin();
  // // run for loop from 0 to vecSize
  // for (iter; iter < vec.end(); iter++)
  // {
  //   // access value in the memory to which the pointer
  //   // is referencing
  //   cout << *iter << " ";
  // }
  // cout << endl;

  return 0;
}
