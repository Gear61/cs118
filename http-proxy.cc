/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include <stdio.h>
#include <iostream>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include "http-request.h"
#include "http-headers.h"
#include "http-response.h"
#include <sys/select.h>
#include <sys/time.h>
#include <string>
#include "cache.h"

using namespace std;

const char* LISTENING_PORT = "14895";
const char* WELCOME_MSG = "This is our proxy server. Thanks for connecting to it!\n";
const char* REJECTION_MSG = "Too many processes. Please try again later.\n";
int sockfd, new_fd;

pid_t connectionsOpen[10];

void signal_handler(int sig)
{
	cout << "SIGINT received. Closing sockets." << endl;
	close(sockfd);
	close(new_fd);
	exit(0);
}

bool doneCheck (char * input, int length)
{
	return ((input[length - 4] == '\r') && (input[length - 3] == '\n') 
		&& (input[length - 2] == '\r') 	&& (input[length - 1] == '\n'));
}

pid_t getOpenSlot()
{
	int i;
	for (i = 0; i < 10; ++i)
	{
		if (connectionsOpen[i] == 0)
		{
			return i;
		}
	}
	return -1;
}

void updateConnections()
{
	int i;
	for (i = 0; i < 10; ++i)
	{
		int status; pid_t pid;
		if (connectionsOpen[i] > 0)
		{
			pid = waitpid(connectionsOpen[i], &status, WNOHANG);
			if (pid)
			{
				connectionsOpen[i] = 0;
			}
		}
	}
}

int main (int argc, char *argv[])
{
	signal(SIGINT, signal_handler);

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
	// Cache myCache;

	while(!terminate)
	{
		updateConnections();

		cout << "Accepting..." << endl;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size);
		// ready to communicate on socket descriptor new_fd!
		if(new_fd != -1)
		{
			pid_t newCon = getOpenSlot();
			if (newCon < 0)
			{
				send(new_fd, REJECTION_MSG, strlen(REJECTION_MSG), 0);
				close(new_fd);
			}
			
			pid_t pid = fork(); // Spawn a new process. Child takes cares of request
			if (pid > 0) // Put child into array of open connections
			{
				connectionsOpen[newCon] = pid;
				continue;
			}
			else if (pid < 0)
			{
				const char* critfail = "Fork failed somehow.";
				cout << critfail << endl;
				send (new_fd, critfail, strlen(critfail), 0);
				close(new_fd);
				close(sockfd);
				exit(1);	
			}
			
			cout << "Received connection." << endl;
			send(new_fd, WELCOME_MSG, strlen(WELCOME_MSG), 0);
			
			while(bytesSent != 0)
			{
				if (offset == 0) // We're not appending to anything. Clear the buffer
				{
					memset(incoming, 0, sizeof(incoming));
				}
				
				bytesSent = recv(new_fd, incoming + offset, sizeof(incoming) - strlen(incoming), 0); // Receive information from client

				if ( (strlen(incoming) >= 4) && (doneCheck(incoming, strlen(incoming))) ) // Alright, they ended with 2 carriage returns. We can parse now
				{
					offset = 0; // We will read into the BEGINNING of the buffer on next iteration
					try
					{
						req.ParseRequest(incoming, strlen(incoming));
						cout << "Legitimate request received." << endl;
						struct addrinfo info1, *info2;
						int outgoing;

						// first, load up address structs with getaddrinfo():

						memset(&info1, 0, sizeof info1);
						info1.ai_family = AF_UNSPEC;  // use IPv4 or IPv6, whichever
						info1.ai_socktype = SOCK_STREAM;
						
						int portNum = (int) req.GetPort();
						char portNum2 [6];
						sprintf(portNum2, "%d", portNum);
						getaddrinfo((req.GetHost()).c_str(), portNum2, &info1, &info2);
						
						// make a socket
						outgoing = socket(info2->ai_family, info2->ai_socktype, info2->ai_protocol);
						
						// make buffer for reading input, buffer for final return
						char response [4096];
						int numChars = 4096;
						char * finalResponse = (char*) malloc(numChars * sizeof(char));
						int index = 0;
						memset(response, 0, sizeof(response));

						string reqMsg = "GET " + req.GetPath() + " HTTP/1.1\r\n\r\n"; 					
	
						int bytes_given = 1;					

						// connect it to the address and port we passed in to getaddrinfo():			
						if (connect(outgoing, info2->ai_addr, info2->ai_addrlen) == 0)
						{
							HttpResponse res2;

							cout << "We have connected!" << endl;
							send(outgoing, reqMsg.c_str(), reqMsg.length(), 0);
							while (1)
							{
								bytes_given = recv(outgoing, response, sizeof(response), 0);
								if(bytes_given == 0)
								{
									cout << "Breaking loose." << endl;
									break;
								}

								while (index + bytes_given > numChars)
								{
									numChars *= 2;
									finalResponse = (char*) realloc ((void*) finalResponse, (numChars * sizeof(char)));
								}
								strcpy(finalResponse + index, response);
								index += bytes_given;

								if(doneCheck(response, strlen(response)))
								{
									res2.ParseResponse(finalResponse, strlen(finalResponse));
									// int contentLength = atoi(res2.FindHeader("Content-Length").c_str());
									memset(response, 0, sizeof(response));
									break;
								}
								memset(response, 0, sizeof(response));
							}
							bytes_given = (recv(outgoing, response, sizeof(response), 0));
							strcpy(finalResponse + index, response);
							index += bytes_given;

							send(new_fd, finalResponse, index, 0);
							free(finalResponse);
						}
						else
						{
							cout << "Connection to " << req.FindHeader("Host") << " failed." << endl;
						}
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
			exit(0);
		}
	}	
	close(sockfd);
 	return 0;
}
