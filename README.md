# Progetto_SO

### Elementi facoltativi
- 1 (Sì) Ad  ogni  accelerazione,  c’è  una  probabilità  di  
10-5  che  l’acceleratore  fallisca.  In  tal  caso,  il  
componente  throttle  control  invia  un  segnale 
alla Central ECU per evidenziare tale evento, e 
la Central ECU avvia la procedura di ARRESTO   
- 2 (Si, anche se non ho ben capito cosa ci fa la ECU con i dati provenienti da questo ma ok) Componente “forward facing radar”   
- 3 (Boh) Quando  si  attiva  l’interazione  con  park  assist,  
la  Central  ECU  sospende  (o  rimuove)  tutti  I  
sensori e attuatori, tranne park assist e 
surround view cameras. 
- 4 (Probabilmente si, aiuta anche per un problemino durante la fase di parcheggio. Non dovrebbe essere così difficile da fare) Il componente Park assist non è generato 
all’avvio  del  Sistema,  ma  creato  dalla  Central  
ECU al bisogno.   
- 5 (Si) Se  il  componente  surround  view  cameras  è  
implementato,  park  assist  trasmette  a  Central  
ECU  anche  i  byte  ricevuti  da  surround  view  
cameras.   
- 6 (Si) Componente “surround view cameras”   
- 7 (No) Il  comando  di  PARCHEGGIO  potrebbe  arrivare  
mentre i vari attuatori stanno eseguendo 
ulteriori comandi (accelerare o sterzare). I vari 
attuatori interrompono le loro azioni, per 
avviare le procedure di parcheggio.   
- 8 (Si, questo non ci vuole veramente nulla a farlo) Se la Central ECU riceve il segnale di fallimento 
accelerazione da “throttle control”, imposta la 
velocità  a  0  e  invia  all’output  della  HMI  un  
messaggio di totale terminazione 
dell’esecuzione 