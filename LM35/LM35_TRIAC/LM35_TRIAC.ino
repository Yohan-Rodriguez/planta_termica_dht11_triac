/*
  Controlador del tiempo
*/
// Util para realizar el temporizador que testeara cada 100 us el contador 
#include <TimerOne.h>


/* 
  Declaración de variables para el control de la temperatura
*/
float lectura_lm35;
// Temperatura control:
float temp_lm35;
// Temperatura control:
float temp_deseada;
// Temperatura mínima:
float tem_min;
// Temperatura media:
float tem_med;
// Temperatura máxima:
float tem_max;


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
  
  Serial.println("Bienvenido al controlador de temperatura");
  delay(500);
  Serial.println("LM35: Control de temperatura analogo (OpAm)");  
  
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
    if (i>=dim) {
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
  lectura_lm35 = analogRead(A1);
  // Escalar la lectura del potenciometro a valores de voltaje 
  float volts_lm35 = (lectura_lm35 * 5) / 1023.0;
  
  // Obtener temperatuRa en función del valor leido del potenciometro 
  temp_lm35 = volts_lm35 * 100;

  Serial.println(temp_lm35);
}

void loop() {
  // Lectura análoga del valor del potenciometro (ADC) 
  int read_pot = analogRead(A0) ;

  // Escalar la lectura del potenciometro a valores de voltaje 
  float volts = (read_pot * 5) / 1023.0;
  
  // Obtener temperatuRa en función del valor leido del potenciometro 
  temp_deseada = volts * 10;
  Serial.print("Temperatura de control: ");
  Serial.println(temp_deseada);

  // Rango de operación del control de temperatura
  tem_min = temp_deseada - (temp_deseada * 0.1);
  tem_med = temp_deseada;
  tem_max = temp_deseada + (temp_deseada * 0.1);

  // Lectura del valor de temperatura por medio del "LM35"
  Calc_temperature();

  if (temp_lm35 < tem_min) {
    dim = 21;
  }
  
  // temperatura entre a 27 °C y 30.1 °C 
  else if ((temp_lm35 >= tem_min) && (temp_lm35 < tem_med)) {
    dim = 35;
  }

  // temperatura entre a 27 °C y 30.1 °C 
  else if ((temp_lm35 >= tem_med) && (temp_lm35 < tem_max)) {
    dim = 35;
  }

  // temperatura mayor a 30.7 °C 
  else if (temp_lm35 >= tem_max){
    dim = 83;
  }
  delay(300);
}
