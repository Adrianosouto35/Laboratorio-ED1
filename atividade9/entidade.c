/*
 * entidade.c - implementação do módulo Entidade
 * Reconstruído a partir do uso em atividade9.c e inventario.c.
 */
#include <stdlib.h>
#include "entidade.h"

Entidade *entidadeCriar(TipoEntidade tipo, Vector2 pos) {
    Entidade *e = (Entidade *)malloc(sizeof(Entidade));
    if (e == NULL) return NULL;

    e->tipo = tipo;
    e->pos  = pos;

    switch (tipo) {
        case ENTIDADE_JOGADOR:
            e->raio = 15.0f;
            e->vida = 100;
            e->extra.valor = 0;
            break;
        case ENTIDADE_INIMIGO:
            e->raio = 18.0f;
            e->vida = 60;
            e->extra.dano = 1;      // dano por frame de contato
            break;
        case ENTIDADE_ITEM:
            e->raio = 10.0f;
            e->vida = 1;
            e->extra.valor = 10;
            break;
    }
    return e;
}

bool entidadeColidiu(const Entidade *a, const Entidade *b) {
    if (a == NULL || b == NULL) return false;
    return CheckCollisionCircles(a->pos, a->raio, b->pos, b->raio);
}

void entidadeAplicarDano(Entidade *e, int dano) {
    if (e == NULL) return;
    e->vida -= dano;
    if (e->vida < 0) e->vida = 0;
}

void entidadeDesenhar(const Entidade *e) {
    if (e == NULL) return;

    Color cor = BLUE;
    if (e->tipo == ENTIDADE_INIMIGO) cor = RED;
    else if (e->tipo == ENTIDADE_ITEM) cor = GOLD;

    DrawCircleV(e->pos, e->raio, cor);
}
