#include <iostream>
#include <thread>    // Para crear hilos
#include <mutex>     // Para la exclusión mutua
#include <condition_variable> // Para la sincronización
#include <queue>     // Para manejar el buffer como una cola

using namespace std;

const int N = 100;  // Tamaño del buffer
queue<string> buffer;  // Buffer compartido (cola de mensajes)

// Definir mutex y variables de condición
mutex mtx;
condition_variable bufferLleno;
condition_variable bufferVacio;

// Funciones para manipular el buffer
void producir(string &mensaje) {
    cout << "Produciendo mensaje: " << mensaje << endl;
}

void consumir_msg(string &mensaje) {
    cout << "Consumiendo mensaje: " << mensaje << endl;
}

void entrar_msg(string mensaje) {
    buffer.push(mensaje);  // Inserta mensaje en el buffer
    cout << "Mensaje insertado en el buffer: " << mensaje << endl;
}

void remover_msg(string &mensaje) {
    mensaje = buffer.front();  // Extrae el mensaje más antiguo
    buffer.pop();              // Lo elimina del buffer
    cout << "Mensaje removido del buffer: " << mensaje << endl;
}

// Función para el productor
void Productor() {
    string mensaje;
    while (true) {
        mensaje = "Mensaje";  // Aquí puedes generar tu mensaje
        producir(mensaje);    // Produce el mensaje fuera de la sección crítica

        // Protocolo de entrada al buffer
        unique_lock<mutex> lock(mtx);
        bufferVacio.wait(lock, [] { return buffer.size() < N; }); // Espera si el buffer está lleno

        entrar_msg(mensaje);  // Inserta el mensaje en el buffer

        lock.unlock();        // Libera el bloqueo del buffer
        bufferLleno.notify_one();  // Notifica que hay un nuevo mensaje disponible
    }
}

// Función para el consumidor
void Consumidor() {
    string mensaje;
    while (true) {
        unique_lock<mutex> lock(mtx);
        bufferLleno.wait(lock, [] { return !buffer.empty(); });  // Espera si no hay mensajes

        remover_msg(mensaje);  // Extrae el mensaje del buffer

        lock.unlock();         // Libera el bloqueo del buffer
        bufferVacio.notify_one();  // Notifica que hay espacio disponible

        consumir_msg(mensaje);  // Consume el mensaje fuera de la sección crítica
    }
}

int main() {
    // Crear los hilos de productor y consumidor
    thread prod(Productor);
    thread cons(Consumidor);

    // Esperar a que los hilos terminen (en este ejemplo, nunca lo harán)
    prod.join();
    cons.join();

    return 0;
}
