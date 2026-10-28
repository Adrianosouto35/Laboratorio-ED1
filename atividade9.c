#include "raylib.h"
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define LARGURA_JANELA 800
#define ALTURA_JANELA 600
#define MAX_ENTIDADES 30
#define TOTAL_INIMIGOS 5
#define TOTAL_ITENS 6
#define DANO_TIRO 20

typedef enum {
    ENTIDADE_JOGADOR,
    ENTIDADE_INIMIGO,
    ENTIDADE_ITEM
} TipoEntidade;

typedef struct Entidade Entidade;

struct Entidade {
    TipoEntidade tipo;
    Vector2 pos;
    int vida;
    float raio;
    union {
        int valor;
        int dano;
    } extra;
};

typedef struct inventario Inventario;

struct inventario {
    Entidade **vetor;
    int total;
    int capacidade;
};

Entidade *entidadeCriar(TipoEntidade tipo, Vector2 pos) {
    Entidade *e = malloc(sizeof(Entidade));
    if (e == NULL) return NULL;

    e->tipo = tipo;
    e->pos = pos;
    e->raio = 14.0f;
    e->vida = 100;

    switch (tipo) {
        case ENTIDADE_JOGADOR:
            e->raio = 16.0f;
            e->vida = 100;
            e->extra.dano = 10;
            break;
        case ENTIDADE_INIMIGO:
            e->raio = 14.0f;
            e->vida = 40;
            e->extra.dano = 1;
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

    float dx = a->pos.x - b->pos.x;
    float dy = a->pos.y - b->pos.y;
    float distancia = sqrtf(dx * dx + dy * dy);

    return distancia <= a->raio + b->raio;
}

void entidadeAplicarDano(Entidade *entidade, int dano) {
    if (entidade == NULL || dano < 0) return;

    entidade->vida -= dano;
    if (entidade->vida < 0) entidade->vida = 0;
}

void entidadeDesenhar(const Entidade *entidade) {
    if (entidade == NULL) return;

    switch (entidade->tipo) {
        case ENTIDADE_JOGADOR:
            DrawCircleV(entidade->pos, entidade->raio, BLUE);
            DrawCircleLines((int)entidade->pos.x, (int)entidade->pos.y,
                            entidade->raio, DARKBLUE);
            break;
        case ENTIDADE_INIMIGO:
            DrawCircleV(entidade->pos, entidade->raio, RED);
            DrawCircleLines((int)entidade->pos.x, (int)entidade->pos.y,
                            entidade->raio, MAROON);
            break;
        case ENTIDADE_ITEM:
            DrawCircleV(entidade->pos, entidade->raio, GOLD);
            DrawCircleLines((int)entidade->pos.x, (int)entidade->pos.y,
                            entidade->raio, ORANGE);
            break;
    }
}

Inventario *inventarioCriar(int capacidade) {
    if (capacidade <= 0) return NULL;

    Inventario *inv = malloc(sizeof(Inventario));
    if (inv == NULL) return NULL;

    inv->vetor = calloc((size_t)capacidade, sizeof(Entidade *));
    if (inv->vetor == NULL) {
        free(inv);
        return NULL;
    }

    inv->total = 0;
    inv->capacidade = capacidade;
    return inv;
}

void inventarioDestruir(Inventario *inv) {
    if (inv == NULL) return;

    for (int i = 0; i < inv->total; i++) {
        free(inv->vetor[i]);
    }

    free(inv->vetor);
    free(inv);
}

bool inventarioAdicionar(Inventario *inv, Entidade *e) {
    if (inv == NULL || e == NULL || inv->total >= inv->capacidade) {
        return false;
    }

    inv->vetor[inv->total] = e;
    inv->total++;
    return true;
}

void inventarioRemover(Inventario *inv, int indice) {
    if (inv == NULL || indice < 0 || indice >= inv->total) return;

    free(inv->vetor[indice]);
    inv->vetor[indice] = inv->vetor[inv->total - 1];
    inv->vetor[inv->total - 1] = NULL;
    inv->total--;
}

Entidade *inventarioObter(Inventario *inv, int indice) {
    if (inv == NULL || indice < 0 || indice >= inv->total) return NULL;
    return inv->vetor[indice];
}

int inventarioTotal(Inventario *inv) {
    if (inv == NULL) return 0;
    return inv->total;
}

bool inventarioEstaCheio(Inventario *inv) {
    if (inv == NULL) return false;
    return inv->total >= inv->capacidade;
}

int inventarioContarPorTipo(Inventario *inv, TipoEntidade tipo) {
    if (inv == NULL) return 0;

    int quantidade = 0;
    for (int i = 0; i < inv->total; i++) {
        if (inv->vetor[i] != NULL && inv->vetor[i]->tipo == tipo) {
            quantidade++;
        }
    }

    return quantidade;
}

int main(void) {
    srand((unsigned int)time(NULL));

    InitWindow(LARGURA_JANELA, ALTURA_JANELA,
               "Atividade 9 - TAD (Tipo Abstrato de Dados)");
    SetTargetFPS(60);

    Inventario *inventario = inventarioCriar(MAX_ENTIDADES);
    if (inventario == NULL) {
        CloseWindow();
        return 1;
    }

    Entidade *jogador = entidadeCriar(
        ENTIDADE_JOGADOR,
        (Vector2){LARGURA_JANELA / 2.0f, ALTURA_JANELA / 2.0f}
    );

    if (jogador == NULL || !inventarioAdicionar(inventario, jogador)) {
        free(jogador);
        inventarioDestruir(inventario);
        CloseWindow();
        return 1;
    }

    for (int i = 0; i < TOTAL_INIMIGOS; i++) {
        Vector2 pos = {
            (float)GetRandomValue(30, LARGURA_JANELA - 30),
            (float)GetRandomValue(65, ALTURA_JANELA - 30)
        };
        Entidade *inimigo = entidadeCriar(ENTIDADE_INIMIGO, pos);
        if (inimigo != NULL && !inventarioAdicionar(inventario, inimigo)) {
            free(inimigo);
        }
    }

    for (int i = 0; i < TOTAL_ITENS; i++) {
        Vector2 pos = {
            (float)GetRandomValue(30, LARGURA_JANELA - 30),
            (float)GetRandomValue(65, ALTURA_JANELA - 30)
        };
        Entidade *item = entidadeCriar(ENTIDADE_ITEM, pos);
        if (item != NULL && !inventarioAdicionar(inventario, item)) {
            free(item);
        }
    }

    int pontuacao = 0;
    float tempoAviso = 0.0f;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        float vel = 250.0f * dt;

        if (IsKeyDown(KEY_RIGHT)) jogador->pos.x += vel;
        if (IsKeyDown(KEY_LEFT)) jogador->pos.x -= vel;
        if (IsKeyDown(KEY_UP)) jogador->pos.y -= vel;
        if (IsKeyDown(KEY_DOWN)) jogador->pos.y += vel;

        if (jogador->pos.x < jogador->raio) jogador->pos.x = jogador->raio;
        if (jogador->pos.x > LARGURA_JANELA - jogador->raio)
            jogador->pos.x = LARGURA_JANELA - jogador->raio;
        if (jogador->pos.y < 60 + jogador->raio) jogador->pos.y = 60 + jogador->raio;
        if (jogador->pos.y > ALTURA_JANELA - jogador->raio)
            jogador->pos.y = ALTURA_JANELA - jogador->raio;

        for (int i = 1; i < inventarioTotal(inventario); i++) {
            Entidade *e = inventarioObter(inventario, i);
            if (e == NULL || !entidadeColidiu(jogador, e)) continue;

            if (e->tipo == ENTIDADE_ITEM) {
                pontuacao += e->extra.valor;
                inventarioRemover(inventario, i);
                i--;
            } else if (e->tipo == ENTIDADE_INIMIGO) {
                entidadeAplicarDano(jogador, e->extra.dano);
            }
        }

        if (IsKeyPressed(KEY_SPACE)) {
            int alvo = -1;
            float menorDistancia = 999999999.0f;

            for (int i = 1; i < inventarioTotal(inventario); i++) {
                Entidade *e = inventarioObter(inventario, i);
                if (e == NULL || e->tipo != ENTIDADE_INIMIGO) continue;

                float dx = e->pos.x - jogador->pos.x;
                float dy = e->pos.y - jogador->pos.y;
                float distancia = dx * dx + dy * dy;

                if (distancia < menorDistancia) {
                    menorDistancia = distancia;
                    alvo = i;
                }
            }

            if (alvo >= 0) {
                Entidade *e = inventarioObter(inventario, alvo);
                entidadeAplicarDano(e, DANO_TIRO);
                if (e->vida <= 0) inventarioRemover(inventario, alvo);
            }
        }

        if (inventarioEstaCheio(inventario)) {
            tempoAviso = 1.5f;
        } else if (tempoAviso > 0.0f) {
            tempoAviso -= dt;
        }

        int itensRestantes = inventarioContarPorTipo(inventario, ENTIDADE_ITEM);
        int inimigosRestantes = inventarioContarPorTipo(inventario, ENTIDADE_INIMIGO);

        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawRectangle(0, 0, LARGURA_JANELA, 55, (Color){235, 239, 245, 255});

            for (int i = 0; i < inventarioTotal(inventario); i++) {
                entidadeDesenhar(inventarioObter(inventario, i));
            }

            DrawText(TextFormat("Vida: %d   Pontuacao: %d",
                                jogador->vida, pontuacao),
                     10, 7, 20, DARKGRAY);
            DrawText(TextFormat("Entidades: %d/%d",
                                inventarioTotal(inventario), MAX_ENTIDADES),
                     10, 32, 16, GRAY);
            DrawText(TextFormat("Itens: %d   Inimigos: %d",
                                itensRestantes, inimigosRestantes),
                     390, 32, 16, DARKGRAY);

            if (tempoAviso > 0.0f) {
                DrawRectangle(180, 75, 440, 42, (Color){255, 220, 220, 255});
                DrawText("AVISO: INVENTARIO CHEIO!", 220, 86, 22, RED);
            }

            if (jogador->vida <= 0) {
                DrawRectangle(150, 240, 500, 100, (Color){255, 230, 230, 245});
                DrawText("VOCE FOI DERROTADO!", 230, 260, 30, RED);
                DrawText("Pressione ESC para sair.", 265, 300, 18, DARKGRAY);
            }

            DrawText("Setas movem | ESPACO ataca | ESC sai",
                     10, ALTURA_JANELA - 25, 16, GRAY);
        EndDrawing();
    }

    inventarioDestruir(inventario);
    CloseWindow();
    return 0;
}
