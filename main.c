#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <ctype.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define RIGHE 32
#define COLONNE 32
#define INPUT_DIM (RIGHE * COLONNE) // 1024 ingressi (32x32)
#define HIDDEN_DIM 16               // Neuroni strato nascosto
#define OUTPUT_DIM 5                // 5 neuroni -> 2^5 = 32 combinazioni (a..z)

void ridimensionaInput(const unsigned char raw[], int larg_src, int alt_src,
                       float dst[], int larg_dst, int alt_dst);
int caricaImmagine(const char *nomeFile, float input[]);
void inizializzaPesi(float w[], int num_pesi);
float sommatoria(const float input[], const float pesi[], int dim);
float sigmoide(float sum, float amp, float trasl);
void forwardStrato(const float input[], const float pesi[], float output[], int dim_in, int dim_out);
char decodificaLettera(const float output[], int num_neuroni, char binario_str[]);
void letteraInTarget(char lettera, float target[], int num_neuroni);
float erroreQuadroSingolo(float target, float previsione);
float erroreQuadraticoMedio(const float target[], const float previsioni[], int dim);

int main(void) {
    srand((unsigned int)time(NULL));

    char nomeFile[256];
    printf("Inserisci il percorso dell'immagine (.png o .jpg): ");
    if (scanf(" %255[^\n]", nomeFile) != 1) {
        fprintf(stderr, "Errore nella lettura del nome file.\n");
        return 1;
    }

    float *input = malloc(INPUT_DIM * sizeof *input);
    float *pesi_in_hid = malloc((HIDDEN_DIM * INPUT_DIM) * sizeof *pesi_in_hid);
    float *hidden = malloc(HIDDEN_DIM * sizeof *hidden);
    float *pesi_hid_out = malloc((OUTPUT_DIM * HIDDEN_DIM) * sizeof *pesi_hid_out);
    float *output = malloc(OUTPUT_DIM * sizeof *output);

    if (!input || !pesi_in_hid || !hidden || !pesi_hid_out || !output) {
        fprintf(stderr, "Errore: allocazione memoria fallita.\n");
        free(input); free(pesi_in_hid); free(hidden); free(pesi_hid_out); free(output);
        return 1;
    }

    // 1. Caricamento PNG/JPG e downscaling automatico a 32x32
    if (!caricaImmagine(nomeFile, input)) {
        free(input); free(pesi_in_hid); free(hidden); free(pesi_hid_out); free(output);
        return 1;
    }

    // 2. Inizializzazione pesi casuali in [-1, 1]
    inizializzaPesi(pesi_in_hid, HIDDEN_DIM * INPUT_DIM);
    inizializzaPesi(pesi_hid_out, OUTPUT_DIM * HIDDEN_DIM);

    // 3. Propagazione in avanti (Forward Pass)
    forwardStrato(input, pesi_in_hid, hidden, INPUT_DIM, HIDDEN_DIM);
    forwardStrato(hidden, pesi_hid_out, output, HIDDEN_DIM, OUTPUT_DIM);

    // 4. Decodifica dei 5 neuroni nella lettera corrispondente (a = 00000, b = 00001, ...)
    char codice_binario[OUTPUT_DIM + 1];
    char lettera_predetta = decodificaLettera(output, OUTPUT_DIM, codice_binario);

    printf("\nUscite dei 5 neuroni:\n");
    for (int i = 0; i < OUTPUT_DIM; i++) {
        printf("  Neurone %d: %.4f -> bit: %c\n", i, output[i], codice_binario[i]);
    }
    printf("Codice binario: %s\n", codice_binario);
    if (lettera_predetta != '?') {
        printf("Lettera riconosciuta: '%c'\n", lettera_predetta);
    } else {
        printf("Lettera riconosciuta: non mappata (codice >= 26)\n");
    }

    // 5. Esempio calcolo MSE rispetto a una lettera attesa (es. 'c' -> 00010)
    float target[OUTPUT_DIM];
    letteraInTarget('c', target, OUTPUT_DIM);
    float mse = erroreQuadraticoMedio(target, output, OUTPUT_DIM);
    printf("Errore Quadratico Medio (MSE rispetto a 'c' = 00010): %.4f\n", mse);

    free(input);
    free(pesi_in_hid);
    free(hidden);
    free(pesi_hid_out);
    free(output);

    return 0;
}

