#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/ip.h>  // struct iphdr
#include <netinet/tcp.h> // struct tcphdr
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main()
{
    int sock_raw;
    char buffer[65536]; // tamaño del paquete
    struct sockaddr saddr;
    socklen_t saddr_size = sizeof(saddr);

    // Crear raw socket
    sock_raw = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if (sock_raw < 0)
    {
        perror("Error al crear el socket");
        return 1;
    }

    while (1)
    {
        // Recibir paquete
        int data_size = recvfrom(sock_raw, buffer, sizeof(buffer), 0,
                                 &saddr, &saddr_size);
        if (data_size < 0)
        {
            perror("Error al recibir paquete");
            return 1;
        }

        // Obtener cabecera IP
        struct iphdr *ip = (struct iphdr *)buffer;
        struct tcphdr *tcp = (struct tcphdr *)(buffer + ip->ihl * 4);

        struct in_addr src_addr, dst_addr;
        src_addr.s_addr = ip->saddr;
        dst_addr.s_addr = ip->daddr;

        printf("\n===== Paquete capturado =====\n");

        // Campos IP
        printf("IP Origen:      %s\n", inet_ntoa(src_addr));
        printf("IP Destino:     %s\n", inet_ntoa(dst_addr));
        printf("Versión:        %d\n", ip->version);
        printf("TTL:            %d\n", ip->ttl);
        printf("Protocolo:      %d (6 = TCP)\n", ip->protocol);

        // Verificar que sea TCP
        if (ip->protocol == 6)
        {
            // Campos TCP
            printf("Puerto Origen:  %u\n", ntohs(tcp->source));
            printf("Puerto Destino: %u\n", ntohs(tcp->dest));
            printf("Secuencia:      %u\n", ntohl(tcp->seq));
            printf("ACK:            %u\n", ntohl(tcp->ack_seq));
            printf("Flags:          ");
            if (tcp->syn)
                printf("SYN ");
            if (tcp->ack)
                printf("ACK ");
            if (tcp->fin)
                printf("FIN ");
            if (tcp->rst)
                printf("RST ");
            if (tcp->psh)
                printf("PSH ");
            if (tcp->urg)
                printf("URG ");
            printf("\n");
            printf("Ventana:        %d\n", ntohs(tcp->window));
        }

        printf("=============================\n");
    }

    close(sock_raw);
    return 0;
}
