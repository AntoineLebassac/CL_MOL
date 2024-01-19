#include <Ethernet.h>

const int numSensors = 8;// Nombre de capteurs
const int LedSensors = 8;//Nombre de sorties LEDS
const int numNotes = 8; //Nombre de notes
const int analogInPins[8] = {A0, A1, A2, A3, A4, A5, A8, A9}; // Broches analogiques des capteurs
const int LedPins[8] = {29, 28, 27, 24, 25, 26, 23, 22}; //Broches de sortie des LEDS
const char* noteNames[8] = {"DO", "RE", "MI", "FA", "SOL", "LA", "SI", "do"}; // Noms des notes
int ancien_canal = 0;
//Flag permettant de ne pas envoyer plusieurs noteON en placant sa main sur les capteurs
int flagDO = 0, flagRE = 0, flagMI = 0, flagFA = 0, flagSOL = 0, flagLA = 0, flagSI = 0, flagdo = 0; 

int tabl[8];

int sensorValue = 0;  // Valeur d'un capteur
byte mac[] = { 0x3E, 0xAF, 0x7D, 0x2B, 0x9C, 0x1F }; // Set une adresse MAC pour l'arduino
IPAddress ip(192, 168, 1, 177);  // Set une adresse IP pour l'arduino
IPAddress server(192, 168, 1, 212); // Set l'adresse IP de l'arduino

int convertirVariable(int valeur) {
  // Conversion linéaire de la plage 520-180 à la plage 60-127
  int nouvelleValeur = int((valeur - 180) * ((127 - 60.0) / (520 - 180))) + 60;
  // Assurer que la nouvelle valeur reste dans la plage 60-127
  nouvelleValeur = constrain(nouvelleValeur, 60, 127);
  return nouvelleValeur;
}

int trouverMax(int tableau[], int taille) {
  // Initialiser la valeur maximale avec la première valeur du tableau
  int maxValue = tableau[0];
  // Parcourir le tableau pour trouver la plus grande valeur
  for (int i = 1; i < taille; i++) {
    if (tableau[i] > maxValue) {
      maxValue = tableau[i];
    }
  }
  // Retourner la plus grande valeur trouvée
  return maxValue;
}

void envoieNote(const char* note) {
  String requete = "GET /midioverlan?ON"; //On commence à écrire la requete HTTP
  if (strcmp(note, "DO") == 0 && flagDO == 0) { //Si on demande de jouer un DO et que l'on est pas en train d'en jouer un : 
    requete += "001"; //On ajoute à la requete le fait de jouer la note DO
    flagDO = 1; //On met le flag à 1, on le repasse à 0 au moment d'arreter la note
  } else if (strcmp(note, "RE") == 0 && flagRE == 0) { //Même chose pour tout les else if 
    requete += "010";
    flagRE = 1;
  } else if (strcmp(note, "MI") == 0 && flagMI == 0) {
    requete += "100";
    flagMI = 1;
  } else if (strcmp(note, "FA") == 0 && flagFA == 0) {
    requete += "300";
    flagFA = 1;
  } else if (strcmp(note, "SOL") == 0 && flagSOL == 0) {
    requete += "030";
    flagSOL = 1;
  } else if (strcmp(note, "LA") == 0 && flagLA == 0) {
    requete += "003";
    flagLA = 1;
  } else if (strcmp(note, "SI") == 0 && flagSI == 0) {
    requete += "007";
    flagSI = 1;
  } else if (strcmp(note, "do") == 0 && flagdo == 0) {
    requete += "900";
    flagdo = 1;
  } else {   //Si aucun flag n'est à 0 : 
    return; //Sortir de la boucle
  }

  EthernetClient client;  //Envoyer la requete http
  if (client.connect(server, 80)) {
    Serial.println("Connected");
    client.print(requete);
    client.print(" HTTP/1.1\r\n");
    client.print("Host: 192.168.1.212\r\n");
    client.print("Connection: close\r\n\r\n");
    client.stop();
    Serial.println("Note sent");
  } else {
    Serial.println("Connection failed");
  }
}

