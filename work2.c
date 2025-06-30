#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

// Función para verificar si un path es un directorio
int is_directory(const char *path) {
    struct stat statbuf;
    if (stat(path, &statbuf) != 0)
        return 0;
    return S_ISDIR(statbuf.st_mode);
}

// Función recursiva para buscar archivos y listar directorios
void search_recursive(const char *dir_path, const char *target_file, int depth) {
    DIR *dir;
    struct dirent *entry;
    char full_path[1024];
    
    // Intentar abrir el directorio
    dir = opendir(dir_path);
    if (dir == NULL) {
        perror("Error al abrir directorio");
        return;
    }
    
    // Crear indentación basada en la profundidad
    char indent[256] = "";
    for (int i = 0; i < depth; i++) {
        strcat(indent, "  ");
    }
    
    printf("%s📁 DIRECTORIO: %s\n", indent, dir_path);
    
    // Leer todas las entradas del directorio
    while ((entry = readdir(dir)) != NULL) {
        // Ignorar "." y ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        
        // Construir la ruta completa
        snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);
        
        if (is_directory(full_path)) {
            // Es un directorio - hacer llamada recursiva
            printf("%s  📂 Subdirectorio encontrado: %s\n", indent, entry->d_name);
            search_recursive(full_path, target_file, depth + 1);
        } else {
            // Es un archivo - listarlo y verificar si coincide con el buscado
            printf("%s  📄 Archivo: %s\n", indent, entry->d_name);
            
            // Verificar si es el archivo que buscamos
            if (target_file != NULL && strcmp(entry->d_name, target_file) == 0) {
                printf("%s  ⭐ ¡ARCHIVO ENCONTRADO!: %s\n", indent, full_path);
                printf("%s     Ruta completa: %s\n", indent, full_path);
            }
        }
    }
    
    closedir(dir);
}

// Función para mostrar ayuda
void show_help(const char *program_name) {
    printf("Uso: %s <directorio_inicio> [archivo_a_buscar]\n", program_name);
    printf("\nDescripción:\n");
    printf("  - Busca recursivamente en todos los subdirectorios\n");
    printf("  - Lista todos los directorios y archivos encontrados\n");
    printf("  - Si se especifica un archivo, lo marca cuando lo encuentra\n");
    printf("\nEjemplos:\n");
    printf("  %s /home/usuario              # Lista todo recursivamente\n", program_name);
    printf("  %s . documento.txt            # Busca documento.txt desde directorio actual\n", program_name);
    printf("  %s /usr/local programa.exe    # Busca programa.exe en /usr/local\n", program_name);
}

int main(int argc, char *argv[]) {
    char *start_directory;
    char *target_file = NULL;
    
    // Verificar argumentos
    if (argc < 2) {
        show_help(argv[0]);
        return 1;
    }
    
    start_directory = argv[1];
    
    // Si se proporciona un segundo argumento, es el archivo a buscar
    if (argc >= 3) {
        target_file = argv[2];
        printf("🔍 Buscando archivo: '%s'\n", target_file);
    } else {
        printf("📋 Listando todos los archivos y directorios\n");
    }
    
    printf("📁 Directorio de inicio: %s\n", start_directory);
    printf("=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=\n");
    
    // Verificar que el directorio de inicio existe
    if (!is_directory(start_directory)) {
        fprintf(stderr, "Error: '%s' no es un directorio válido\n", start_directory);
        return 1;
    }
    
    // Iniciar búsqueda recursiva
    search_recursive(start_directory, target_file, 0);
    
    printf("=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=\n");
    printf("✅ Búsqueda completada.\n");
    
    return 0;
}