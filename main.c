#include "tdas/extra.h"
#include "tdas/list.h"
#include "tdas/map.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
  char id[100];
  char title[100];
  List *genres;
  char director[300];
  float rating;
  int year;
} Film;

char *espacioInicial(char *str) {

    while (isspace((unsigned char)*str)) str++; //Mientras el primer carácter es un espacio, avanzamos el puntero del inicio de la cadena

    return str; //Retornamos la cadena cambiada

}

void borrarComillas(char *str) { 

    int len = strlen(str); //Obtenemos la longitud de la cadena

    if (str[0] == '"') { //SI el primer carácter es una comilla:

        memmove(str, str + 1, len); //Movemos el inicio de la cadena a un espacio a la derecha

        len--; //Bajamos en 1 el tamaño de la cadena

    }

    if (len > 0 && str[len - 1] == '"') { //Si el último carácter es una comilla

        str[len - 1] = '\0'; //Lo cambiamos al carácter nulo para que ahí termine la cadena.

    }

}

// Menú principal
void mostrarMenuPrincipal() {
  limpiarPantalla();
  puts("========================================");
  puts("     Base de Datos de Películas");
  puts("========================================");

  puts("1) Cargar Películas");
  puts("2) Buscar por id");
  puts("3) Buscar por género");
  puts("4) Buscar por década");
  puts("5) Buscar por década y género");
  puts("6) Salir");
}

/**
 * Compara dos claves de tipo string para determinar si son iguales.
 * Esta función se utiliza para inicializar mapas con claves de tipo string.
 *
 * @param key1 Primer puntero a la clave string.
 * @param key2 Segundo puntero a la clave string.
 * @return Retorna 1 si las claves son iguales, 0 de lo contrario.
 */
int is_equal_str(void *key1, void *key2) {
  return strcmp((char *)key1, (char *)key2) == 0;
}

/**
 * Compara dos claves de tipo entero para determinar si son iguales.
 * Esta función se utiliza para inicializar mapas con claves de tipo entero.
 *
 * @param key1 Primer puntero a la clave entera.
 * @param key2 Segundo puntero a la clave entera.
 * @return Retorna 1 si las claves son iguales, 0 de lo contrario.
 */
int is_equal_int(void *key1, void *key2) {
  return *(int *)key1 == *(int *)key2; // Compara valores enteros directamente
}

/**
 * Carga películas desde un archivo CSV y las almacena en un mapa por ID.
 */
void cargar_peliculas(Map *pelis_byid) {
  // Intenta abrir el archivo CSV que contiene datos de películas
  FILE *archivo = fopen("data/Top1500.csv", "r");
  if (archivo == NULL) {
    perror(
        "Error al abrir el archivo"); // Informa si el archivo no puede abrirse
    return;
  }

  char **campos;
  // Leer y parsear una línea del archivo CSV. La función devuelve un array de
  // strings, donde cada elemento representa un campo de la línea CSV procesada.
  campos = leer_linea_csv(archivo, ','); // Lee los encabezados del CSV

  // Lee cada línea del archivo CSV hasta el final
  while ((campos = leer_linea_csv(archivo, ',')) != NULL) {
    // Crea una nueva estructura Film y almacena los datos de cada película
    Film *peli = (Film *)malloc(sizeof(Film));
    strcpy(peli->id, campos[1]);        // Asigna ID
    strcpy(peli->title, campos[5]);     // Asigna título
    strcpy(peli->director, campos[14]); // Asigna director
    
    peli->genres = list_create(); //Creamos la lista de géneros de la película

    borrarComillas(campos[11]); //Borramos las comillas en caso de que haya

    char *token = strtok(campos[11], ","); //Cortamos el campo de géneros hasta una coma

    while (token != NULL) { //Mientras token no sea NULL

        char *clean_token = espacioInicial(token); //Limpiaremos el género en caso de que tenga espacios al principio

        list_pushBack(peli->genres, strdup(clean_token)); //Ahora agregamos el género a la lista de géneros de la película

        token = strtok(NULL, ","); //Volvemos a cortar la siguiente parte de la cadena hasta la coma

    }    // Inicializa la lista de géneros
    peli->year =
        atoi(campos[10]); // Asigna año, convirtiendo de cadena a entero

    // Inserta la película en el mapa usando el ID como clave
    map_insert(pelis_byid, peli->id, peli);
  }
  fclose(archivo); // Cierra el archivo después de leer todas las líneas


  // Itera sobre el mapa para mostrar las películas cargadas
  MapPair *pair = map_first(pelis_byid);
  while (pair != NULL) {
    Film *peli = pair->value;
    printf("ID: %s, Título: %s, Director: %s, Año: %d\n", peli->id, peli->title,
           peli->director, peli->year);
    pair = map_next(pelis_byid); // Avanza al siguiente par en el mapa
  }
}

