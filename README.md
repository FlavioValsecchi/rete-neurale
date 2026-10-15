# Rete neurale

Piccolo programma didattico in C che costruisce vettori di valori e pesi, calcola una somma pesata e applica una funzione sigmoide. Il punto di ingresso e l'implementazione principale sono in `main.c`.

## Requisiti

- GCC o un compilatore C compatibile.
- La libreria matematica C, collegata con `-lm`.
- Per compilare e avviare il debug da Visual Studio Code: l'estensione C/C++ e GDB installati nell'ambiente di sviluppo.

## Compilazione ed esecuzione

Dalla cartella del progetto:

```sh
gcc -g main.c -lm -o main
./main
```

Il programma chiede la dimensione dei vettori. Inserisci un intero positivo. Per compilare da Visual Studio Code puoi usare l'attività `build-c`; la configurazione `Compila e Debugga C` compila il progetto e avvia il debugger.

## Flusso del programma

Dato `dim`, il programma esegue questi passaggi:

1. Inizializza il generatore pseudo-casuale usando l'ora corrente.
2. Crea `x`, un vettore di interi con valori alternati `1, 0, 1, 0, ...`.
3. Crea `w`, un vettore di pesi pseudo-casuali compresi tra `-1` e `1`.
4. Stampa `x` e `w` e calcola la somma pesata:

	```text
	s = x[0] * w[0] + x[1] * w[1] + ... + x[dim - 1] * w[dim - 1]
	```

5. Calcola una sigmoide di `s` con ampiezza `5` e traslazione `2`.
6. Riempie `risSigmoidi` applicando a ogni elemento la sigmoide di `s` con ampiezza `1` e traslazione `5`. Stampa il primo elemento del vettore.

I pesi vengono generati con `rand()` e cambiano in genere a ogni esecuzione. Di conseguenza, anche la somma pesata e i risultati della sigmoide possono cambiare.

## Funzioni

| Funzione | Descrizione |
| --- | --- |
| `riempiVettore` | Alterna `1` e `0` nel vettore dei valori e genera i pesi casuali. |
| `stampaVettori` | Stampa i valori e i pesi. |
| `sommatoria` | Restituisce la somma dei prodotti elemento per elemento. |
| `sigmoide` | Calcola `amp * (expf(sum) / (expf(sum) + 1)) + trasl`. |
| `insiemeDiSigmoidi` | Riempie un vettore applicando la sigmoide agli stessi parametri per ogni indice. |

La sigmoide implementata è una logistica scalata e traslata. Per i parametri interi positivi usati in `main`, il suo risultato è compreso tra `trasl` e `trasl + amp`.

## Output

L'output contiene i valori dei due vettori, la somma pesata, il risultato della sigmoide singola e il primo elemento di `risSigmoidi`. I valori numerici sono stampati con due cifre decimali; i pesi e i risultati dipendono dai numeri casuali generati all'avvio.

## Limiti attuali

- L'inserimento da tastiera non viene controllato: il programma presuppone che venga fornito un intero positivo.
- I risultati di `malloc()` non vengono verificati e la memoria allocata non viene liberata prima della chiusura.
- Le dimensioni per i vettori `float` sono calcolate con `sizeof(int)`. È preferibile usare `sizeof(float)` o `sizeof *puntatore` per rendere l'allocazione corretta e portabile.
- `insiemeDiSigmoidi` calcola lo stesso valore a ogni iterazione perché riceve una sola somma; il nome non indica una collezione di somme o di input differenti.
- Per somme molto grandi, `expf(sum)` può superare l'intervallo rappresentabile da `float`.