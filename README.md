# Some demo projects and code snippets by L. Santamato

## dmpg.c (Linguaggio C)
Estratto di un complex driver di controllo motore a combustione implementato su famiglia uC Freescale MPC5XXX .

Nel 2008 sono stato incaricato di sviluppare il driver di controllo iniettori multijet per la GEN8 delle ECU controllo motore Magneti Marelli. Le ECU GEN8 montavano come micro principale un Freescale MPC5XXX, architettura 32-bit PowerPC .
I motori multijet prevedono la possibilità di fare iniettate multiple e venni quindi incaricato di specificare ed implementare il driver per la generazione di N comandi logici indipendenti su N pin uC che erano collegati ad altrettanti ingressi di un ASIC che si occupava della generazione del profilo di corrente necessario per comandare l'apertura e la chiusura degli iniettori . Il requisito era che i comandi logici fosser programmabili via SW per generare N treni di M impulsi di durata variabile. 

L'estratto che ho riportato è la parte del driver (MPG sta per multiple pulse generator) di interfacciamento fra il SW applicativo e le periferiche micro che gestiscono la generazione dei comandi di iniezione . Il timer utilizzato è la eTPU, un timer programmabile comandato da una MCU indipendente con un proprio instruction set. Io ho sviluppato anche il micro-codice della eTPU per la programmazione dei compare-match per la generazione dei comandi di iniezione. Per motivi di policy aziendale, non posso pubblicare il microcodice ma sono disponibile per approfondire l'argomento .

Ho condiviso questo estratto perchè l'esperienza è stata davvero istruttiva, ho ancora un po' di incubi quando penso alle sessioni di prove sui rulli o su strada per catturare sporadiche perdite di iniettate e capirne poi il motivo scatenante...awesome! 

In quel periodo e nell'anno successivo ho poi sviluppato e validato altri driver : comunicazioni (SPI / CAN / MSC) , ADC, VRS,... Ho avuto la fortuna di poter lavorare in prima persona con un mio collega sul 90% dei driver GEN8/9 . Poi sono passato ad essere responsabile dell'integrazione , integration testing e rilascio del pacchetto FW verso l'applicativo, staccandomi dallo sviluppo dei componenti. 

Su altre architetture micro ho lavorato  molto di meno come developer e maggiormente come team leader. Ho eseguito il porting del RTOS ERTK, sviluppato dal CRF su architettura PowerPc , verso l'architettura Renesas RH850. Si tratta di un RTOS basato su un semplice scheduler mono-core full-preemptive, con architettura mono-stack ed interfaccia OSEK-compliant . Sempre su RH850 ho sviluppato un driver per il controllo di uno stepper-motor.

# Unity Projects (C#)

Per poter fornire alcuni progetti completi, ho allegato alcune simulazioni che ho sviluppato con l'engine Unity 2018. La versione base si può scaricare online ed i progetti allegati si possono caricare, eseguire nell'engine nonchè generare l'eseguibile Win32/Win64.

### Amazons.zip
La variante (un po' semplificata) degli scacchi chiamata Amazons, le cui regole si trovano a questo link : https://en.wikipedia.org/wiki/Game_of_the_Amazons . 
Ho fatto questo progetto per esplorare gli algoritmi di AI, sia rule-based (agente mini-max) sia data-driven (machine learning/reinforcement learning). La versione attuale supporta l'agente mini-max, nella prossima versione aggiungerò un agente basato su supervised learning per poi passare al deep reinforncement learning.
### SniperBot.zip
Simulazione AI di reinforcement learning. L'agente (il cecchino al centro del campo) con il passare del tempo impara ad uccidere tutti i nemici prima che lo raggiungano. Al momento l'algoritmo impiega 15 min circa per ottenere una performance decente. L'algoritmo è basato su Q-Learning e non fa uso di reti neurali .
### BlockChain.zip 
Soft-Simulation del funzionamento di una blockchain (generazione transizioni , mining, wallet elettronico) . Ho utilizzato le librerie RSA di Microsoft .NET per implementare gli algoritmi di crittografia a chiave asimmetrica.

# Self-Driving Car Engineer Nanodegree Projects (Python / C++)

Ho allegato alcuni progetti (non tutti) che ho sviluppato durante il corso online di 9 mesi con cui ho ottenuto la certificazione di Self-Driving Car Engineer. Sul mio profilo Linkedin trovate l'attestato . I progetti sono sviluppati in Python o C++ ed erano tutti soggetti a review da parte degli istruttori per poter proseguire verso la certificazione. I progetti in Python sono stati creati usando lo Jupyter Notebook, con cui potete aprire i file .ipynb per vedere il sorgente . In più, ogni progetto richiedeva la stesura di un Writeup (PDF) , che potete leggere, dove viene spiegato cosa è stato fatto ed il processo decisionale e di design su cui si basa l'implementazione del progetto.

### Advanced Lane Finding (Python)
Progetto di computer vision, viene richiesto di implementare in Python un algoritmo di Lane Detection.
### Model Predictive Control (Python)
Controllo automatico della vettura su pista sotto determinati vincoli . Disponibile in uno zip a parte il video che mostra in pratica la soluzione implementata.
### Path Planner (Python)
Controllo automatico della vettura che deve essere in grado di guidare su una autostrada a 3 corsie, gestendo i sorpassi, rispettando i limiti di velocità ed evitando collisioni con vetture più lente.
### Unscented Kalman Filter (C++)
Implementazione in C++ di un filtro di Kalman per predire lo stato di un oggetto in movimento (per esempio un robot) in un ambiente circoscritto, sulla base di misure provenienti da sensori tipo lidar/radar (intrinsecamente rumorosi) .

# Altri corsi / progetti in progress

Entro i prossimi 2-3 mesi conto di terminare due corsi online (sempre di carattere teorico+pratico) che sto seguendo nel mio spare time : Full Stack Web Developer (tecnologie sviluppo web Frontend e Backend) e Hadoop Ecosystem (Big Data Management). Entrambi credo siano importanti per chiudere il loop con l'esplorazione del machine learning che sto portando avanti dall'anno scorso .

