#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>

#define SERVER_IP "127.0.0.1"
#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in server_addr;
    socklen_t addr_len = sizeof(server_addr);

    // Create UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("Invalid address/Address not supported");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("UDP Echo Client is running...\n");

    // Send and receive messages
    while (1) {
        printf("Enter message to send (or type 'exit' to quit): ");
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = 0;  // Remove newline character

        if (strcmp(buffer, "exit") == 0) break;

        // Record the time before sending
        struct timeval start, end;
        gettimeofday(&start, NULL);

        // Send the message to the server
        sendto(sockfd, buffer, strlen(buffer), 0,
               (struct sockaddr *)&server_addr, addr_len);

        // Receive the echoed message from the server
        memset(buffer, 0, BUFFER_SIZE);
        recvfrom(sockfd, buffer, BUFFER_SIZE, 0,
                 (struct sockaddr *)&server_addr, &addr_len);

        // Record the time after receiving
        gettimeofday(&end, NULL);

        // Calculate RTT in milliseconds
        double rtt = (end.tv_sec - start.tv_sec) * 1000.0 +
                     (end.tv_usec - start.tv_usec) / 1000.0;

        printf("Server echoed: %s\n", buffer);
        printf("Round Trip Time (RTT): %.2f ms\n", rtt);
    }

    close(sockfd);
    return 0;
}
