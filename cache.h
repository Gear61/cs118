#ifndef CACHE
#define CACHE

#include "http-request.h"
#include "http-response.h"


#include <string.h>
#include <string>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <hash_map>

#include <unistd.h>
#include <netinet/in.h>

#include <arpa/inet.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

const char* PORT = "15555";


class Cache
{
public:

	
	int getHttp (const HttpRequest& req) // may need to force port 90 
	{
		const int MAX = 512; // for buf
		char buf[MAX];
		
		struct addrinfo hinst;
		struct addrinfo *serv, *p;
	
		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktupe = SOCK_STREAM; 

		if ( (getaddrinfo(req.GetHost().c_str(), "80", &hints, &serv) ) != 0)
			return -1; //error could not get address


		int sock = socket (serv->ai_family, serv->ai_socktype, 	
					serv->ai_protocol);
		if (sock <0) return -2; // could not get socket

		if (connect(sock, serv->ai_addr, serv->ai_addlen) == -1)
			return -3;  // could not connect

	       // at this point we have a connection dont need linked list
	        freeaddrinfo(serv);
		
		
		string query = "GET " + req.GetPath() + "HTTP/1.1\r\n"
				+ "Host:" + req.GetHost() + "\r\n";
		int size = req.HttpHeaders::GetTotalLength();
		char hdrs[size];
		req.HttpHeaders::FormatHeaders(hdrs);
		string h(hdrs);

		query+= h;
		
		std::cout << query; // see format
	
		int numbytes;		
	}

	int checkCache (const HttpRequest& req, int socket)
	{
		return 0;
	}


	void sendToClient (int socket, const HttpRequest& req)
	{

	}

		


private:
	hash_map<std::string, HttpResponse> responses; /// used keep http responses
							// mainly to check for expiration
					
	

};










http://coding.debuntu.org/c-linux-socket-programming-tcp-simple-http-client

#endif
