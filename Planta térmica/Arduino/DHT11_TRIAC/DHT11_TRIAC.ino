/* 
  DHT11
*/
#include <DHT_U.h>

// Pin digital conectado al sensor DHT11
#define DHTPIN A0    

#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

/*
  Controlador del tiempo
*/
// Util para realizar el temporizador que testeara cada 100 us el contador 
#include <TimerOne.h>     


/* 
  Declaración de variables para el control de la temperatura
*/
int temperature = 0; 

// Temperatura mínima
int tem_min = 18;

// Rango de temperaturas medias
int tem_med_1 = 22;
int tem_med_2 = 24;

// Temperatura máxima
int tem_max_1 = 25;

// Temperatura para apagar por completo el bombillo
int tem_max_2 = 27;
         

/* 
  Declaración de variables para el control del TRIAC
*/
// Variable usada por el contador
volatile int i=0;                 

// variable que actúa como switch al detectar cruce por cero
volatile boolean cruce_cero=0;     

// Salida conectada al optoacoplador MOC 3021
int Triac = 3;

// Controla la intensidad de iluminación, 0 = MAX-ON ; 83 = MIN-OFF                
int dim;

// Tiempo en el cual se producen la interrupciones en us.
int T_int = 100;


/*
  Cofiguraciones "SET UP"
*/
void setup() { 

  // Configurar el TRIAC como salida.
  pinMode(Triac, OUTPUT);

  /*
    "attachInterrupt" realiza una interrupción al detectar el cruce por cero en el pin 2
        - 0 hace referecnia al pin 2 que detecta el cruce por 0
        - Se llama a la fucnión "Deteccion_cruce_cero"
        - "RISING": Si el valor cambia de 0 a 1 se realiza la interrupción
  */
  attachInterrupt(0, Deteccion_cruce_cero, RISING);
  
  //Inicializa la librería "TimerOne.h" con el tiempo deseado
  Timer1.initialize(T_int);
  
  // En cada interrupción (cada 100 us) se ejecuta el codigo de la función "Dimer"
  Timer1.attachInterrupt(Dimer, T_int);

  // inicializar comunicación Serail a 9600 budios
  Serial.begin(9600);
  
  // Inicializar la libreria del sensor DHT11
  dht.begin();
  Serial.println("Welcome to Microdigisoftn");
  delay(500);
  Serial.println("DHT11 Humidity & temperature Sensor");  
  
  // Esperar un segundo para empezar a acceder al sensor DHT11
  delay(1000);
}       


/*
  Funcción que detectar cuando hay un cruce por 0
*/
void Deteccion_cruce_cero() {
  // Si existe un cruce por cero entonces la variable "cruce_cero" cambia a TRUE... 
  //...reseteando el valor de "i", y colocando la salida conectada al Triac en estado "LOW"
  cruce_cero = true;
  i=0;                                                 
  digitalWrite(Triac, LOW);  
}   


/*
  Función para controlar el ángulo de disparo del TRIAC
*/
void Dimer() {                   
  if (cruce_cero == true) {
    if (i>=dim ) {
      digitalWrite(Triac, HIGH);
      i=0;
      cruce_cero=false;
    }

    else {
      i++;
    }
  }
}


/*
  Función para sensar la temperature y humedad del sensor DHT11
*/
void Calc_temperature() {
  temperature = dht.readTemperature();
  // Serial.print("Humidity = ");
  // Serial.print(dht.readHumidity());
  // Serial.println("%  ");
  // Serial.print("Temperature = ");
  Serial.println(temperature);
  // Serial.println("C  ");
  // Serial.println("");  
}



void loop () {

  // Sensar el valor de temperatura y humedad del DHT11
  Calc_temperature();

  // temperatura menor a 27 °C
  if (temperature<tem_min) {
    dim = 21;
  }
  
  // temperatura entre a 27 °C y 30.1 °C 
  else if (temperature>=tem_min && temperature<tem_med_1) {
    dim = 35;
  }

  // temperatura entre 30.1 °C y 30.3 °C 
  else if (temperature>=tem_med_1 && temperature<tem_med_2) {
    dim = 45;
  }

  // temperatura entre 30.3 °C y 30.5 °C 
  else if (temperature>=tem_med_2 && temperature<tem_max_1) {
    dim = 60;
  }

  // temperatura entre 30.5 °C y 30.7 °C 
  else if (temperature>=tem_max_1 && temperature<tem_max_2) {
    dim = 68;
  }

  // temperatura mayor a 30.7 °C 
  else if (temperature>=tem_max_2){
    dim = 83;
  }
}
