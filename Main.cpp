#include "CapaPresentacion.hpp"
#include "CapaNegocio.hpp"
#include "CapaDatos.hpp"

int main() {
    CapaPresentacion vista;
    CapaNegocio negocio;
    CapaDatos datos;

    int opcion;
    std::string urlActual = ""; // Declarada aqui para que sea global al main

    do {
        opcion = vista.mostrarMenu(); // Ahora mostrarMenu devuelve int

        if (opcion == 1) {

            // Pedir URL
            urlActual = vista.pedirURL();

            // Pedir profundidad
            int prof = vista.pedirProfundidad();

            // Procesar sitio
            vista.mensajeProcesando();

            // Llamar a procesarSitio con la URL y profundidad dadas
            negocio.procesarSitio(urlActual, prof);

            // Calcular métricas
            auto m = negocio.calcularMetricas();

            // Mostrar métricas
            vista.mostrarMetricas(m.totalPaginas, m.totalEnlaces, m.promedioEnlaces, m.paginaMasConectada, m.maxEnlaces);

            // Generar reportes
            datos.generarReportes(urlActual, negocio.getGrafo(), prof, m);

            vista.mensajeExito();
        } 

        // Buscar camino a palabra clave
        else if (opcion == 2) {
            if (urlActual.empty()) {
                std::cout << "[!] Primero analice un sitio." << std::endl;
            } else {
                std::string palabra = vista.pedirPalabra();
                std::vector<std::string> ruta = negocio.buscarRuta(urlActual, palabra);
                vista.mostrarResultadoRuta(ruta);
            }
        }
    } while (opcion != 3);

    return 0;
}