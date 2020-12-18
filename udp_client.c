#include "./udp_client.h"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void handle_socket_error(const char *message)
{
	perror(message);
}

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		print_usage(argv[0]);
		return EXIT_FAILURE;
	}

	char* conversion_exception = NULL;
	const int server_port = (int) strtol(argv[2], &conversion_exception, 10);
	if ((*conversion_exception != '\0') || (conversion_exception == argv[2]))
	{
		fprintf(stderr, "Invalid server port specified. Please specify an integer number between 0 and 65535.\n\n");
		print_usage(argv[0]);
		return EXIT_FAILURE;
	}

	int socket_file_descriptor = 0;
	if ((socket_file_descriptor = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
	{
		handle_socket_error("socket");
		return EXIT_FAILURE;
	}

	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons((uint16_t) server_port);
	server_address.sin_addr.s_addr = inet_addr(argv[1]);

	struct sockaddr_in client_address;
	client_address.sin_family = AF_INET;
	client_address.sin_addr.s_addr = htonl(INADDR_ANY);
	client_address.sin_port = htons(SOURCE_PORT);

	if (bind(socket_file_descriptor, (const struct sockaddr *) &client_address, sizeof(client_address)) < 0)
	{
		handle_socket_error("bind");
		return EXIT_FAILURE;
	}
	printf("Client source port: %d\n", ntohs(client_address.sin_port));

	if (connect(socket_file_descriptor, (const struct sockaddr *) &server_address, sizeof(server_address)) < 0)
	{
		handle_socket_error("connect");
		return EXIT_FAILURE;
	}
	printf("Established connected UDP socket.\n");

	const char *message = "It works!";
	for (;;)
	{
		sendto
		(
			socket_file_descriptor,
			message, strlen(message), MSG_CONFIRM,
			(const struct sockaddr *) &server_address, sizeof(struct sockaddr)
		);
		printf("Sent message '%s'\n", message);
		sleep(DELAY_BETWEEN_MESSAGES);
	}

	close(socket_file_descriptor);
	return EXIT_SUCCESS;
}

void print_usage(const char *program_name)
{
	fprintf(stderr, "USAGE: %s server_address server_port\n", program_name);
}