void arretNote(const char* note) {  //Même fonctionnement que envoieNote
  String requete = "GET /midioverlan?ON";
  if (strcmp(note, "DO") == 0 && flagDO == 1) {
    requete += "002";
    flagDO = 0; // Réinitialiser le flag
  } else if (strcmp(note, "RE") == 0 && flagRE == 1) {
    requete += "020";
    flagRE = 0; // Réinitialiser le flag
  } else if (strcmp(note, "MI") == 0 && flagMI == 1) {
    requete += "200";
    flagMI = 0; // Réinitialiser le flag
  } else if (strcmp(note, "FA") == 0 && flagFA == 1) {
    requete += "400";
    flagFA = 0; // Réinitialiser le flag
  } else if (strcmp(note, "SOL") == 0 && flagSOL == 1) {
    requete += "040";
    flagSOL = 0; // Réinitialiser le flag
  } else if (strcmp(note, "LA") == 0 && flagLA == 1) {
    requete += "004";
    flagLA = 0; // Réinitialiser le flag
  } else if (strcmp(note, "SI") == 0 && flagSI == 1) {
    requete += "008";
    flagSI = 0; // Réinitialiser le flag
  } else if (strcmp(note, "do") == 0 && flagdo == 1) {
    requete += "090";
    flagdo = 0; // Réinitialiser le flag
  } else {
    return;
  }
  EthernetClient client;
  if (client.connect(server, 80)) {
    Serial.println("Connected");
    client.print(requete);
    client.print(" HTTP/1.1\r\n");
    client.print("Host: 192.168.1.212\r\n");
    client.print("Connection: close\r\n\r\n");
    client.stop();
    Serial.println("Note stopped");
  } else {
    Serial.println("Connection failed");
  }
}

void Volume(int sensorValue) { //Permet de changer le volume du chanel actuelle le volume est comprise entre 0 et 127
  String requete = "GET /midioverlan?OV";
  int volume = convertirVariable(sensorValue);
  requete += volume; //La requete prend la forme de /midioverlan?OV(volume)

  EthernetClient client;
  if (client.connect(server, 80)) {
    Serial.println("Connected");
    client.print(requete);
    client.print(" HTTP/1.1\r\n");
    client.print("Host: 192.168.1.212\r\n");
    client.print("Connection: close\r\n\r\n");
    client.stop();
    Serial.println("Note sent");
  } else {
    Serial.println("Connection failed");
  }
}

void Canal(int canal) { //permet de changer de canal 
  String requete = "GET /midioverlan?OC";
  requete += canal; //La requete prend la forme de /midioverlan?OV(canal)

  EthernetClient client;
  if (client.connect(server, 80)) {
    Serial.println("Connected");
    client.print(requete);
    client.print(" HTTP/1.1\r\n");
    client.print("Host: 192.168.1.212\r\n");
    client.print("Connection: close\r\n\r\n");
    client.stop();
    Serial.println("Note sent");
  } else {
    Serial.println("Connection failed");
  }
}

void setup() {
  Ethernet.begin(mac, ip);
  Serial.begin(31250);  //On set à 31250 Baud pour gagner en vitesse
  delay(1000);
  for (int pin = 22; pin <= 30; pin++) {  //On définie les broches des LEDS en sortie,
    pinMode(pin, OUTPUT);
  }
  //Ne pas déclarer les capteurs de distance en entrées
  Serial.println("Ethernet setup complete");
}


void loop() {
  for (int i = 0; i < numSensors; i++) {
    int sensorValue = analogRead(analogInPins[i]);  // Lire la valeur du capteur
    // Serial.print("Capteur ");            //Afficher les valeurs des capteurs
    // Serial.print(i + 1);
    // Serial.print(" : ");
    // Serial.print(sensorValue);
    // Serial.print("  ");
    tabl[i] = sensorValue;   //On stockes les 8 valeurs dans un tableau
    if(sensorValue >= 210){
      delay(50);
      sensorValue = analogRead(analogInPins[i]);
      if(sensorValue >= 210){  //Verification anti-bruit
        digitalWrite(LedPins[i], HIGH);   //On allume les leds autour du capteu
        envoieNote(noteNames[i]);         //On demande à jouer la note
        Volume(trouverMax(tabl, 8));      //On regle le volume en fonction de la valeur max se trouvant parmis les 8 valeurs du tableau
        int ValPotent = analogRead(A15);  
        int canal = ValPotent/127;        //Le canal varie entre 0 et 8 en fonction du potentiometre
        if (canal != ancien_canal){       //Si le canal à changé :
          for (int i = 0; i < numSensors; i++) { //On arretes les 8 notes pour éviter de jouer une note constante
          arretNote(noteNames[i]);
          }
        }
      Canal(canal);  //On change le canal
      ancien_canal = canal;
      }
    } else {
      digitalWrite(LedPins[i], LOW);  //Si le capteur ne detecte rien, on éteind la led
      arretNote(noteNames[i]);        //et on arrete de jouer la note qui lui correspond 
    }
  }
}