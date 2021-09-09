#define _POSIX_C_SOURCE 200809L
#include "abb.h"
#include "pila.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

typedef struct abb_nodo {
    struct abb_nodo *izq;
    struct abb_nodo *der;
    char *clave;
    void *dato;
} abb_nodo_t;

struct abb {
    abb_nodo_t *raiz;
    abb_comparar_clave_t cmp;
    abb_destruir_dato_t destruir_dato;
    size_t cant;
};

typedef int (*abb_comparar_clave_t) (const char *, const char *);
typedef void (*abb_destruir_dato_t) (void *);

// Devuelve el padre de un nodo, o el nodo a partir de una clave, un arbol, una raiz y el anterior a la raiz (NULL)
// Si "padre" es true, se devolvera al padre del nodo que buscamos, si es false, se devuelve el nodo
// Si el nodo no existe, se devuelve un NULL en caso de devolver el nodo, o en caso de devolver el padre, se devuelve quien
// deberia ser el padre de dicho nodo
abb_nodo_t *buscar_nodo_abb(const abb_t *arbol, abb_nodo_t *raiz, const char *clave, abb_nodo_t *padre_raiz, bool padre) {
    if (!raiz && !padre) return NULL; // La clave no existe en el arbol
    if (!raiz && padre) return padre_raiz; // La clave no existe en el arbol, pero devolvemos el padre de donde deberia estar

    int cmp_clave = arbol->cmp(raiz->clave, clave);
    if (cmp_clave > 0) {
        return buscar_nodo_abb(arbol, raiz->izq, clave, raiz, padre); // La clave actual es mayor que la buscamos, nos vamos al hijo izquierdo
    }
    else if (cmp_clave < 0) {
        return buscar_nodo_abb(arbol, raiz->der, clave, raiz, padre); // La clave actual es menor que la que buscamos, nos vamos al hijo derecho
    }

    if (padre) return padre_raiz; // Es el padre del nodo que estabamos buscando. Si el nodo que buscamos es la raiz, entonces es NULL

    return raiz; // Es el nodo que estabamos buscando
}

// Crea un nodo a partir de su clave y dato
abb_nodo_t *crear_nodo(const char *clave, void *dato){
    if (!clave) return NULL;

    abb_nodo_t *nodo_abb = malloc(sizeof(abb_nodo_t));
    if (!nodo_abb) return NULL;

    nodo_abb->izq = NULL;
    nodo_abb->der = NULL;
    nodo_abb->dato = dato;
    nodo_abb->clave = strdup(clave);

    return nodo_abb;
}

// Crea un arbol a partir de una funcion de comparacion y de destruccion
abb_t *abb_crear(abb_comparar_clave_t cmp, abb_destruir_dato_t destruir_dato){
    abb_t *arbol = malloc(sizeof(abb_t));
    if (!arbol) return NULL;

    arbol->raiz = NULL;
    arbol->cmp = cmp;
    arbol->destruir_dato = destruir_dato;
    arbol->cant = 0;

    return arbol;
}

// Guarda un dato en un arbol dada una clave que lo identifica
bool abb_guardar(abb_t *arbol, const char *clave, void *dato) {
    // Caso en el que no hay raiz, se guarda como raiz
    if (!arbol->raiz) {
        abb_nodo_t *raiz = crear_nodo(clave, dato);
        if (!raiz) return false;
        arbol->raiz = raiz;
        arbol->cant++;
        return true;
    }

    // Hay raiz, vemos si el nodo ya existe
    abb_nodo_t *nodo = buscar_nodo_abb(arbol, arbol->raiz, clave, NULL, false);
    if (!nodo) {
        // No hay nodo en el arbol, creamos uno nuevo
        abb_nodo_t *nuevo = crear_nodo(clave, dato);
        if (!nuevo) return false;

        arbol->cant++;
        abb_nodo_t *lugar = buscar_nodo_abb(arbol, arbol->raiz, clave, NULL, true); // Ahora lugar apunta al nodo al cual hay que insertarle como uno de sus hijos el nuevo nodo

        // Vemos si guardamos el nuevo nodo como hijo izquierdo o hijo derecho de "lugar"
        int comparacion = arbol->cmp(lugar->clave, clave);
        if (comparacion < 0) {
            lugar->der = nuevo;
        }
        else if (comparacion > 0) {
            lugar->izq = nuevo;
        }
    }
    else {
        // El nodo existe, reemplazamos su dato
        if (arbol->destruir_dato) {
            arbol->destruir_dato(nodo->dato);
        }
        nodo->dato = dato;
    }

    return true;
}

