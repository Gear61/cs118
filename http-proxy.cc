/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <cstring>
#include <signal.h>
#include "http-headers.h"
#include "http-request.h"
#include <sys/time.h>
#include <unistd.h>
#include <sys/poll.h>

using namespace std;

const char * LISTENING_PORT = "14077";
const char * welcomeMessage = "Hello and thank you for connecting to our http proxy server!\n";

static int sockfd; // 14805

bool bail = false;

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
	// int sockfd; // initialize a socket for listening

	// first, load up address structs with getaddrinfo():

	hints.ai_family = AF_UNSPEC;  // use IPv4 or IPv6, whichever
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

	getaddrinfo(NULL, LISTENING_PORT, &hints, &res); // Open port 14805 for listening

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
	
	char incoming[256]; // Buffer for storing incoming messages from client
	int bytes_sent; // Variable to keep track of how many bytes were sent
	
	// Accept an incoming connection, open a socket 'newSock' for it
	int newSock;
	while (!bail)
	{
		newSock = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size); // Try to establish a connection
		if (newSock != -1) // If we successfully accepted their connection request
		{
			send(newSock, welcomeMessage, strlen(welcomeMessage), 0); // Send client a message acknowledging connection
			printf("We have just successfully established a connection.\n");
			
			while (1)
			{	
				bytes_sent = recv(newSock, incoming, sizeof(incoming), 0);
				if (bytes_sent > 1)
				{
					// DEREK: Take in all strings from the above level
					// Parse it for relevant pieces if it's a GET request
					printf("Our client just said: %s\n", incoming);
					HttpRequest req;
					try
					{
						req.ParseRequest(incoming, bytes_sent + 2);
					}
					catch (ParseException& e)
					{
						printf("Parse exception!\n");
						printf("Reason: %s\n", e.what());
					}

					// TODO: Finish tsting
					cout << "Testing host: " << req.GetHost() << endl;
					
					// HEADER LOGIC
					// parse, if exception thrown, return (400)
					// format before outgoing
			
					// Otherwise, return error message mentioned in spec
					// If it's requesting something that is cached
					/* TODO: Implement this.
					CacheObject co = new CacheObject();
					if (co.contains(RequestedThing))
					{ 		
						// Reference the cache and return the proper information
						send(newSock, co.get(RequestedThing), RequestedThing.GetTotalLen(), 0);
					}
					// else, pass it on to Justin's section
					else
					{
						co.add(RequestedThing);
					} */
					// DEREK'S CODE HERE
					
					memset(incoming, 0, 256);
				}
				if (bytes_sent == 2)
				{
					bail = true;
					shutdown(newSock, 0);
					shutdown(sockfd, 0);
					printf("COMMENCING GHETTO SHUTDOWN!\n");
					break;
				}
				if (bytes_sent <= 1)
				{
					shutdown(newSock, 0);
					printf("Our client has disconnected.\n");
					printf("Attempting to establish new connection...\n");
					break;
				}
			}
		}
	}
	
	// JUSTIN: Connect to the server that the client is requesting data from
	// Request the data and cache it
	// Return the data to the client
	// Close the ports that have been opened unless it's non-persistent
	
	// JUSTIN'S CODE HERE
	return 0;
}
