% Eliminar conexión del puerto con otros dispositivos
delete(instrfind({'Port'}, {'COM5'}));

% Definir tiempo de muestreo
fs = 1; % Hz

% Duración máxima de la medición (en segundos)
max_time = 600; 
t = linspace(0, max_time, max_time*fs);

% Inicializar vector de temperatura
temp = zeros(1, max_time*fs);

% Crear objeto de comunicación serial
s = serialport('COM5',9600);

% Abrir objeto de comunicación
fopen(s);

% Esperar a que se estabilice el sensor
pause(5);

% Adquirir muestras de temperatura
for i = 1:length(temp)
    % Enviar comando para leer la temperatura y humedad
    fprintf(s, 't')

    % Leer datos del puerto serial enviados por el Arduino
    data = (readline(s));
    
    % Convertir el número binario a decimal
    temperature = sscanf(data, 'Temperatura: %f.%f');
    
    % Mostrar la temperatura
    disp((temperature));

    pause(1/fs);
end

% Eliminar objeto de comunicación
delete(s);

% Graficar temperatura
subplot(2, 1, 1);
plot(t, temp);
xlabel('Tiempo (s)');
ylabel('Temperatura (°C)');
title('Temperatura vs tiempo');

step_time = 5; % tiempo en segundos en que se aplica el escalón
step_size = 10; % tamaño del escalón en grados Celsius
step_samples = step_time*fs + 1; % número de muestras hasta el momento del escalón
temp(step_samples:end) = temp(step_samples:end) + step_size; % aplicar el escalón

% Graficar Escalón voltaje
subplot(2, 1, 2);
plot(t, temp);
xlabel('Tiempo (s)');
ylabel('Temperatura (°C)');
title('Escalón voltaje');
