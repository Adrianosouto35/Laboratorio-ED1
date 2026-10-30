/*
 * entidade.h - módulo comum (struct VISÍVEL para quem inclui)
 * Reconstruído a partir do uso em atividade9.c e inventario.c.
 */
#ifndef ENTIDADE_H
#define ENTIDADE_H

#include <stdbool.h>
#include "raylib.h"

typedef enum {
    ENTIDADE_JOGADOR,
    ENTIDADE_INIMIGO,
    ENTIDADE_ITEM
} TipoEntidade;

typedef struct {
    TipoEntidade tipo;
    Vector2      pos;
    float        raio;
    int          vida;
    union {
        int valor;   // ENTIDADE_ITEM: pontos que vale
        int dano;    // ENTIDADE_INIMIGO: dano causado ao jogador
    } extra;
} Entidade;

Entidade *entidadeCriar(TipoEntidade tipo, Vector2 pos);
bool      entidadeColidiu(const Entidade *a, const Entidade *b);
void      entidadeAplicarDano(Entidade *e, int dano);
void      entidadeDesenhar(const Entidade *e);

#endif