void *abb_borrar(abb_t *arbol, const char *clave) {
    if (!arbol->raiz) {
        return NULL; // No hay raiz, por ende nada que borrar
    }
    bool borrar_raiz = false;
    void *dato_aux = NULL;

    abb_nodo_t *padre = buscar_nodo_abb(arbol, arbol->raiz, clave, NULL, true); // Buscamos el padre del nodo que queremos borrar
    if (!padre) {
        borrar_raiz = true; // En este caso, se quiere borrar la raiz
    }

    abb_nodo_t *hijo = buscar_nodo_abb(arbol, arbol->raiz, clave, NULL, false); // Buscamos el nodo que queremos borrar
    if (!hijo) {
        return NULL; // El nodo a borrar no existe
    }

    // Llegados a este punto, se va a borrar el nodo
    arbol->cant -= 1; 

    // Vemos todos los casos posibles de borrados
    if (!hijo->izq && !hijo->der) {
        // El hijo a borrar es una hoja por lo tanto solo liberamos y devolvemos
        if (borrar_raiz) arbol->raiz = NULL;

        if (padre && padre->izq == hijo) {
            padre->izq = NULL; // El hijo que queremos borrar es el izquierdo
        }
        else if (padre && padre->der == hijo) {
            padre->der = NULL; // El hijo que queremos borrar es el derecho
        }
        dato_aux = hijo->dato;
        free(hijo->clave);
        free(hijo);

        return dato_aux;
    }
    else if (!hijo->izq && hijo->der) {
        // El hijo a borrar, tiene un solo hijo (el hijo derecho)
        if (borrar_raiz) arbol->raiz = hijo->der;

        if (padre && padre->izq == hijo) {
            padre->izq = hijo->der; // El hijo que queremos borrar es el izquierdo
        }
        else if (padre && padre->der == hijo) {
            padre->der = hijo->der; // El hijo que queremos borrar es el derecho
        }
        dato_aux = hijo->dato;
        free(hijo->clave);
        free(hijo);

        return dato_aux;
    }
    else if (hijo->izq && !hijo->der) {
        // El hijo a borrar, tiene un solo hijo (el hijo izquierdo)
        if (borrar_raiz) arbol->raiz = hijo->izq;

        if (padre && padre->izq == hijo) {
            padre->izq = hijo->izq; // El hijo que queremos borrar es el izquierdo
        }
        else if (padre && padre->der == hijo) {
            padre->der = hijo->izq; // El hijo que queremos borrar es el derecho
        }
        dato_aux = hijo->dato;
        free(hijo->clave);
        free(hijo);

        return dato_aux; 
    }
    
    // Llegados aca, el hijo a borrar, tiene dos hijos. Buscamos un reemplazante (el inmediato de la derecha)
    abb_nodo_t *reemplazante = hijo->der;
    while (reemplazante->izq) {
        reemplazante = reemplazante->izq;
    }
    // El reemplazante ahora es el inmediato de la derecha (y va a ser una hoja o tener un hijo derecho). Copiamos clave y dato
    char *clave_copia = reemplazante->clave;
    void *dato_copia = reemplazante->dato;

    // Buscamos al padre del reemplazante para cambiar su referencia
    abb_nodo_t *padre_reemplazante = buscar_nodo_abb(arbol, arbol->raiz, clave_copia, NULL, true); 
    if (padre_reemplazante->izq == reemplazante) {
        padre_reemplazante->izq = reemplazante->der; // Si el reemplazante no tiene hijos, entonces va a ser NULL, y si tiene hijos, solo va a tener un hijo derecho
    }
    else if (padre_reemplazante->der == reemplazante) {
        padre_reemplazante->der = reemplazante->der;
    }
    free(reemplazante);

    // Ahora piso el hijo a borrar por su reemplazante, y me guardo su dato para devolverlo
    dato_aux = hijo->dato; // Me guardo el dato del nodo que voy a pisar para devolverlo
    free(hijo->clave); // Libero la clave ya que voy a reemplazarla por la de su reemplazante
    hijo->clave = clave_copia;
    hijo->dato = dato_copia;

    return dato_aux;
}

