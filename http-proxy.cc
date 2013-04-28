/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <cstring>

using namespace std;

char LISTENING_PORT[5] = {'1', '4', '8', '0', '5'};
string welcomeMessage ("Hello and thank you for connecting to our http proxy server!\n");

int main (int argc, char *argv[])
{
	// ALEX: Set up a socket and listen to incoming connection requests
	// If we are dealing with 10 requests currently, reject the connection
	// Otherwise, deal with the connection by opening a port for the client to talk to you on
	// Store everything the client passes to you in a string buffer and pass it to Derek
	
	// ALEX'S CODE HERE
	
	// WHAT MY CODE DOES: Just run it and see. MAKE SURE TO LET THE SERVER SAY "SHUTTING DOWN..."!
	// IT WILL AUTOMATICALLY DO IT AFTER 1-2 SECONDS WHEN YOU CLOSE DOWN THE CONNECTION FROM THE CLIENT SIDE
	// IF YOU DON'T LET THE SERVER SHUT DOWN PROPERLY, IT LEAVES THE PORTS OPEN AND THIS TRIPS UP THE OS
	
	struct addrinfo hints, *res; // initialize structs that we'll be passing into functions
	int sockfd; // initialize a socket for listening

	// first, load up address structs with getaddrinfo():

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

	char * cstr = new char [welcomeMessage.length()+1]; // Set up welcome message string
	strcpy (cstr, welcomeMessage.c_str());
	
	// Accept an incoming connection, open a socket 'newSock' for it
	int newSock;
	
	char incoming[256];
	int bytes_sent;
	
	int i = 0;

	newConn:
	newSock = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size); // Try to establish a connection
	if (newSock != -1) // If we successfully accepted their connection request
	{
		send(newSock, cstr, welcomeMessage.length(), 0);
		printf("We have just successfully established a connection.\n");
		
		i++;

		while (1)
		{
			bytes_sent = recv(newSock, incoming, sizeof(incoming), 0);
			if (bytes_sent > 1)
			{
				printf("The client just told us: %s", incoming);
			
				// DEREK: Take in all strings from the above level
				// Parse it for relevant pieces if it's a GET request
				// Otherwise, return error message mentioned in spec
				// If it's requesting something that is cached
				// Reference the cache and return the proper information
				// else, pass it on to Justin's section
	
				// DEREK'S CODE HERE

				memset(incoming, 0, 256); // Reset incoming for future messages
			}
			if (bytes_sent <= 1) // Our client has disconnected
			{
				printf("Our client has disconnected.\n");
				if  (i >= 3)
				{
					printf("Shutting down http-proxy...\n");
					goto exit;
				}
				printf("Trying to establish new connection...\n");
				goto newConn;
			}
		}
	}
	
	exit:
	shutdown(sockfd, 0); // Shut down port 14805
	// shutdown(newSock, 0); // Shut down port we opened for client

	// JUSTIN: Connect to the server that the client is requesting data from
	// Request the data and cache it
	// Return the data to the client
	// Close the ports that have been opened unless it's non-persistent
	
	// JUSTIN'S CODE HERE
	return 0;
}
