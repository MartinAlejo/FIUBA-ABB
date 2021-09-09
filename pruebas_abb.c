#define _POSIX_C_SOURCE 200809L
#include "testing.h"
#include "abb.h"
#include "pila.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

// Compara dos claves
// Pre: Las claves van a ser numeros pero como strings
// Post: Devuelve 1 si la primer clave es mayor que la segunda, -1 si la primera es menor o 0 si son iguales
int comparar_claves(const char *clave_1, const char *clave_2) {
	int clave_1_aux = atoi(clave_1);
	int clave_2_aux = atoi(clave_2);
	//printf("La clave 1: %d La clave 2: %d\n", clave_1_aux, clave_2_aux);
	if (clave_1_aux > clave_2_aux) {
		return 1;
	}
	else if (clave_1_aux < clave_2_aux) {
		return -1;
	}
	return 0;
}

/*
// Funcion auxiliar para "visitar_print"
void _visitar_print(const char *clave, void *dato) {
	printf("La clave es: %s, el dato es: %d\n", clave, *(int*)dato);
}

// Una funcion visitar para debug interno, la cual imprime la clave y el dato de los nodos
bool visitar_print(const char *clave, void *dato, void *extra) {
	_visitar_print(clave, dato);
	return true;
}
*/

// Funcion auxiliar para "sumar_1_primeros_3"
void _sumar_1_primeros_3(void *dato) {
	*(int*)dato += 1;
}

// Una funcion visitar para test, la cual imprime la clave y el dato de los nodos
static bool sumar_1_primeros_3(const char *clave, void *dato, void *extra) {
	if (*(int*)extra >= 3) {
		free(extra);
		return false;
	}
	_sumar_1_primeros_3(dato);
	*(int*)extra += 1;
	return true;
}

static void prueba_crear_y_destruir_arbol(void) {
	abb_t *arbol_1 = abb_crear(comparar_claves, free);
	abb_t *arbol_2 = abb_crear(comparar_claves, NULL);
	print_test("Se crearon los arboles", 1);
	abb_destruir(arbol_1);
	abb_destruir(arbol_2);
	print_test("Se eliminaron los arboles", 1); // Si falla, valgrind tiraria error
}

static void prueba_guardar_arbol(void) {
	abb_t *arbol = abb_crear(comparar_claves, free);
	char *dato = strdup("hola");
	bool se_guardo = abb_guardar(arbol, "3", dato);
	print_test("Se guardo el dato", se_guardo);
	print_test("El dato que se guardo es el correcto", !strcmp((char*)abb_obtener(arbol, "3"), "hola"));

	char *dato_2 = strdup("mundo");
	abb_guardar(arbol, "3", dato_2);
	print_test("Se piso el dato anterior", !strcmp((char*)abb_obtener(arbol, "3"), "mundo"));
	abb_destruir(arbol);
}

static void prueba_pertenece(void) {
	abb_t *arbol = abb_crear(comparar_claves, free);
	char *dato = strdup("hola");
	print_test("La clave no pertenece", !abb_pertenece(arbol, "3"));
	abb_guardar(arbol, "3", dato);
	print_test("La clave pertenece", abb_pertenece(arbol, "3"));
	abb_destruir(arbol);
}

static void prueba_obtener(void) {
	abb_t *arbol = abb_crear(comparar_claves, free);
	char *dato_1 = strdup("hola");
	char *dato_2 = strdup("mundo");
	abb_guardar(arbol, "3", dato_1);
	abb_guardar(arbol, "4", dato_2);
	print_test("No se pudo obtener el dato de una clave que no existe", !abb_obtener(arbol, "5"));
	print_test("Se obtuvo el dato exitosamente 1", !strcmp((char*)abb_obtener(arbol, "3"), "hola"));
	print_test("Se obtuvo el dato exitosamente 2", !strcmp((char*)abb_obtener(arbol, "4"), "mundo"));
	abb_destruir(arbol);
}

