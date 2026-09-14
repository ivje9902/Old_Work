#include "../../src/plugins/typemem/typemem.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

struct cell
{
    struct cell *cell;
    int i;
    char *string;
};

struct listt
{
    struct cell *first;
    struct cell *last;
};

void c_then_list()
{
    // Allokerar minne för första cellen från typen
    struct cell *c = allocate_from_type(struct cell);
    assert(rc(c) == 0);
    retain(c);
    assert(rc(c) == 1);

    // Allokerar minne för nästa cell från typen
    c->cell = allocate_from_type(struct cell);
    assert(rc(c->cell) == 0);
    retain(c->cell);
    assert(rc(c->cell) == 1);

    // Länkar cellerna
    c->cell->cell = allocate_from_type(struct cell);
    retain(c->cell->cell);

    // Sista cellen pekar på NULL
    c->cell->cell->cell = NULL;



    // Allokerar minne för en list från typen
    struct listt *ll = allocate_from_type(struct listt);
    retain(ll);
    ll->first = c;
    retain(c);
    ll->last = c->cell->cell;
    retain(c->cell->cell);

    /*___Demo release list -> cell__
     *
     *Anropar release på första cellen, sen listan
     *kollar att reference count stämmer
     */
    release(c);

    assert(rc(c) == 1);
    assert(rc(c->cell) == 1);
    assert(rc(c->cell->cell) == 2);

    release(ll);
}

void list_then_c()
{
    // Allokerar minne för första cellen från typen
    struct cell *c = allocate_from_type(struct cell);
    assert(rc(c) == 0);
    retain(c);
    assert(rc(c) == 1);

    // Allokerar minne för nästa cell från typen
    c->cell = allocate_from_type(struct cell);
    assert(rc(c->cell) == 0);
    retain(c->cell);
    assert(rc(c->cell) == 1);

    // Länkar cellerna
    c->cell->cell = allocate_from_type(struct cell);
    retain(c->cell->cell);

    // Sista cellen pekar på NULL
    c->cell->cell->cell = NULL;



    // Allokerar minne för en list från typen
    struct listt *ll = allocate_from_type(struct listt);
    retain(ll);
    ll->first = c;
    retain(c);
    ll->last = c->cell->cell;
    retain(c->cell->cell);

    assert(rc(c) == 2);
    assert(rc(c->cell) == 1);
    assert(rc(c->cell->cell) == 2);

    /*___Demo release list -> cell__
     *
     *Anropar release på listan sen första cellen
     *kollar att reference count stämmer
     */
    release(ll);

    assert(rc(c) == 1);
    assert(rc(c->cell) == 1);
    assert(rc(c->cell->cell) == 1);

    release(c);
}

int main(void)
{
    // Registrer cell structen
    register_type(struct cell, cell);
    // Registrerar list stucten
    register_type(struct listt, first, last);


    list_then_c();
    c_then_list();

    exit(0);
}
