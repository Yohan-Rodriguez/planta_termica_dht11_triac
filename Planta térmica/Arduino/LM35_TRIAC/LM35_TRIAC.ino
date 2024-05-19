/*
  LIBRERÍAS
*/
// Incluir la librería PID
#include <PID_v1.h> 

// Realizar el temporizador que testeara cada 100 us el contador 
#include <TimerOne.h> // Temporizador


/*
  VARIABLES PID:

  setpoint (valor de consigna): Es el valor deseado o la referencia que se desea alcanzar o mantener en el sistema controlado. 
  En este sistema de control de temperatura el setpoint será la temperatura objetivo a mantener.

  input (entrada): Es la variable que representa la medición actual del proceso o la planta que se está controlando. 
  input será la temperatura actual medida por el sensor LM35.

  output (salida): Es la variable que representa la señal de control calculada por el controlador PID. output está asociada al 
  ángulo de disparo (dim) del TRIAC, que controla la potencia suministrada a la carga (la lámpara) para ajustar la temperatura.

  Kp, Ki, Kd (constantes del controlador PID): Son los parámetros del controlador PID que determinan cómo responde el controlador 
  a las diferencias entre la entrada y el setpoint.

    Kp (constante proporcional): Controla la contribución proporcional del error actual en la salida del controlador. Ajustar 
    este valor afecta la respuesta del controlador ante errores presentes.
    
    Ki (constante integral): Controla la contribución acumulada de los errores pasados en la salida del controlador. Ajustar este 
    valor afecta la capacidad del controlador para eliminar el error en estado estacionario.
    
    Kd (constante derivativa): Controla la contribución de la tasa de cambio del error en la salida del controlador. Ajustar este 
    valor afecta la capacidad del controlador para prevenir oscilaciones excesivas y mejorar la respuesta transitoria.
    
  myPID (objeto PID): Es el objeto que representa el controlador PID. Se instancia con las direcciones de memoria de input, output 
  y setpoint, junto con los valores de Kp, Ki y Kd. Este objeto encapsula la lógica de cálculo del controlador PID y se encarga de 
  realizar los cálculos necesarios para producir la salida adecuada en función de la entrada y el setpoint.

    DIRECT: En este modo, el controlador aumentará su salida (output) cuando la entrada (input) sea menor que el valor de consigna 
    (setpoint) y disminuirá su salida cuando la entrada sea mayor que el valor de consigna. Es decir, el controlador "actúa 
    directamente" sobre el sistema para llevarlo hacia el valor de consigna. Este es el modo de operación típico para sistemas 
    donde un aumento en la señal de control produce un aumento en la variable controlada.
*/
double setpoint, input, output;
double Kp = 2, Ki = 5, Kd = 1;
PID myPID(&input, &output, &setpoint, Kp, Ki, Kd, DIRECT);


/* 
  VARIABLES PARA EL CONTROL DE LA TEMPERATURA
*/
float temp_lm35;

// Temperatura control:
float temp_deseada;


/* 
  VARIABLES PARA EL CONTROL DEL TRIAC
*/
// Variable usada por el contador
volatile int i=0;       

// Variable que actúa como switch al detectar cruce por cero         
volatile boolean cruce_cero=0;     

// Salida conectada al optoacoplador MOC 3021
int Triac = 3;

// Controla la intensidad de iluminación por medio del ángulo del TRIAC: 0 = MAX-ON || 83 = MIN-OFF
int dim;

// Tiempo en el cual se producen la interrupciones en us.
int T_int = 100;


/*
  CONFIGURACIONES "SET UP"
*/
void setup() {
  // Inicializar comunicación Serial a 9600 budios
  Serial.begin(9600);

  // Configurar el TRIAC como salida.
  pinMode(Triac, OUTPUT);
  
  /*
  "attachInterrupt" realiza una interrupción al detectar el cruce por cero en el pin 2
      - 0 hace referecnia al pin 2 que detecta el cruce por 0
      - Se llama a la fucnión "Deteccion_cruce_cero"
      - "RISING": Si el valor cambia de 0 a 1 se realiza la interrupción
  */
  attachInterrupt(0, Deteccion_cruce_cero, RISING);

  // Inicializa la librería "TimerOne.h" con el tiempo deseado
  Timer1.initialize(T_int);
   
  // En cada interrupción (cada 100 us) se ejecuta el codigo de la función "Dimer"
  Timer1.attachInterrupt(Dimer, T_int);
  
  Serial.println("Bienvenido al controlador de temperatura");
  delay(500);
  Serial.println("LM35: Control de temperatura analogo (OpAm)");  
  // Esperar un segundo para empezar a acceder al sensor LM35
  delay(500);

  /*
    INICIALIZAR PID
  */
  // Valor inicial de la temperatura deseada
  setpoint = 0; 
  myPID.SetMode(AUTOMATIC);
}


/*
  CRUCE X 0 TRUE
*/
void Deteccion_cruce_cero() {
  cruce_cero = true;
  i=0;                                                 
  digitalWrite(Triac, LOW);  
}   


/*
  DIMER: DISPARO DEL TRIAC
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
  CALCULAR TEMPERATURA
*/
void Calc_temperature() {
  int lectura_lm35 = analogRead(A1);
  float volts_lm35 = (lectura_lm35 * 5) / 1023.0;
  temp_lm35 = volts_lm35 * 100;
  Serial.print("Temperatura del LM35: ");
  Serial.println(temp_lm35);
}


/*
  LOOP: MAIN
*/
void loop() {
  // Leer el valor del potenciometro y calcular su equivalente en volts y temperatura
  int read_pot = analogRead(A0);
  float volts = (read_pot * 5) / 1023.0;
  temp_deseada = volts * 10;
  
  // Actualizar valor deseado del PID
  setpoint = temp_deseada;

  Serial.println("==================================================");
  Serial.print("Temperatura de control: ");
  Serial.println(temp_deseada);

  // Leer temperatura actual (del LM35)
  Calc_temperature();
  
  // Establecer entrada del PID como temperatura actual
  input = temp_lm35;

  // Calcular salida del PID
  myPID.Compute();
  
  // Invertir la salida del PID
  dim = 83 - output;

  Serial.print("Ángulo de disparo del TRIAC: ");
  Serial.println(dim);

  delay(300);
}
