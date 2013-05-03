/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include <iostream>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include "http-request.h"
#include "http-headers.h"
#include "http-response.h"

using namespace std;

const char* LISTENING_PORT = "4649";
const char* WELCOME_MSG = "This is our proxy server. Yoroshiku.\n";
int sockfd, new_fd;

void signal_handler(int sig)
{
	cout << "SIGINT received. Closing sockets." << endl;
	close(sockfd);
	close(new_fd);
	exit(0);
}

bool doneCheck (char * input, int length)
{
	if ((input[length - 4] == '\r') && (input[length - 3] == '\n') && (input[length - 2] == '\r') && (input[length - 1] == '\n'))
	{
		return true;
	}
	return false;
}

int main (int argc, char *argv[])
{
	signal(SIGINT, signal_handler);
	// command line parsing

	struct sockaddr_storage their_addr;
	socklen_t addr_size;
	struct addrinfo hints, *res;

	// first, load up address structs with getaddrinfo():

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;  // use IPv4 or IPv6, whichever
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

	getaddrinfo(NULL, LISTENING_PORT, &hints, &res);

	// make a socket, bind it, and listen on it:

	sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	bind(sockfd, res->ai_addr, res->ai_addrlen);
	listen(sockfd, 10);
	cout << "We are now listening on port " << LISTENING_PORT << ". We await connections." << endl;

	// now accept an incoming connection:

	addr_size = sizeof their_addr;

	char incoming[512];
	int bytesSent = 1;
	bool terminate = 0;
	
	int offset = 0; // We will write to 'incoming' one piece at a time. However, we will still account for all input coming at once

	HttpRequest req;

	while(!terminate)
	{
		cout << "Accepting..." << endl;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size);
		// ready to communicate on socket descriptor new_fd!
		if(new_fd != -1)
		{
			cout << "Received connection." << endl;
			send(new_fd, WELCOME_MSG, strlen(WELCOME_MSG), 0);

			while(bytesSent != 0)
			{
				if (offset == 0) // We're not appending to anything. Clear the buffer
				{
					memset(incoming, 0, sizeof(incoming));
				}
				bytesSent = recv(new_fd, incoming + offset, sizeof(incoming) - strlen(incoming), 0); // Receive information from client
				/* cout << "Received: " << incoming << endl;
				cout << "Number of bytes: " << bytesSent << endl; */
				
				if ( (strlen(incoming) >= 4) && (doneCheck(incoming, strlen(incoming))) ) // Alright, they ended with 2 carriage returns. We can parse now
				{
					offset = 0; // We will read into the BEGINNING of the buffer on next iteration
					try
					{
						req.ParseRequest(incoming, strlen(incoming));
						cout << "They sent us a legitimate request." << endl;
						/* cout << "Host: " << req.GetHost() << endl;
						cout << "Version: " << req.GetVersion() << endl; */
					}
					catch (ParseException& e) // Here, we make an HttpResponse object, format it, and return the string from formatting
					{
						cout << "They sent us a bad request." << endl;
						HttpResponse error;
						error.SetVersion("1.1");
						error.SetStatusMsg(e.what());
						error.SetStatusCode("400");
						char * errorMessage = (char*) malloc(error.GetTotalLength());
						memset(errorMessage, 0, error.GetTotalLength());
						error.FormatResponse(errorMessage);
						send(new_fd, errorMessage, 13 + (strlen(e.what())), 0);
						send(new_fd, "\n\n", 2, 0);
						offset = 0; // Ok, they messed up. Let them start anew
						free(errorMessage);
					}
				}
				else // We shall read in input until we get two carriage returns back to back
				{
					offset += bytesSent;
					continue;
				}
			}
		}
	}
	
	close(sockfd);
 	return 0;
}
