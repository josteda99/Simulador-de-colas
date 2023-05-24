/* Definiciones externas para el sistema de colas simple */

#include "iostream"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "lcgrand.cpp" /* Encabezado para el generador de numeros aleatorios */
#include <fstream>
#include <string>
#include <thread>
#include <vector>
#define LIMITE_COLA 1000 /* Capacidad maxima de la cola */
#define OCUPADO 1        /* Indicador de Servidor Ocupado */
#define LIBRE 0          /* Indicador de Servidor Libre */

int sig_tipo_evento, num_clientes_espera, num_esperas_requerido, num_eventos,
    num_entra_cola, estado_servidor;
float area_num_entra_cola, area_estado_servidores, media_entre_llegadas, media_atencion,
    tiempo_simulacion, queue_arrivals[LIMITE_COLA + 1], tiempo_ultimo_evento, tiempo_sig_evento[3],
    total_de_esperas, tiempo_i0, tiempo_i1;

FILE *parametros, *resultados;

void inicializar(void);
void controltiempo(void);
void llegada(void);
void salida(void);
void reportes(void);
void actualizar_estad_prom_tiempo(void);
float expon(float mean);

using namespace std;
int main(void) /* Funcion Principal */
{
  /* Abre los archivos de entrada y salida */
  parametros = fopen("../param.txt", "r");
  resultados = fopen("result.txt", "w");

  if (parametros == NULL)
  {
    printf("No se pudo abrir el archivo param.txt\n");
    return 1;
  }

  /* Especifica el numero de eventos para la funcion controltiempo. */
  num_eventos = 2;

  /* Lee los parametros de enrtrada. */
  fscanf(parametros, "%f %f %d", &media_entre_llegadas, &media_atencion,
         &num_esperas_requerido);

  /* Escribe en el archivo de salida los encabezados del reporte y los parametros iniciales */
  fprintf(resultados, "Sistema de Colas Simple\n\n");
  fprintf(resultados, "Tiempo promedio de llegada%11.3f minutos\n\n",
          media_entre_llegadas);
  fprintf(resultados, "Tiempo promedio de atencion%16.3f minutos\n\n", media_atencion);
  fprintf(resultados, "Numero de clientes%14d\n\n", num_esperas_requerido);

  /* iInicializa la simulacion. */
  inicializar();

  /* Corre la simulacion mientras no se llegue al numero de clientes especificaco en el archivo de entrada*/
  while (num_clientes_espera < num_esperas_requerido)
  {
    /* Determina el siguiente evento */
    controltiempo();

    /* Actualiza los acumuladores estadisticos de tiempo promedio */
    actualizar_estad_prom_tiempo();

    /* Invoca la funcion del evento adecuado. */
    switch (sig_tipo_evento)
    {
    case 1:
      llegada();
      break;
    case 2:
      /* TODO: Revisar como hacer que sea por medio de hilos y tener control de estos */
      salida();
      break;
    }
  }

  /* Invoca el generador de reportes y termina la simulacion. */
  reportes();

  fclose(parametros);
  fclose(resultados);

  return 0;
}

void inicializar(void) /* Funcion de inicializacion. */
{
  /* Inicializa el reloj de la simulacion. */
  tiempo_simulacion = 0.0;

  /* Inicializa las variables de estado */
  estado_servidor = OCUPADO;
  num_entra_cola = 0;
  tiempo_ultimo_evento = 0.0;

  /* Inicializa los contadores estadisticos. */
  num_clientes_espera = 0;
  total_de_esperas = 0.0;
  area_num_entra_cola = 0.0;
  area_estado_servidores = 0.0;
  tiempo_i0 = 0.0;
  tiempo_i1 = 0.0;

  /* Inicializa la lista de eventos. Ya que no hay clientes, el evento salida (terminacion del servicio) no se tiene en cuenta */
  /* La posicion 1 son los eventos de llegadas de clientes */
  /* La posicion 2 es la terminacion de la simulacion */
  tiempo_sig_evento[1] = tiempo_simulacion + expon(media_entre_llegadas);
  tiempo_sig_evento[2] = 1.0e+30;
}

void controltiempo(void) /* Funcion controltiempo, para determinar que evento sigue */
{
  int i;
  float min_tiempo_sig_evento = 1.0e+29;
  sig_tipo_evento = 0;

  /*  Determina el tipo de evento del evento que debe ocurrir. */
  for (i = 1; i <= num_eventos; ++i)
  {
    if (tiempo_sig_evento[i] < min_tiempo_sig_evento)
    {
      min_tiempo_sig_evento = tiempo_sig_evento[i];
      sig_tipo_evento = i;
    }
  }
  /* Revisa si la lista de eventos esta vacia. */
  if (sig_tipo_evento == 0)
  {
    /* La lista de eventos esta vacia, se detiene la simulacion. */
    fprintf(resultados, "\nLa lista de eventos esta vacia %f", tiempo_simulacion);
    exit(1);
  }

  /* TLa lista de eventos no esta vacia, adelanta el reloj de la simulacion. */
  /* t0 -> t1 */
  tiempo_simulacion = min_tiempo_sig_evento;
}

