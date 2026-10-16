#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

void riempiVettore(int v1[], float v2[], int dim);
void stampaVettori(int v1[], float v2[], int dim);
float sommatoria(int v1[], float v2[], int dim);
float sigmoide(float sum, int amp, int trasl);
void insiemeDiSigmoidi(float v[], float sum, int amp, int trasl, int dim);
float erroreQuadroSingolo(float target, float previsione);
float erroreQuadraticoMedio(float target[], float previsioni[], int dim);

int main(){
    srand(time(NULL));
    int dim = 0;

    printf("Inserisci la dimensione del vettore: ");
    scanf("%d", &dim);

    int* x = malloc(sizeof(int)*dim);
    float* w = malloc(sizeof(float)*dim);         // Corretto: sizeof(float)
    float* risSigmoidi = malloc(sizeof(float)*dim); // Corretto: sizeof(float)

    riempiVettore(x, w, dim);
    stampaVettori(x, w, dim);

    double s = sommatoria(x, w, dim);
    printf("Sommatoria: %.2f\n", s);

    double y1 = sigmoide(s, 5, 2);
    printf("Sigmoide singola: %.2f\n", y1);

    insiemeDiSigmoidi(risSigmoidi, s, 1, 5, dim);
    printf("Insieme di sigmoidi popolato (primo elemento: %.2f)\n", risSigmoidi[0]);

    // Esempio d'uso per testare le nuove funzioni (ipotizzando un vettore di target ideali a 1.0f)
    float* target = malloc(sizeof(float) * dim);
    for(int i = 0; i < dim; i++) target[i] = 1.0f; 

    printf("\n--- Calcolo Errori ---\n");
    float errore_singolo = erroreQuadroSingolo(target[0], risSigmoidi[0]);
    printf("Scarto quadratico singolo (primo elemento): %.4f\n", errore_singolo);

    float mse = erroreQuadraticoMedio(target, risSigmoidi, dim);
    printf("Errore Quadratico Medio (MSE): %.4f\n", mse);

    free(x);
    free(w);
    free(risSigmoidi);
    free(target);

    return 0;
}

void riempiVettore(int v1[], float v2[], int dim){
    for(int i = 0; i < dim; i++){
        if(i % 2 == 0){
            v1[i] = 1;
        } else {
            v1[i] = 0;
        }

        v2[i] = (float)rand() / RAND_MAX * 2.0f - 1.0f;
    }
}

void stampaVettori(int v1[], float v2[], int dim){
    printf("Vettore valori: ");
    for(int i = 0; i < dim; i++){
        printf("[%d] ", v1[i]);
    }
    printf("\nVettore pesi: ");
    for(int j = 0; j < dim; j++){
        printf("[%.2f] ", v2[j]);
    }
    printf("\n");
}

float sommatoria(int v1[], float v2[], int dim){
    float ris = 0.0f;
    for(int i = 0; i < dim; i++){
        ris += v1[i] * v2[i];
    }
    return ris;
}

float sigmoide(float sum, int amp, int trasl){
    float ris = 0.0f;
    ris = amp * (expf(sum) / (expf(sum) + 1.0f)) + trasl;
    return ris;
}

void insiemeDiSigmoidi(float v[], float sum, int amp, int trasl, int dim){
    for (int i = 0; i < dim; ++i){
        v[i] = sigmoide(sum, amp, trasl);
    }
}

float erroreQuadroSingolo(float target, float previsione) {
    float errore = target - previsione;
    return errore * errore;
}

float erroreQuadraticoMedio(float target[], float previsioni[], int dim) {
    float somma_errori = 0.0f;
    for(int i = 0; i < dim; i++) {
        somma_errori += erroreQuadroSingolo(target[i], previsioni[i]);
    }
    return somma_errori / dim;
}