// Ridimensiona (downscale o copia 1:1) l'immagine sorgente in una griglia 32x32 normalizzata [0, 1]
void ridimensionaInput(const unsigned char raw[], int larg_src, int alt_src,
                       float dst[], int larg_dst, int alt_dst) {
    for (int r = 0; r < alt_dst; r++) {
        int r_inizio = (int)(((long long)r * alt_src) / alt_dst);
        int r_fine   = (int)(((long long)(r + 1) * alt_src) / alt_dst);
        if (r_fine <= r_inizio) r_fine = r_inizio + 1;
        if (r_fine > alt_src)   r_fine = alt_src;

        for (int c = 0; c < larg_dst; c++) {
            int c_inizio = (int)(((long long)c * larg_src) / larg_dst);
            int c_fine   = (int)(((long long)(c + 1) * larg_src) / larg_dst);
            if (c_fine <= c_inizio) c_fine = c_inizio + 1;
            if (c_fine > larg_src)  c_fine = larg_src;

            unsigned long long somma = 0;
            int conteggio = 0;
            for (int y = r_inizio; y < r_fine; y++) {
                for (int x = c_inizio; x < c_fine; x++) {
                    somma += raw[y * larg_src + x];
                    conteggio++;
                }
            }

            float media = (float)somma / (float)conteggio;
            dst[r * larg_dst + c] = media / 255.0f;
        }
    }
}

// Carica PNG o JPG convertendolo automaticamente in scala di grigi (1 canale)
int caricaImmagine(const char *nomeFile, float input[]) {
    int larghezza = 0, altezza = 0, canali = 0;
    unsigned char *raw = stbi_load(nomeFile, &larghezza, &altezza, &canali, 1);
    if (raw == NULL) {
        fprintf(stderr, "Errore nel caricamento di '%s': %s\n", nomeFile, stbi_failure_reason());
        return 0;
    }

    if (larghezza > COLONNE || altezza > RIGHE) {
        printf("Immagine %dx%d rilevata: downscaling a %dx%d in corso...\n",
               larghezza, altezza, COLONNE, RIGHE);
    } else if (larghezza < COLONNE || altezza < RIGHE) {
        printf("Immagine %dx%d rilevata: ridimensionamento a %dx%d...\n",
               larghezza, altezza, COLONNE, RIGHE);
    } else {
        printf("Immagine %dx%d caricata direttamente.\n", larghezza, altezza);
    }

    ridimensionaInput(raw, larghezza, altezza, input, COLONNE, RIGHE);
    stbi_image_free(raw);
    return 1;
}

// Converte i 5 neuroni nella lettera corrispondente: 00000 -> 'a', 00001 -> 'b', ..., 11001 -> 'z'
char decodificaLettera(const float output[], int num_neuroni, char binario_str[]) {
    int codice = 0;
    for (int i = 0; i < num_neuroni; i++) {
        int bit = (output[i] >= 0.5f) ? 1 : 0;
        codice = (codice << 1) | bit;
        binario_str[i] = bit ? '1' : '0';
    }
    binario_str[num_neuroni] = '\0';

    if (codice >= 0 && codice < 26) {
        return (char)('a' + codice);
    }
    return '?';
}

// Genera il vettore target a 5 bit per una lettera ('a'..'z')
void letteraInTarget(char lettera, float target[], int num_neuroni) {
    char c = (char)tolower((unsigned char)lettera);
    int indice = (c >= 'a' && c <= 'z') ? (c - 'a') : 0;
    for (int i = 0; i < num_neuroni; i++) {
        int shift = num_neuroni - 1 - i;
        target[i] = ((indice >> shift) & 1) ? 1.0f : 0.0f;
    }
}

void inizializzaPesi(float w[], int num_pesi) {
    for (int i = 0; i < num_pesi; i++) {
        w[i] = (float)rand() / RAND_MAX * 2.0f - 1.0f;
    }
}

float sommatoria(const float input[], const float pesi[], int dim) {
    float ris = 0.0f;
    for (int i = 0; i < dim; i++) {
        ris += input[i] * pesi[i];
    }
    return ris;
}

float sigmoide(float sum, float amp, float trasl) {
    return amp * (1.0f / (1.0f + expf(-sum))) + trasl;
}

void forwardStrato(const float input[], const float pesi[], float output[], int dim_in, int dim_out) {
    for (int j = 0; j < dim_out; j++) {
        const float *pesi_neurone = &pesi[j * dim_in];
        float sum = sommatoria(input, pesi_neurone, dim_in);
        output[j] = sigmoide(sum, 1.0f, 0.0f);
    }
}

float erroreQuadroSingolo(float target, float previsione) {
    float errore = target - previsione;
    return errore * errore;
}

float erroreQuadraticoMedio(const float target[], const float previsioni[], int dim) {
    float somma_errori = 0.0f;
    for (int i = 0; i < dim; i++) {
        somma_errori += erroreQuadroSingolo(target[i], previsioni[i]);
    }
    return dim > 0 ? somma_errori / dim : 0.0f;
}
