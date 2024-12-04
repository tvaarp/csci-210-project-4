#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>

struct message {
	char source[50];
	char target[50]; 
	char msg[200]; // message body
};

void terminate(int sig) {
	printf("Exiting....\n");
	fflush(stdout);
	exit(0);
}

int main() {
	int server;
	int target;
	int dummyfd;
	struct message req;
	signal(SIGPIPE,SIG_IGN);
	signal(SIGINT,terminate);
	server = open("serverFIFO",O_RDONLY);
	dummyfd = open("serverFIFO",O_WRONLY);

	while (1) {
		// TODO:
		// read requests from serverFIFO

		if (read(server, &req, size(req)) > 0) {
			// requests
			printf("Received a request from %s to send the message %s to %s.\n",req.source,req.msg,req.target);

			// construct target FIFO name
			char targetFIFO[50];
			snprintf(targetFIFO, sizeof(targetFIFO), "%s", req.target);

			// open target FIFO
			target = open(targetFIFO, O_WRONLY);
			if (target == -1) {
				perror("Error opening target FIFO");
				continue;
			}

			// write the message structure to the target FIFO
			if (write(target, &req, sizeof(req)) == -1) {
				perror("Error writing to target FIFO");
			}
			else {
				printf("Message sent successfully to %s.\n", req.target);
			}

			// close target FIFO
			close(target);
		}
		else {
			perror("Error reading from serverFIFO");
			continue;
		}

		// TODO:
		// open target FIFO and write the whole message struct to the target FIFO
		// close target FIFO after writing the message
	}
	close(server);
	close(dummyfd);
	return 0;
}