static void prueba_cantidad(void) {
	abb_t *arbol = abb_crear(comparar_claves, free);
	print_test("La cantidad de un arbol recien creado es 0", abb_cantidad(arbol) == 0);
	char *dato_1 = strdup("hola");
	char *dato_2 = strdup("mundo");
	abb_guardar(arbol, "3", dato_1);
	abb_guardar(arbol, "4", dato_2);
	print_test("La cantidad del arbol ahora es 2", abb_cantidad(arbol) == 2);
	abb_destruir(arbol);
}

static void prueba_iter_interno_sumar_1_a_todos(void) {
	int *dato_1 = malloc(sizeof(int));
	*dato_1 = 4;
	int *dato_2 = malloc(sizeof(int));
	*dato_2 = 2;
	int *dato_3 = malloc(sizeof(int));
	*dato_3 = 1;
	int *dato_4 = malloc(sizeof(int));
	*dato_4 = 6;
	int *dato_5 = malloc(sizeof(int));
	*dato_5 = 7;
	int *dato_6 = malloc(sizeof(int));
	*dato_6 = 5;
	int *dato_7 = malloc(sizeof(int));
	*dato_7 = 3;

	abb_t *arbol = abb_crear(comparar_claves, free);
	abb_guardar(arbol, "4", dato_1);
	abb_guardar(arbol, "2", dato_2);
	abb_guardar(arbol, "1", dato_3);
	abb_guardar(arbol, "6", dato_4);
	abb_guardar(arbol, "7", dato_5);
	abb_guardar(arbol, "5", dato_6);
	abb_guardar(arbol, "3", dato_7);

	print_test("El primer elemento del recorrido in-order vale 1", *(int*)abb_obtener(arbol, "1") == 1);
	print_test("El segundo elemento del recorrido in-order vale 2", *(int*)abb_obtener(arbol, "2") == 2);
	print_test("El tercer elemento del recorrido in-order vale 3", *(int*)abb_obtener(arbol, "3") == 3);
	print_test("El cuarto elemento del recorrido in-order vale 4", *(int*)abb_obtener(arbol, "4") == 4);

	int *extra = malloc(sizeof(int));
	*extra = 0;
	abb_in_order(arbol, sumar_1_primeros_3, extra);

	print_test("Le sumo 1 al primer elemento del recorrido in-order, ahora vale 2", *(int*)abb_obtener(arbol, "1") == 2);
	print_test("Le sumo 1 al segundo elemento del recorrido in-order, ahora vale 3", *(int*)abb_obtener(arbol, "2") == 3);
	print_test("Le sumo 1 al tercer elemento del recorrido in-order, ahora vale 4", *(int*)abb_obtener(arbol, "3") == 4);
	print_test("El cuarto elemento del recorrido in-order, sigue valiendo 4", *(int*)abb_obtener(arbol, "4") == 4);

	abb_destruir(arbol);
}

static void prueba_borrado_dos_hijos_y_raiz(void) {
	int *dato_1 = malloc(sizeof(int));
	*dato_1 = 4;
	int *dato_2 = malloc(sizeof(int));
	*dato_2 = 2;
	int *dato_3 = malloc(sizeof(int));
	*dato_3 = 1;
	int *dato_4 = malloc(sizeof(int));
	*dato_4 = 6;
	int *dato_5 = malloc(sizeof(int));
	*dato_5 = 7;
	int *dato_6 = malloc(sizeof(int));
	*dato_6 = 5;
	int *dato_7 = malloc(sizeof(int));
	*dato_7 = 3;

	abb_t *arbol = abb_crear(comparar_claves, free);
	abb_guardar(arbol, "4", dato_1);
	abb_guardar(arbol, "2", dato_2);
	abb_guardar(arbol, "1", dato_3);
	abb_guardar(arbol, "6", dato_4);
	abb_guardar(arbol, "7", dato_5);
	abb_guardar(arbol, "5", dato_6);
	abb_guardar(arbol, "3", dato_7);
	//abb_in_order(arbol, visitar_print, NULL);
	
	void *dato_borrado;
	dato_borrado = abb_borrar(arbol, "6");
	print_test("Se borro correctamente el nodo con clave '6'", dato_borrado == dato_4);
	print_test("La cantidad es 6", abb_cantidad(arbol) == 6);
	free(dato_borrado);
	//abb_in_order(arbol, visitar_print, NULL);

	dato_borrado = abb_borrar(arbol, "4");
	print_test("Se borro correctamente el nodo con clave '4'", dato_borrado == dato_1);
	free(dato_borrado);
	//abb_in_order(arbol, visitar_print, NULL);

	abb_destruir(arbol);
}