void llegada(void) /* Funcion de llegada */
{
  float espera;

  /* Programa la siguiente llegada. */
  tiempo_sig_evento[1] = tiempo_simulacion + expon(media_entre_llegadas);

  /* TODO: Revisar si todos los hilos estan ocupados */
  /* Reisa si el servidor esta OCUPADO. */
  if (estado_servidor == OCUPADO)
  {
    /* Servidor OCUPADO, aumenta el numero de clientes en cola */
    ++num_entra_cola;

    /* Verifica si hay condici�n de desbordamiento */
    if (num_entra_cola > LIMITE_COLA)
    {
      /* Se ha desbordado la cola, detiene la simulacion */
      fprintf(resultados, "\nDesbordamiento del arreglo queue_arrivals a la hora");
      fprintf(resultados, "%f", tiempo_simulacion);
      exit(2);
    }

    /* Todavia hay espacio en la cola, se almacena el tiempo de llegada del cliente en el ( nuevo ) final de la cola */
    queue_arrivals[num_entra_cola] = tiempo_simulacion;
  }
  else
  {
    /*  El servidor esta IDLE, por lo tanto el cliente que llega tiene tiempo de espera=0
       (Las siguientes dos lineas del programa son para claridad, y no afectan
       el reultado de la simulacion ) */
    espera = 0.0;
    total_de_esperas += espera;

    /* Incrementa el numero de clientes en espera, y pasa el servidor a ocupado */
    /* Esta variables puede ser escrita por servidores, teber cuidado */
    ++num_clientes_espera;
    estado_servidor = OCUPADO;

    /* Programa una salida ( servicio terminado ). */
    tiempo_sig_evento[2] = tiempo_simulacion + expon(media_atencion);
  }
}

void salida(void) /* Funcion de Salida. */
{
  int i;
  float espera;

  /* Revisa si la cola esta vacia */
  if (num_entra_cola == 0)
  {
    /* La cola esta vacia, pasa el servidor a IDLE y
    no considera el evento de salida*/
    estado_servidor = LIBRE;
    tiempo_sig_evento[2] = 1.0e+30;
  }
  else
  {

    /* La cola no esta vacia, disminuye el numero de clientes en cola. */
    --num_entra_cola;

    /* Calcula la espera del cliente que esta siendo atendido y
    actualiza el acumulador de espera */
    espera = tiempo_simulacion - queue_arrivals[1];
    total_de_esperas += espera;

    /*Incrementa el numero de clientes en espera, y programa la salida. */
    ++num_clientes_espera;
    tiempo_sig_evento[2] = tiempo_simulacion + expon(media_atencion);
    // [1] 5 minutes
    // [2] 8 minutes, 1 minutes, 3 minutes, 4 minutes, 5 minutes
    //
    // a * b *
    /* Mueve cada cliente en la cola ( si los hay ) una posicion hacia adelante */
    for (i = 1; i <= num_entra_cola; ++i)
      queue_arrivals[i] = queue_arrivals[i + 1];
  }
}

void reportes(void) /* Funcion generadora de reportes. */
{
  /* Calcula y estima los estimados de las medidas deseadas de desempe�o */
  fprintf(resultados, "\n\nEspera promedio en la cola%11.3f minutos\n\n",
          total_de_esperas / num_clientes_espera);
  fprintf(resultados, "Numero promedio en cola%10.3f\n\n",
          area_num_entra_cola / tiempo_simulacion);
  fprintf(resultados, "Uso del servidor%15.3f\n\n",
          area_estado_servidores / tiempo_simulacion);
  fprintf(resultados, "Tiempo de terminacion de la simulacion%12.3f minutos", tiempo_simulacion);
}

void actualizar_estad_prom_tiempo(void) /* Actualiza los acumuladores de
                           area para las estadisticas de tiempo promedio. */
{
  float time_since_last_event;

  /* Calcula el tiempo desde el ultimo evento, y actualiza el marcador
    del ultimo evento */
  time_since_last_event = tiempo_simulacion - tiempo_ultimo_evento;
  tiempo_ultimo_evento = tiempo_simulacion;

  /* Actualiza el area bajo la funcion de numero_en_cola */
  area_num_entra_cola += num_entra_cola * time_since_last_event;

  /*Actualiza el area bajo la funcion indicadora de servidor ocupado*/
  area_estado_servidores += estado_servidor * time_since_last_event;
}

float expon(float media) /* Funcion generadora de la exponencias */
{
  /* Retorna una variable aleatoria exponencial con media "media"*/
  return -media * log(lcgrand(1));
}
