#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
void riempiVettore(int v1[], float v2[], int dim);
void stampaVettori(int v1[], float v2[], int dim);
float sommatoria(int v1[], float v2[], int dim);
float sigmoide(float sum, int amp, int trasl);
void insiemeDiSigmoidi(float v[], float sum, int amp, int trasl, int dim);

int main(){
    srand(time(NULL));
    int dim = 0;

    printf("Inserisci la dimensione del vettore: ");
    scanf("%d", &dim);

    int* x = malloc(sizeof(int)*dim);
    float* w = malloc(sizeof(int)*dim);
    float* risSigmoidi = malloc(sizeof(int)*dim);

    riempiVettore(x, w, dim);
    stampaVettori(x, w, dim);

    double s = sommatoria(x, w, dim);
    printf("Sommatoria: %.2f\n", s);

    double y1 = sigmoide(s, 5, 2);
    printf("Sigmoide singola: %.2f\n", y1);

    insiemeDiSigmoidi(risSigmoidi, s, 1, 5, dim);
    printf("Insieme di sigmoidi popolato (primo elemento: %.2f)\n", risSigmoidi[0]);

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