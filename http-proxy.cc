/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>

using namespace std;

char LISTENING_PORT[5] = {'1', '4', '8', '0', '5'};

int main (int argc, char *argv[])
{
	// ALEX: Set up a socket and listen to incoming connection requests
	// If we are dealing with 10 requests currently, reject the connection
	// Otherwise, deal with the connection by opening a port for the client to talk to you on
	// If they send you a GET request, deal with it by passing it down to Derek's level
	// Otherwise, output the error message specified in the spec
	
	// ALEX'S CODE HERE
	
	struct addrinfo hints, *res; // initialize structs that we'll be passing into functions
	int sockfd; // initialize a socket for listening

	//first, load up address structs with getaddrinfo():

	hints.ai_family = AF_UNSPEC;  // use IPv4 or IPv6, whichever
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

	getaddrinfo(NULL, LISTENING_PORT, &hints, &res); // Open port 14805 for victory

	// make a socket

	sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

	// bind it to the port we passed in to getaddrinfo():

	bind(sockfd, res->ai_addr, res->ai_addrlen);
	listen(sockfd, 10); // Listen on port 14805 for incoming connections

	printf("We're now listening on port %s...\n", LISTENING_PORT);

	struct sockaddr_storage their_addr; // Create a struct to hold the receiver information
	socklen_t addr_size; // Initialize a size variable for their IP address
	addr_size = sizeof(their_addr); // Get the size of it
	
	printf("Trying to set up a connection...\n");

	// Accept an incoming connection, open a socket 'newSock' for it
	int newSock;
	while (1)
	{
		newSock = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size);
		if (newSock != -1)
		{
			printf("FUCK YEAH SEAKING! WE HAVE A CONNECTION BITCHES!\n");
			struct timeval tv;
			fd_set readfds; // Set of fds to listen on

			tv.tv_sec = 7;
			tv.tv_usec = 0;

			FD_ZERO(&readfds); // Clear the set
			FD_SET(newSock, &readfds); // Add our socket to the set

			select(newSock+1, &readfds, NULL, NULL, &tv);
		
			char incoming[1024];
			if (FD_ISSET(newSock, &readfds))
			{
				printf("OMFG, THEY DID SOMETHING\n");
				/* int bytes_sent = recv(newSock, incoming, sizeof(incoming), 0);
				if (bytes_sent > 2)
				{
					printf("I love One Piece.\n");
				} */
			}
			else
			{
				printf("They didn't do anything. The fuck?\n");
			}

			// int i = 0;
			// int iResult;
			/* while (1)
			{		
				// iResult = recv(newSock, incoming, sizeof(incoming), 0);
				if (iResult > 1)
				{
					printf("Bytes received: %d\n", iResult);
				}
				if (i < 10)
				{
					send(newSock, "Seaking\n", 8, 0);
					i++;
				}
			} */
		}
	}

	/* char incoming[1024];
	while(1)
	{
		if (!recv(newSock, (void*) incoming, 1024, 0))
		{
			printf("The client just told us: %s\n", incoming);
		}
	} */

	shutdown(sockfd, 0);

	// DEREK: Take in the request from the above level
	// Parse it for relevant pieces
	// If it's requesting something that is cached
	// Reference the cache and return the proper information
	// else, pass it on to Justin's section
	
	// DEREK'S CODE HERE
	
	// JUSTIN: Connect to the server that the client is requesting data from
	// Request the data and cache it
	// Return the data to the client
	// Close the ports that have been opened unless it's non-persistent
	
	// JUSTIN'S CODE HERE
}