static void prueba_borrado_cambio_raiz(void) {
	int *dato_1 = malloc(sizeof(int));
	*dato_1 = 1;
	int *dato_2 = malloc(sizeof(int));
	*dato_2 = 2;
	abb_t *arbol = abb_crear(comparar_claves, free);
	abb_guardar(arbol, "1", dato_1);
	abb_guardar(arbol, "2", dato_2);
	//abb_in_order(arbol, visitar_print, NULL);
	// Ahora nos deberia quedar un arbol con "1" como raiz y como dato debe tener un 1, y a su derecha deberia haber un nodo con
	// un "2" de clave, y un 2 de dato
	void *dato_borrado = abb_borrar(arbol, "1");
	print_test("Se borro correctamente el nodo con clave '1'", dato_borrado == dato_1);
	print_test("La cantidad es 1", abb_cantidad(arbol) == 1);
	//abb_in_order(arbol, visitar_print, NULL);
	free(dato_borrado);
	abb_destruir(arbol);
}
/*
static void prueba_de_volumen(size_t tam){
	printf("-----------Inicio de prueba de volumen-----------\n");
	abb_t* abb = abb_crear(comparar_claves,free);
	print_test("Se creo el ABB correctamente",abb);
	print_test("El ABB se encuentra vacio",abb_cantidad(abb) == 0);
	char** claves = malloc(sizeof(char*) * tam);
	bool ok = true;
	for (size_t i = 0; i < tam; i++){
		ok = abb_guardar(abb,claves[i],(void*)i);
	}
}
*/
static void prueba_iterador_vacio(void){
	printf("-----------Inicio de prueba de iterador con elementos-----------\n");
	abb_t* arbol = abb_crear(strcmp, NULL);
	abb_iter_t* iter = abb_iter_in_crear(arbol);
	print_test("El iterador se creo correctamente", iter);
	print_test("El iterador se encuentra al final",abb_iter_in_al_final(iter));
	print_test("Ver actual devuelve NULL",abb_iter_in_ver_actual(iter) == NULL);
	print_test("Avanzar el iterador devuelve false",!abb_iter_in_avanzar(iter));

	abb_iter_in_destruir(iter);
    abb_destruir(arbol);
}

