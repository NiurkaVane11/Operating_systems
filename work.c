/*
 * Realice un programa que contenga una función main, variable local, función recursiva.
 * - Imprimir en la función main y en la función recursiva.
 * - Imprimir las direcciones de las variables de 32 bits y 64 bits.
 *
 * @author Vanesa Yupanqui
 * @version 1.0
 * @date 06/13/2025
 */

#include <stdio.h>

// funcion recursiva, que imprime numeros en orden descendente
void imprimirDescendente(int n)
{
    int numero = n + 1;  // variable local
    char caracter = 'N'; // variable local
    if (n == 0)
    {
        return;
    }
    else
    {
        printf("%-3d | %-14p | %-6d | %-14p | %-8c | %-14p\n",
               n, (void *)&n, numero, (void *)&numero, caracter, (void *)&caracter);

        imprimirDescendente(n - 1); // llamada recursiva
    }
}

int main()
{

    printf("Numero descendente   | Direccion  | Numero desendente + 1 | Direccion  | caracter | Direccion \n");
    printf("-------------------------------------------------------------------------------\n");

    imprimirDescendente(5);
    return 0;
}