/**
 * Busca y muestra la información de una película por su ID en un mapa.
 */
void buscar_por_id(Map *pelis_byid) {
  char id[10]; // Buffer para almacenar el ID de la película

  // Solicita al usuario el ID de la película
  printf("Ingrese el id de la película: ");
  scanf("%s", id); // Lee el ID del teclado

  // Busca el par clave-valor en el mapa usando el ID proporcionado
  MapPair *pair = map_search(pelis_byid, id);

  // Si se encontró el par clave-valor, se extrae y muestra la información de la
  // película
  if (pair != NULL) {
    Film *peli =
        pair->value; // Obtiene el puntero a la estructura de la película
    // Muestra el título y el año de la película
    printf("Título: %s, Año: %d\n", peli->title, peli->year);
  } else {
    // Si no se encuentra la película, informa al usuario
    printf("La película con id %s no existe\n", id);
  }
}

void buscar_por_genero(Map *pelis_byid) {
    char genero[100];
    printf("Ingrese el género de películas a buscar: ");
    scanf("%s", genero);

    // Convertimos el género ingresado a minúsculas para hacer la búsqueda sin distinción de mayúsculas o minúsculas
    for (int i = 0; genero[i]; i++) {
        genero[i] = tolower(genero[i]);
    }

    // Variable para rastrear si se encuentra alguna película del género dado
    int found = 0;

    // Iteramos sobre todas las películas cargadas
    MapPair *pair = map_first(pelis_byid);
    while (pair != NULL) {
        Film *peli = pair->value;

        // Iteramos sobre los géneros de la película
        void *current_genre = list_first(peli->genres);
        while (current_genre != NULL) {
            char *genre = (char *)current_genre;
            // Convertimos el género de la película a minúsculas para hacer la comparación sin distinción de mayúsculas o minúsculas
            for (int i = 0; genre[i]; i++) {
                genre[i] = tolower(genre[i]);
            }
          
            // Verificamos si el género actual coincide con el género buscado
            if (strcmp(genre, genero) == 0) {
              
                // Si la película pertenece al género proporcionado, la mostramos
                printf("ID: %s, Título: %s, Año: %d", peli->id, peli->title, peli->year);
              
              // Iteramos sobre los géneros de la película e imprimimos cada uno
              printf(", Géneros: ");
              void *current_genre = list_first(peli->genres);
              while (current_genre != NULL) {
                  char *genre = (char *)current_genre;
                  printf("%s, ", genre);
                  current_genre = list_next(peli->genres);
              }
              printf("\n");
                found = 1;
                break; // Salimos del bucle interno si encontramos el género
            }
            current_genre = list_next(peli->genres); // Avanzamos al siguiente género
        }
        pair = map_next(pelis_byid); // Avanzamos al siguiente par en el mapa
    }

    // Si no se encontraron películas del género dado, mostramos un mensaje
    if (!found) {
        printf("No se encontraron películas del género %s\n", genero);
    }
}



