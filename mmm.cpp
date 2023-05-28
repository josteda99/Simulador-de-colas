/* Definiciones externas para el sistema de colas simple sistema (M/M/m) */
#include "lcgrand.cpp" /* Encabezado para el generador de numeros aleatorios */
#include <algorithm>
#include "iostream"
#include <stdlib.h>
#include <fstream>
#include <stdio.h>
#include <numeric>
#include <vector>
#include <string>
#include <math.h>

#define QUEUE_LIMIT 1000 /* Capacidad maxima de la cola */
#define NULL_INDEX -1    /* Indice Nulo, no hay coincidencias */
#define BUSSY 1          /* Indicador de Servidor Ocupado */
#define IDLE 0           /* Indicador de Servidor Libre */

using namespace std;

int sig_tipo_evento, num_clientes_espera, num_esperas_requerido, num_eventos,
    num_entra_cola, num_servers;

float area_num_entra_cola, media_entre_llegadas, media_atencion,
    tiempo_simulacion, tiempo_ultimo_evento, total_de_esperas, erlang;
/* Estados de los servidores */
vector<int> servers_status;

vector<double> tiempo_sig_evento, area_estado_servidores;

vector<float> queue_arrivals;
FILE *parametros, *resultados;

void actualizar_estad_prom_tiempo(void);
int get_idle_server(void);
void controltiempo(void);
float expon(float mean);
void inicializar(void);
void reportes(void);
void llegada(void);
void salida(void);
template <typename T>
void print_vector(vector<T>);

int main(void) /* Funcion Principal */
{
  /* Abre los archivos de entrada y salida */
  parametros = fopen("../param2.txt", "r");
  resultados = fopen("result.txt", "w");

  if (parametros == NULL)
  {
    printf("No se pudo abrir el archivo param.txt\n");
    return 1;
  }

  /* Lee los parametros de enrtrada. */
  fscanf(parametros, "%f %f %d %d", &media_entre_llegadas, &media_atencion,
         &num_esperas_requerido, &num_servers);

  /* Escribe en el archivo de salida los encabezados del reporte y los parametros iniciales */
  fprintf(resultados, "Sistema de Colas Simple Modelo (M/M/c)\n\n");
  fprintf(resultados, "Tiempo promedio de llegada%11.3f segundos\n\n",
          media_entre_llegadas);
  fprintf(resultados, "Tiempo promedio de atencion%16.3f segundos\n\n", media_atencion);
  fprintf(resultados, "Numero de clientes%14d\n\n", num_esperas_requerido);
  fprintf(resultados, "Numero de servidores%12d\n\n", num_servers);

  /* iInicializa la simulacion. */
  inicializar();

  /* Corre la simulacion mientras no se llegue al numero de clientes especificaco en el archivo de entrada*/
  while (num_clientes_espera < num_esperas_requerido)
  {
    // cout << "Servers: ";
    // print_vector(servers_status);

    // cout << endl
    //      << "Tiempo eventos: ";
    // print_vector(tiempo_sig_evento);
    // cout << endl
    //      << "Total espera: " << total_de_esperas << endl;
    /* Determina el siguiente evento */
    controltiempo();

    /* Actualiza los acumuladores estadisticos de tiempo promedio */
    actualizar_estad_prom_tiempo();

    /* Invoca la funcion del evento adecuado. */
    switch (sig_tipo_evento)
    {
    case -1:
      // Si entra aca algo fallo... ya que no hay un evento programado para eso
      return 1;
    case 0:
      llegada();
      break;
    default:
      salida();
      break;
    }
  }

  /* Invoca el generador de reportes y termina la simulacion. */
  reportes();

  fclose(parametros);
  fclose(resultados);

  tiempo_sig_evento.clear();
  servers_status.clear();
  queue_arrivals.clear();
  area_estado_servidores.clear();
  return 0;
}

