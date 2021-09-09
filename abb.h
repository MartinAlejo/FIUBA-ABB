#include <stdlib.h>
#include <stdbool.h>
//ABB
typedef struct abb abb_t;
typedef struct nodo nodo_abb;
typedef int (*abb_comparar_clave_t) (const char *, const char *);
typedef void (*abb_destruir_dato_t) (void *);

/* ******************************************************************
 *              PRIMITIVAS DEL ARBOL BINARIO DE BUSQUEDA
 * *****************************************************************/

// Crea un arbol binario de busqueda.
// Pre: Se tiene una funcion para comprar claves.
// Post: Se creo un arbol binario de busqueda.
abb_t *abb_crear(abb_comparar_clave_t cmp, abb_destruir_dato_t destruir_dato);

// Guardar un dato en un arbol, a partir de una clave. Si la clave ya existe, pisa el dato.
// Pre: El arbol fue creado.
// Post: El dato fue guardado en el arbol, con su clave identificatoria. Devuelve true si se guardo exitosamente, false en caso contrario.
bool abb_guardar(abb_t *arbol, const char *clave, void *dato);

// Borra un dato en un arbol, a partir de una clave.
// Pre: El arbol fue creado.
// Post: Se borro el dato del arbol, y lo devuelve, o NULL si la clave no existe.
void *abb_borrar(abb_t *arbol, const char *clave);

// Obtiene el dato a partir de una clave. 
// Pre: El arbol fue creado.
// Post: Se devuelve el dato asociado a la clave, o NULL si la clave no existe.
void *abb_obtener(const abb_t *arbol, const char *clave);

// Verifica si la clave pertenece al arbol.
// Pre: El arbol fue creado.
// Post: Devuelve true si la clave pertenece al arbol, false en caso contrario.
bool abb_pertenece(const abb_t *arbol, const char *clave);

// Devuelve la cantidad de elementos almacenados en el arbol.
// Pre: El arbol fue creado.
// Post: Devuelve la cantidad de elementos almacenados en el arbol, o 0 si no hay ningun elemento.
size_t abb_cantidad(const abb_t *arbol);

// Destruye el arbol.
// Pre: El arbol fue creado.
// Post: Destruye el arbol y aplica (en el caso de existir), la funcion de destruccion a cada uno de los datos almacenados en el arbol.
void abb_destruir(abb_t *arbol);

/* ******************************************************************
 *                           ITERADORES
 * *****************************************************************/

/* Iterador Externo */

typedef struct abb_iter abb_iter_t;

// Crea un iterador externo in-order para recorrer el arbol.
// Pre: El arbol fue creado.
// Post: Devuelve el iterador del arbol.
abb_iter_t *abb_iter_in_crear(const abb_t *arbol);

// Avanza el iterador in-order.
// Pre: El iterador fue creado.
// Post: Avanza el iterador al siguiente elemento, siguiendo un orden in-order.
bool abb_iter_in_avanzar(abb_iter_t *iter);

// Devuelve la clave actual donde esta parado iterador.
// Pre: El iterador fue creado.
// Post: Se devolvio la clave actual sobre la que esta parado el iterador.
const char *abb_iter_in_ver_actual(const abb_iter_t *iter);

// Verifica si el iterador esta al final.
// Pre: El iterador fue creado.
// Post: Devuelve true si el iterador esta al final del recorrido in-order, false en caso contrario.
bool abb_iter_in_al_final(const abb_iter_t *iter);

// Destruye el iterador.
// Pre: El iterador fue creado.
// Post: El iterador fue destruido.
void abb_iter_in_destruir(abb_iter_t *iter);


/* Iterador Interno */

// Itera todos los elementos del arbol siguiendo un recorrido in-order. Se tiene por parametro un extra que puede ser NULL. Deja de iterar cuando visitar devuelve false.
// Pre: El arbol fue creado.
// Post: Recorrio todos los elementos del arbol siguiendo un recorrido in-order y les aplico la funcion "visitar" a cada uno de los
// elementos hasta el final, o hasta que la funcion devolvio false.
void abb_in_order(abb_t *arbol, bool visitar(const char *, void *, void *), void *extra);