void *abb_obtener(const abb_t *arbol, const char *clave){
    abb_nodo_t *actual = buscar_nodo_abb(arbol, arbol->raiz, clave, NULL, false);
    return !actual ? NULL : actual->dato;
}

bool abb_pertenece(const abb_t *arbol, const char *clave){
    abb_nodo_t *actual = buscar_nodo_abb(arbol, arbol->raiz, clave, NULL, false);
    return !actual ? false : true;
}

size_t abb_cantidad(const abb_t *arbol){
    return arbol->cant;
}

// Funcion auxiliar para destruir el arbol
void _abb_destruir(abb_nodo_t *raiz, const abb_t *arbol){
    if (!raiz) {
        return;
    }
    _abb_destruir(raiz->izq, arbol);
    _abb_destruir(raiz->der, arbol);

    if (arbol->destruir_dato) arbol->destruir_dato(raiz->dato); // Destruimos el dato
    free(raiz->clave); // Destruimos la clave
    free(raiz); // Destruimos el nodo
    
}

// Destruye el arbol
void abb_destruir(abb_t *arbol){
    _abb_destruir(arbol->raiz, arbol);
    free(arbol);
}

/* Primitivas de los iteradores */

struct abb_iter{
    pila_t *pila;
};


void apilar_izquierdos(abb_iter_t* iter,abb_nodo_t* raiz){
    while(raiz){
        pila_apilar(iter->pila,raiz);
        raiz = raiz->izq;
    }
}


// Crea el iterador para un abb
abb_iter_t *abb_iter_in_crear(const abb_t *arbol){

    abb_iter_t* iter = malloc(sizeof(abb_iter_t));
    if (!iter) return NULL;

    pila_t *pila = pila_crear();
    if (!pila) {
        free(iter);
        return NULL;
    }

    iter->pila = pila; // Asignamos la pila al iterador
    apilar_izquierdos(iter, arbol->raiz); // Inicializamos la pila con la raiz para no perder referencia a ningun nodo

    return iter;
}

// Avanza el iterador, devuelve true si se pudo avanzar, false en caso contrario
bool abb_iter_in_avanzar(abb_iter_t *iter){
    if(abb_iter_in_al_final(iter)) return false;

    abb_nodo_t* nodo = pila_desapilar(iter->pila);
    if(nodo->der){
        apilar_izquierdos(iter,nodo->der);
    }
    return true;
}

// Devuelve la clave actual del iterador del abb
const char *abb_iter_in_ver_actual(const abb_iter_t *iter){
    abb_nodo_t* actual = pila_ver_tope(iter->pila);
    return actual ? actual->clave : NULL;
}

// Devuelve true si el iterador esta al final, false en caso contrario
bool abb_iter_in_al_final(const abb_iter_t *iter){
    return pila_esta_vacia(iter->pila);
}

// Destruye el iterador del abb
void abb_iter_in_destruir(abb_iter_t *iter){
    pila_destruir(iter->pila);
    free(iter);
}

// Funcion auxiliar para el iterador interno
void _abb_in_order(abb_nodo_t *nodo_act, bool visitar(const char *, void *, void *), void *extra, int *cortar) {
    if (!nodo_act || *cortar) return;

    _abb_in_order(nodo_act->izq, visitar, extra, cortar);
    if (*cortar) return;
    if (!visitar(nodo_act->clave, nodo_act->dato, extra)) {
        *cortar = 1;
        return;
    }
    if (!*cortar) _abb_in_order(nodo_act->der, visitar, extra, cortar);
}

// Iterador Interno
void abb_in_order(abb_t *arbol, bool visitar(const char *, void *, void *), void *extra) {
    if (!arbol->raiz) return;
    int *cortar = malloc(sizeof(int));
    *cortar = 0;
    _abb_in_order(arbol->raiz, visitar, extra, cortar);
    free(cortar);
}