void inicializar(void) /* Funcion de inicializacion. */
{
  /* Especifica el numero de eventos para la funcion controltiempo. */
  num_eventos = num_servers + 1;

  /* Inicializa el reloj de la simulacion. */
  tiempo_simulacion = 0.0;

  /* Inicializa las variables de estado */
  servers_status.insert(servers_status.begin(), num_servers, IDLE);
  num_entra_cola = 0;
  tiempo_ultimo_evento = 0.0;

  /* Inicializa los contadores estadisticos. */
  erlang = 0.0;
  num_clientes_espera = 0;
  total_de_esperas = 0.0;
  area_num_entra_cola = 0.0;
  area_estado_servidores.insert(area_estado_servidores.begin(), num_servers, 0.0);

  /* Inicializa la lista de eventos. Ya que no hay clientes, el evento salida (terminacion del servicio) no se tiene en cuenta */
  /* La posicion 1 son los eventos de llegadas de clientes */
  /* La posicion 2 es la terminacion de la simulacion */
  tiempo_sig_evento.insert(tiempo_sig_evento.begin(), num_eventos, 1.0e+30);
  tiempo_sig_evento[0] = tiempo_simulacion + expon(media_entre_llegadas);
}

void controltiempo(void) /* Funcion controltiempo, para determinar que evento sigue */
{

  int i;
  float min_tiempo_sig_evento = 1.0e+29;
  sig_tipo_evento = NULL_INDEX;

  /*  Determina el tipo de evento del evento que debe ocurrir. */
  for (i = 0; i < num_eventos; i++)
  {
    if (tiempo_sig_evento[i] < min_tiempo_sig_evento)
    {
      min_tiempo_sig_evento = tiempo_sig_evento[i];
      sig_tipo_evento = i;
    }
  }
  /* Revisa si la lista de eventos esta vacia. */
  if (sig_tipo_evento == NULL_INDEX)
  {
    /* La lista de eventos esta vacia, se detiene la simulacion. */
    fprintf(resultados, "\nLa lista de eventos esta vacia %f", tiempo_simulacion);
    exit(1);
  }

  /* TLa lista de eventos no esta vacia, adelanta el reloj de la simulacion. */
  tiempo_simulacion = min_tiempo_sig_evento;
}

void llegada(void) /* Funcion de llegada */
{
  float espera;

  /* Programa la siguiente llegada. */
  tiempo_sig_evento[0] = tiempo_simulacion + expon(media_entre_llegadas);

  /* Reisa si hay un servidor disponible. */
  int idle_server = get_idle_server();
  if (idle_server == NULL_INDEX)
  {

    /* Verifica si hay condici�n de desbordamiento */
    if (num_entra_cola > QUEUE_LIMIT)
    {
      /* Se ha desbordado la cola, detiene la simulacion */
      fprintf(resultados, "\nDesbordamiento del arreglo queue_arrivals a la hora");
      fprintf(resultados, "%f", tiempo_simulacion);
      exit(2);
    }
    /* Todavia hay espacio en la cola, se almacena el tiempo de llegada del cliente en el ( nuevo ) final de la cola */
    queue_arrivals.push_back(tiempo_simulacion);

    /* Servidor OCUPADO, aumenta el numero de clientes en cola */
    num_entra_cola++;
  }
  else
  {
    /*  El servidor esta IDLE, por lo tanto el cliente que llega tiene tiempo de espera=0
       (Las siguientes dos lineas del programa son para claridad, y no afectan
       el reultado de la simulacion ) */
    espera = 0.0;
    total_de_esperas += espera;

    /* Incrementa el numero de clientes en espera, y pasa el servidor a ocupado */
    /* Llego y estoy siendo atendido luego salgo */
    ++num_clientes_espera; // Incrementar la cantidad de clientes que han sido atendidos
    servers_status[idle_server] = BUSSY;

    /* Programa una salida para el servidor que se acaba de ocupar( servicio terminado ). */
    tiempo_sig_evento[idle_server + 1] = tiempo_simulacion + expon(media_atencion);
  }
}