static void prueba_iterador_con_elementos(void){
	printf("-----------Inicio de prueba de iterador con elementos-----------\n");
    abb_t* arbol = abb_crear(strcmp, NULL);

	char* nombres[] = {"Alan","Matias","Gabriel","Juan","Tobias"};
    int valores[] = {1,2,3,4,5};

	print_test("Guardo un elemento: ",abb_guardar(arbol, nombres[0], &valores[0]));
	print_test("Guardo un elemento: ",abb_guardar(arbol, nombres[1], &valores[1]));
	print_test("Guardo un elemento: ",abb_guardar(arbol, nombres[2], &valores[2]));
	print_test("Guardo un elemento: ",abb_guardar(arbol, nombres[3], &valores[3]));
	print_test("Guardo un elemento: ",abb_guardar(arbol, nombres[4], &valores[4]));

    print_test("Alan pertenece al arbol: ", abb_pertenece(arbol, "Alan"));
    print_test("Matias pertenece al arbol: ", abb_pertenece(arbol, "Matias"));
    print_test("Gabriel pertenece al arbol: ", abb_pertenece(arbol, "Gabriel"));
	print_test("Juan pertenece al arbol: ", abb_pertenece(arbol, "Juan"));
    print_test("Tobias pertenece al arbol: ", abb_pertenece(arbol, "Tobias"));

    abb_iter_t* iter = abb_iter_in_crear(arbol);

    print_test("Iter no esta al final: ", !abb_iter_in_al_final(iter));
    print_test("Elemento actual es 'Alan: ", strcmp(abb_iter_in_ver_actual(iter),"Alan") == 0);
	print_test("Avanzo el iterador",abb_iter_in_avanzar(iter));
    print_test("Elemento actual es 'Gabriel: ", strcmp(abb_iter_in_ver_actual(iter),"Gabriel") == 0);
	print_test("Avanzo el iterador",abb_iter_in_avanzar(iter));
	print_test("El iterador no se encuentra al final",!abb_iter_in_al_final(iter));
	print_test("Elemento actual es 'Juan: ", strcmp(abb_iter_in_ver_actual(iter),"Juan") == 0);
	print_test("Avanzo el iterador",abb_iter_in_avanzar(iter));
	print_test("El iterador no se encuentra al final",!abb_iter_in_al_final(iter));
	print_test("Elemento actual es 'Matias: ", strcmp(abb_iter_in_ver_actual(iter),"Matias") == 0);
	print_test("Avanzo el iterador",abb_iter_in_avanzar(iter));
	print_test("El iterador no se encuentra al final",!abb_iter_in_al_final(iter));
	print_test("Elemento actual es 'Tobias: ", strcmp(abb_iter_in_ver_actual(iter),"Tobias") == 0);
	print_test("Avanzo el iterador", abb_iter_in_avanzar(iter));
    print_test("El iterador se encuentra al final", abb_iter_in_al_final(iter));
    print_test("Ver actual devuelve NULL ", abb_iter_in_ver_actual(iter) == NULL);
    print_test("No puedo avanzar mas el iterador", !abb_iter_in_avanzar(iter));

	abb_iter_in_destruir(iter);
    abb_destruir(arbol);
}

void pruebas_generales(void) {
	int *dato_1 = malloc(sizeof(int));
	*dato_1 = 1;
	int *dato_2 = malloc(sizeof(int));
	*dato_2 = 2;
	abb_t *arbol = abb_crear(comparar_claves, free);
	abb_guardar(arbol, "1", dato_1);
	abb_guardar(arbol, "2", dato_2);
	// Ahora nos deberia quedar un arbol con "1" como raiz y como dato debe tener un 1, y a su derecha deberia haber un nodo con
	// un "2" de clave, y un 2 de dato
	void *dato_borrado = abb_borrar(arbol, "2");
	print_test("Se borro correctamente el nodo con clave '2'", dato_borrado == dato_2);
	free(dato_borrado);
	dato_borrado = abb_borrar(arbol, "5");
	print_test("No se pudo borrar una clave inexistente en un arbol no vacio", dato_borrado == NULL);
	free(dato_borrado);
	dato_borrado = abb_borrar(arbol, "1");
	print_test("Se borro correctamente el nodo con clave '1'", dato_borrado == dato_1);
	print_test("La cantidad es 0", abb_cantidad(arbol) == 0);
	free(dato_borrado);
	dato_borrado = abb_borrar(arbol, "1");
	print_test("No se pudo borrar una clave en un arbol vacio", dato_borrado == NULL);
	free(dato_borrado);
	abb_destruir(arbol);

}

void pruebas_abb_estudiante(void) {
	pruebas_generales();
	prueba_crear_y_destruir_arbol();
	prueba_guardar_arbol();
	prueba_pertenece();
	prueba_obtener();
	prueba_cantidad();
	prueba_iter_interno_sumar_1_a_todos();
	prueba_borrado_cambio_raiz();
	prueba_borrado_dos_hijos_y_raiz();
	//prueba_de_volumen(1000);
	prueba_iterador_vacio();
	prueba_iterador_con_elementos();
}

/*
int main(void) {
	pruebas_abb_estudiante();
	return 0;
}
*/