void buscar_por_decada(Map *pelis_byid, Map *pelis_byyear) {
    char decada[5];
    printf("Ingrese la década (por ejemplo, 2010, 2000, 1990): ");
    scanf("%s", decada);

    printf("Películas de la década %s:\n", decada);

    // Calculamos los años de inicio y fin de la década
    int inicio_decada = atoi(decada);
    int fin_decada = inicio_decada + 9;

    int found = 0;

    // Iteramos sobre todas las películas cargadas
    MapPair *pair = map_first(pelis_byid);
    while (pair != NULL) {
        Film *peli = pair->value;
        int year_peli = peli->year;

        // Verificamos si el año de la película está dentro de la década proporcionada
        if (year_peli >= inicio_decada && year_peli <= fin_decada) {
            // Si la película pertenece a la década, la mostramos
            printf("ID: %s, Título: %s, Director: %s, Año: %d\n", peli->id, peli->title, peli->director, peli->year);
            found = 1;
        }
        pair = map_next(pelis_byid); // Avanzamos al siguiente par en el mapa
    }

    if (!found) {
        printf("No se encontraron películas en la década %s\n", decada);
    }
}

void buscar_por_genero_y_decada(Map *pelis_byid, Map *pelis_byyear) {
      char genero[100];
      printf("Ingrese el género de películas a buscar: ");
      scanf("%s", genero);

      char decada[5];
      printf("Ingrese la década (por ejemplo, 2010, 2000, 1990): ");
      scanf("%s", decada);

      // Convertimos el género ingresado a minúsculas para hacer la búsqueda sin distinción de mayúsculas o minúsculas
      for (int i = 0; genero[i]; i++) {
          genero[i] = tolower(genero[i]);
      }

      // Calculamos los años de inicio y fin de la década
      int inicio_decada = atoi(decada);
      int fin_decada = inicio_decada + 9;

      // Variable para rastrear si se encuentra alguna película del género y la década dados
      int found = 0;

      // Iteramos sobre todas las películas cargadas
      MapPair *pair = map_first(pelis_byid);
      while (pair != NULL) {
          Film *peli = pair->value;
          int year_peli = peli->year;

          // Verificamos si el año de la película está dentro de la década proporcionada
          if (year_peli >= inicio_decada && year_peli <= fin_decada) {
              // Iteramos sobre los géneros de la película
              void *current_genre = list_first(peli->genres);
              while (current_genre != NULL) {
                  char *genre = (char *)current_genre;
                  // Convertimos el género de la película a minúsculas para hacer la comparación sin distinción de mayúsculas o minúsculas
                  for (int i = 0; genre[i]; i++) {
                      genre[i] = tolower(genre[i]);
                  }
                  // Verificamos si el género actual coincide con el género buscado
                  if (strcmp(genre, genero) == 0) {
                      // Si la película pertenece al género y a la década proporcionados, la mostramos
                      printf("ID: %s, Título: %s, Director: %s, Año: %d\n", peli->id, peli->title, peli->director, peli->year);
                      found = 1;
                      break; // Salimos del bucle interno si encontramos el género
                  }
                  current_genre = list_next(peli->genres); // Avanzamos al siguiente género
              }
          }
          pair = map_next(pelis_byid); // Avanzamos al siguiente par en el mapa
      }

      // Si no se encontraron películas del género y la década dados, mostramos un mensaje
      if (!found) {
          printf("No se encontraron películas del género %s en la década %s\n", genero, decada);
      }
  }

int main() {
  char opcion; // Variable para almacenar una opción ingresada por el usuario
               // (sin uso en este fragmento)

  // Crea un mapa para almacenar películas, utilizando una función de
  // comparación que trabaja con claves de tipo string.
  Map *pelis_byid = map_create(is_equal_str);
  Map *pelis_bygenre = map_create(is_equal_str);
  Map *pelis_byyear = map_create(is_equal_str);

  // Recuerda usar un mapa por criterio de búsqueda

  do {
    mostrarMenuPrincipal();
    printf("Ingrese su opción: ");
    scanf(" %c", &opcion);

    switch (opcion) {
    case '1':
      cargar_peliculas(pelis_byid);
      break;
    case '2':
      buscar_por_id(pelis_byid);
      break;
    case '3':
      buscar_por_genero(pelis_byid);
      break;
    case '4':
      buscar_por_decada(pelis_byid, pelis_byyear);
      break;
    case '5':
      buscar_por_genero_y_decada(pelis_byid, pelis_byyear);
      break;
    case '6': 
      printf("Saliendo del programa...\n");
      break;
      default: 
      break;
    }
    presioneTeclaParaContinuar();

  } while (opcion != '6');

  return 0;
}