void salida(void) /* Funcion de Salida. */
{
  int server = sig_tipo_evento - 1;
  float espera;

  /* Revisa si la cola esta vacia */
  if (num_entra_cola == 0)
  {
    /* La cola esta vacia, pasa el servidor a IDLE y
    no considera el evento de salida*/
    servers_status[server] = IDLE;
    tiempo_sig_evento[sig_tipo_evento] = 1.0e+30;
  }
  else
  {
    /* La cola no esta vacia, disminuye el numero de clientes en cola. */
    num_entra_cola--;

    /* Calcula la espera del cliente que esta siendo atendido y
        actualiza el acumulador de espera */
    espera = tiempo_simulacion - queue_arrivals.front();
    queue_arrivals.erase(queue_arrivals.begin());
    total_de_esperas += espera;

    /*Incrementa el numero de clientes en espera, y programa la salida. */
    num_clientes_espera++; // Aumenta el numero de clientes atendidos
    tiempo_sig_evento[sig_tipo_evento] = tiempo_simulacion + expon(media_atencion);
  }
}

void reportes(void) /* Funcion generadora de reportes. */
{
  /* Calcula y estima los estimados de las medidas deseadas de desempe�o */
  fprintf(resultados, "\n\nEspera promedio en la cola%11.4f segundos\n\n",
          total_de_esperas / num_clientes_espera);
  fprintf(resultados, "Numero promedio en cola%12.4f\n\n",
          area_num_entra_cola / tiempo_simulacion);
  // Ahora este debe ser el promedio
  for (unsigned int i = 0; i < area_estado_servidores.size(); i++)
  {
    fprintf(resultados, "Uso del servidor %d: %15.4f\n\n", i, area_estado_servidores[i] / tiempo_simulacion);
  }
  double avarage_servers = accumulate(area_estado_servidores.begin(), area_estado_servidores.end(), 0.0) / area_estado_servidores.size();
  fprintf(resultados, "Uso de los servidores: %12.4f\n\n", avarage_servers / tiempo_simulacion);
  fprintf(resultados, "Erlang-C: %25.6f\n\n", erlang / tiempo_simulacion);
  fprintf(resultados, "Tiempo de terminacion de la simulacion%12.4f segundos", tiempo_simulacion);
}

void actualizar_estad_prom_tiempo(void) /* Actualiza los acumuladores de area para las estadisticas de tiempo promedio. */
{
  float time_since_last_event;

  /* Calcula el tiempo desde el ultimo evento, y actualiza el marcador
    del ultimo evento */
  time_since_last_event = tiempo_simulacion - tiempo_ultimo_evento;
  tiempo_ultimo_evento = tiempo_simulacion;

  /* Actualiza el area bajo la funcion de numero_en_cola */
  area_num_entra_cola += num_entra_cola * time_since_last_event;

  /* Actualiza el area bajo la funcion indicadora de servidores ocupados */
  /* Hacer esto para cada servidor */
  int is_busy = 1;
  for (unsigned int index = 0; index < servers_status.size(); index++)
  {
    is_busy *= servers_status[index];
    area_estado_servidores[index] += servers_status[index] * time_since_last_event;
  }

  /* Actualizar el area bajo el timepo que todos los servidores estan ocupados */
  erlang += is_busy * time_since_last_event;
}

float expon(float media) /* Funcion generadora de la exponencias */
{
  /* Retorna una variable aleatoria exponencial con media "media"*/
  return -media * log(lcgrand(1));
}

/* funcion que busca el primer servidor que este desocupado*/
int get_idle_server()
{
  int index;
  int state = IDLE;
  auto result = find(servers_status.begin(), servers_status.end(), state);
  if (result != servers_status.end())
  {
    index = result - servers_status.begin();
    return index;
  }
  return -1;
}

template <typename T>
void print_vector(vector<T> test)
{
  for (auto it = test.begin(); it != test.end(); it++)
  {
    cout << "Status: " << *it << ", ";
  }
  cout << endl;
}
