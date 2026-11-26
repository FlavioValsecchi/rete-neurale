# Rete neurale per immagini

Progetto didattico in C che carica un'immagine, la converte in scala di grigi e la ridimensiona a 32 × 32 pixel. I 1024 valori risultanti vengono passati attraverso una rete neurale con uno strato nascosto e cinque neuroni di uscita, poi interpretati come codice binario di una lettera da `a` a `z`.

Il caricamento delle immagini è fornito da [`stb_image.h`](./stb_image.h), incluso nel progetto. `main.c` definisce `STB_IMAGE_IMPLEMENTATION`, quindi l'implementazione della libreria viene compilata in quel solo file.

## Requisiti

- GCC o un compilatore C compatibile.
- La libreria matematica standard C (`-lm`), usata dalla funzione sigmoide.
- Per compilare o eseguire il debug in VS Code: l'estensione **C/C++** di Microsoft e GDB.
- Per usare il pulsante **Run Code**: l'estensione **Code Runner**.
- Un'immagine supportata da `stb_image` (ad esempio PNG o JPEG).

## Compilazione ed esecuzione da terminale

Dalla cartella del progetto:

```sh
gcc -Wall -Wextra -g -I. main.c -lm -o main
./main
```

Quando richiesto, inserisci il percorso dell'immagine, relativo alla cartella del progetto o assoluto. Ad esempio:

```text
images/lettera.png
```

Il percorso può contenere spazi. L'immagine viene caricata in scala di grigi e ridimensionata a 32 × 32; non è necessario ridimensionarla prima. Il programma segnala un errore se il percorso non è valido o il formato non è supportato.

## Esecuzione in Visual Studio Code

Apri in VS Code la cartella principale del progetto (quella che contiene `main.c` e `stb_image.h`).

- **Run Code:** con l'estensione Code Runner installata, apri `main.c` e premi il pulsante **Run Code**. Le impostazioni del progetto in `.vscode/settings.json` compilano il sorgente con `-I` per trovare `stb_image.h` e `-lm` per `expf`, poi eseguono il programma nel terminale così da poter inserire il percorso dell'immagine.
- **Debug con F5:** scegli **Esegui rete neurale** in **Run and Debug**. La configurazione in `.vscode/launch.json` esegue prima l'attività `build-c`, che genera `${workspaceFolder}/main`, poi avvia GDB.
- **Solo compilazione:** `Ctrl+Shift+B` esegue l'attività predefinita **C/C++: gcc compila il file attivo**.

Se VS Code segnala che manca un compilatore o GDB, installa GCC e GDB nell'ambiente in cui è aperto il progetto. Se `Run Code` non è disponibile, installa l'estensione Code Runner; l'attività di build e la configurazione F5 usano invece l'estensione C/C++.

## Come funziona il programma

Le dimensioni e il numero di neuroni sono definiti in `main.c`:

| Costante | Valore | Significato |
| --- | ---: | --- |
| `RIGHE` | 32 | Altezza della griglia di input |
| `COLONNE` | 32 | Larghezza della griglia di input |
| `INPUT_DIM` | 1024 | Pixel in ingresso (`RIGHE * COLONNE`) |
| `HIDDEN_DIM` | 16 | Neuroni dello strato nascosto |
| `OUTPUT_DIM` | 5 | Bit usati per decodificare una lettera |

Il flusso è il seguente:

1. `caricaImmagine` legge il file con `stbi_load`, chiedendo un solo canale per ottenere la scala di grigi.
2. `ridimensionaInput` porta l'immagine a 32 × 32. Per le immagini più grandi calcola la media dei pixel in ogni area; per quelle più piccole espande le aree corrispondenti. I valori sono normalizzati nell'intervallo `[0, 1]`.
3. `inizializzaPesi` genera pesi casuali tra `-1` e `1` per i collegamenti input-hidden e hidden-output.
4. `forwardStrato` calcola la somma pesata e applica una sigmoide a ciascun neurone, prima per lo strato nascosto e poi per quello di uscita.
5. `decodificaLettera` converte ogni uscita in `0` o `1` usando la soglia `0.5`. I cinque bit rappresentano gli indici da 0 a 31; gli indici da 0 a 25 corrispondono alle lettere `a`–`z`.
6. `erroreQuadraticoMedio` calcola l'MSE rispetto al target fisso della lettera `c` (`00010`).

## Funzioni principali

| Funzione | Descrizione |
| --- | --- |
| `ridimensionaInput` | Ridimensiona e normalizza l'immagine nella griglia usata dalla rete. |
| `caricaImmagine` | Carica l'immagine e gestisce gli errori riportati da `stb_image`. |
| `inizializzaPesi` | Inizializza i pesi con valori pseudo-casuali tra `-1` e `1`. |
| `sommatoria` | Calcola il prodotto scalare tra input e pesi di un neurone. |
| `sigmoide` | Applica la funzione logistica `1 / (1 + expf(-sum))`. |
| `forwardStrato` | Calcola le attivazioni di uno strato completamente connesso. |
| `decodificaLettera` | Converte i cinque bit di uscita in una lettera, oppure `?` se il codice è fuori dall'intervallo `0`–`25`. |
| `letteraInTarget` | Crea il vettore binario associato a una lettera. |
| `erroreQuadroSingolo` / `erroreQuadraticoMedio` | Calcolano l'errore quadratico singolo e la sua media. |

## Limiti

- La rete non viene addestrata: i pesi sono casuali e cambiano a ogni avvio. La lettera stampata è quindi una dimostrazione della pipeline, non un riconoscimento affidabile.
- Il target `c` serve solo per mostrare il calcolo dell'MSE; non modifica i pesi né le uscite.
- L'elaborazione riduce l'immagine a intensità in scala di grigi, senza rilevare o ritagliare automaticamente il carattere.
- L'indice 26–31 non rappresenta una lettera e viene mostrato come non mappato.

