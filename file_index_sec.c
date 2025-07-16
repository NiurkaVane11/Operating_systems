#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define MAX_RECORDS 100
#define OVERFLOW_FILE "overflow.dat"
#define MAIN_FILE "main.dat"
#define INDEX_FILE "index.dat"

// Estructura del registro
typedef struct
{
    int id;
    char nombre[50];
    int activo;
    int siguiente; // Puntero al siguiente registro en desbordamiento
} Registro;

// Estructura del índice
typedef struct
{
    int id;
    int posicion;
} IndiceEntry;

// Variables globales
int total_registros = 0;
int overflow_count = 0;

// Función para medir tiempo
double medir_tiempo(clock_t inicio, clock_t fin)
{
    return ((double)(fin - inicio) / CLOCKS_PER_SEC) * 1000; // en milisegundos
}

// Insertar en archivo principal
int insertar_principal(Registro reg)
{
    FILE *archivo = fopen(MAIN_FILE, "ab");
    if (!archivo)
        return 0;

    fwrite(&reg, sizeof(Registro), 1, archivo);
    fclose(archivo);

    // Actualizar índice
    FILE *indice = fopen(INDEX_FILE, "ab");
    if (indice)
    {
        IndiceEntry entry = {reg.id, total_registros};
        fwrite(&entry, sizeof(IndiceEntry), 1, indice);
        fclose(indice);
    }

    total_registros++;
    return 1;
}

// Insertar en archivo de desbordamiento
int insertar_desbordamiento(Registro reg)
{
    FILE *archivo = fopen(OVERFLOW_FILE, "ab");
    if (!archivo)
        return 0;

    reg.siguiente = -1; // Marca fin de cadena
    fwrite(&reg, sizeof(Registro), 1, archivo);
    fclose(archivo);

    overflow_count++;
    return 1;
}

// Insertar registro (decide si va a principal o desbordamiento)
double insertar_registro(Registro reg)
{
    clock_t inicio = clock();

    if (total_registros < MAX_RECORDS)
    {
        insertar_principal(reg);
    }
    else
    {
        insertar_desbordamiento(reg);
    }

    clock_t fin = clock();
    return medir_tiempo(inicio, fin);
}

// Buscar en archivo principal
int buscar_principal(int id, Registro *resultado)
{
    FILE *archivo = fopen(MAIN_FILE, "rb");
    if (!archivo)
        return 0;

    Registro reg;
    while (fread(&reg, sizeof(Registro), 1, archivo))
    {
        if (reg.id == id && reg.activo)
        {
            *resultado = reg;
            fclose(archivo);
            return 1;
        }
    }

    fclose(archivo);
    return 0;
}

// Buscar en archivo de desbordamiento
int buscar_desbordamiento(int id, Registro *resultado)
{
    FILE *archivo = fopen(OVERFLOW_FILE, "rb");
    if (!archivo)
        return 0;

    Registro reg;
    while (fread(&reg, sizeof(Registro), 1, archivo))
    {
        if (reg.id == id && reg.activo)
        {
            *resultado = reg;
            fclose(archivo);
            return 1;
        }
    }

    fclose(archivo);
    return 0;
}

// Buscar registro (busca en ambos archivos)
double buscar_registro(int id, Registro *resultado)
{
    clock_t inicio = clock();

    // Primero buscar en archivo principal
    if (buscar_principal(id, resultado))
    {
        clock_t fin = clock();
        return medir_tiempo(inicio, fin);
    }

    // Si no se encuentra, buscar en desbordamiento
    if (buscar_desbordamiento(id, resultado))
    {
        clock_t fin = clock();
        return medir_tiempo(inicio, fin);
    }

    clock_t fin = clock();
    return medir_tiempo(inicio, fin); // No encontrado
}

// Función principal del experimento
int main()
{
    printf("=== EXPERIMENTO: Archivo Secuencial Indexado con Desbordamiento ===\n\n");

    // Limpiar archivos existentes
    remove(MAIN_FILE);
    remove(OVERFLOW_FILE);
    remove(INDEX_FILE);

    double tiempo_insercion_principal = 0;
    double tiempo_insercion_desbordamiento = 0;
    double tiempo_busqueda_principal = 0;
    double tiempo_busqueda_desbordamiento = 0;

    // FASE 1: Llenar archivo principal
    printf("FASE 1: Llenando archivo principal (100 registros)...\n");
    for (int i = 1; i <= MAX_RECORDS; i++)
    {
        Registro reg = {i, "Estudiante", 1, -1};
        sprintf(reg.nombre, "Estudiante_%d", i);

        double tiempo = insertar_registro(reg);
        tiempo_insercion_principal += tiempo;
    }
    tiempo_insercion_principal /= MAX_RECORDS;

    // Medir búsqueda en archivo principal
    printf("Midiendo búsquedas en archivo principal...\n");
    for (int i = 1; i <= 10; i++)
    {
        Registro resultado;
        double tiempo = buscar_registro(i * 10, &resultado);
        tiempo_busqueda_principal += tiempo;
    }
    tiempo_busqueda_principal /= 10;

    // FASE 2: Forzar desbordamiento
    printf("\nFASE 2: Forzando desbordamiento (50 registros adicionales)...\n");
    for (int i = 101; i <= 150; i++)
    {
        Registro reg = {i, "Estudiante", 1, -1};
        sprintf(reg.nombre, "Estudiante_%d", i);

        double tiempo = insertar_registro(reg);
        tiempo_insercion_desbordamiento += tiempo;
    }
    tiempo_insercion_desbordamiento /= 50;

    // Medir búsqueda con desbordamiento
    printf("Midiendo búsquedas con desbordamiento...\n");
    for (int i = 101; i <= 110; i++)
    {
        Registro resultado;
        double tiempo = buscar_registro(i, &resultado);
        tiempo_busqueda_desbordamiento += tiempo;
    }
    tiempo_busqueda_desbordamiento /= 10;

    // RESULTADOS
    printf("\n=== RESULTADOS ===\n");
    printf("Inserción en archivo principal: %.1f ms\n", tiempo_insercion_principal);
    printf("Inserción con desbordamiento: %.1f ms\n", tiempo_insercion_desbordamiento);
    printf("Búsqueda en archivo principal: %.1f ms\n", tiempo_busqueda_principal);
    printf("Búsqueda con desbordamiento: %.1f ms\n", tiempo_busqueda_desbordamiento);

    printf("\nArchivo principal: %d registros\n", total_registros);
    printf("Archivo desbordamiento: %d registros\n", overflow_count);

    return 0;
}