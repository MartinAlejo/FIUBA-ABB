#include "pila.h"

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stddef.h>


#define CAPACIDAD_INIC 10
/* Definición del struct pila proporcionado por la cátedra.
 */
struct pila {
    void **datos;
    size_t cantidad;   // Cantidad de elementos almacenados.
    size_t capacidad;  // Capacidad del arreglo 'datos'.
};

/* *****************************************************************
 *                    PRIMITIVAS DE LA PILA
 * *****************************************************************/

bool _pila_modificar_tamano(pila_t *pila, size_t tam_nuevo) {
    void *datos_nuevos = realloc(pila -> datos, sizeof(void*) * tam_nuevo);
    if (datos_nuevos == NULL) {
        return false;
    }

    pila -> capacidad = tam_nuevo;
    pila -> datos = datos_nuevos;
    return true;
}

pila_t *pila_crear(void) {
    pila_t *pila = malloc(sizeof(pila_t));
    if (pila == NULL) {
        return NULL;
    }

    pila -> datos = malloc(sizeof(void*) * CAPACIDAD_INIC);

    if (pila -> datos == NULL) {
        free(pila);
        return NULL;
    }

    pila -> cantidad = 0;
    pila -> capacidad = CAPACIDAD_INIC;
    return pila;
}

void pila_destruir(pila_t *pila) {
    free(pila -> datos);
    free(pila);
}

bool pila_esta_vacia(const pila_t *pila) {
    return pila -> cantidad == 0;
}

bool pila_apilar(pila_t *pila, void *valor) {
    if (pila -> cantidad >= pila -> capacidad) {
        size_t tam_nuevo = pila -> capacidad * 2;
        if (!_pila_modificar_tamano(pila, tam_nuevo)) {
            return false;
        }
    }

    pila -> datos[pila -> cantidad] = valor;
    pila -> cantidad += 1;
    return true;
}

void *pila_ver_tope(const pila_t *pila) {
    if (pila_esta_vacia(pila)) {
        return NULL;
    }
    size_t pos_tope = pila -> cantidad - 1;
    return pila -> datos[pos_tope];
}

void *pila_desapilar(pila_t *pila) {
    if (pila_esta_vacia(pila)) {
        return NULL;
    }

    void *desapilado = pila_ver_tope(pila);
    pila -> cantidad -= 1;

    if ((pila -> cantidad * 4 <= pila -> capacidad) && (pila -> capacidad >= CAPACIDAD_INIC)) {
        size_t tam_nuevo = pila -> capacidad / 2;
        _pila_modificar_tamano(pila, tam_nuevo);
    }

    return desapilado;
}
