/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>

using namespace std;

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

	getaddrinfo(NULL, "14805", &hints, &res); // Open port 14805 for victory

	// make a socket

	sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

	// bind it to the port we passed in to getaddrinfo():

	bind(sockfd, res->ai_addr, res->ai_addrlen);
	listen(sockfd, 10); // Listen on port 14805 for incoming connections

	struct sockaddr_storage their_addr; // Create a struct to hold the receiver information
	socklen_t addr_size; // Initialize a size variable for their IP address
	addr_size = sizeof(their_addr); // Get the size of it
	
	// Accept an incoming connection, open a socket 'newSock' for it
	int newSock = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size);

	while (1)
	{
		sendto(newSock, "Hello", 5, 0, (struct sockaddr *)&their_addr, addr_size);
	}	

	shutdown(newSock, 0);

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
