#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include "json.hpp"
#include "base64.h"
#include <iostream>
#include <fstream>
#define BUFSIZE 1450
#define MYPORT 8866
#define MAXNAME 100
using namespace std;

int checkNUM = 0;
char datareceived[BUFSIZE];
int sd, numbytes;
struct sockaddr_in
	serveraddress,
	cliaddr;
socklen_t length;

string inf;

void save(nlohmann::json j)
{
	if (j["part"] >= 1)
	{
		inf += j["str"];
	}
	if (j["part"] == 1)
	{
		string bse = base64_decode(inf);
		vector<unsigned char> data(bse.begin(), bse.end());

		ofstream fout("test.png", ios_base::app | ios_base::binary | ios_base::out);
		fout.write((char *)data.data(), data.size());
	}
}

void Warranty_sending(char *packet)
{
	nlohmann::json j;
	j = nlohmann::json::parse(packet);
	if (j["part"] == 2)
	{
		printf("ok");
	}
	int i = j["part"];

	int bites;
	if (i != checkNUM)

		while (i != checkNUM)
		{
			cout << i << endl;
			save(j);

			checkNUM = i;
			string data = "OK";
			int bytessent = sendto(sd, data.c_str(), numbytes, 0, (struct sockaddr *)&cliaddr, length);
			if (0 > bytessent)
			{
				perror("Error while sending:");
				exit(1);
			}
			// cout << "Server Receives: " << (*str) << endl;
		}
	else
		bites = sendto(sd, datareceived, numbytes, 0, (struct sockaddr *)&cliaddr, length);
}

int main(int argc, char *argv[])

{
	int bytessent, ret;
	char clientname[MAXNAME], datareceived[BUFSIZE];

	sd = socket(AF_INET, SOCK_DGRAM, 0);
	if (0 > sd)
	{
		perror("socket");
		exit(1);
	}

	memset(&serveraddress, 0, sizeof(serveraddress));
	memset(&cliaddr, 0, sizeof(cliaddr));

	serveraddress.sin_family = AF_INET;
	serveraddress.sin_port = htons(MYPORT);			   // PORT NO
	serveraddress.sin_addr.s_addr = htonl(INADDR_ANY); // IP ADDRESS
	ret = bind(sd, (struct sockaddr *)&serveraddress, sizeof(serveraddress));
	if (0 > ret)
	{
		perror("Bind Fails:");
		exit(1);
	}
	for (;;)
	{
		printf("UDP Server:  Waiting for client data\n");
		length = sizeof(cliaddr);
		/*Received a datagram*/
		numbytes = recvfrom(sd, datareceived, BUFSIZE, 0,
							(struct sockaddr *)&cliaddr, &length);
		if (0 > numbytes)
		{
			perror("Error while receiving:");
			exit(1);
		}
		/*Printing client's address*/
		printf("Data Received from %s\n", inet_ntop(AF_INET, &cliaddr.sin_addr, clientname, sizeof(clientname)));

		/*Sending the Received datagram back*/
		datareceived[numbytes] = '\0';
		string data;
		checkNUM = 0;
		Warranty_sending(datareceived);
	}
}
