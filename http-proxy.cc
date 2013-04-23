/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>

using namespace std;

int main (int argc, char *argv[])
{
	// ALEX: Set up a socket and listen to incoming connections
	// If we are dealing with 10 requests currently, reject the connection
	// Otherwise, deal with the connection
	// If it's not a GET request, output whatever the spec says
	// If it's a GET request, store it and pass it on
	
	// ALEX'S CODE HERE
	
	// DEREK: Open a new port for the client
	// Take in the request from the above level
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

	return 0;
}
