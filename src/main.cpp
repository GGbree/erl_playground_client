#include <iostream>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <time.h>
#include <plibsys.h>
#include <psocket.h>
#include "../proto/erl_playground.pb.h"
#define MAX_MESSAGE_LENGTH 258
#define SERVER_PORT 9900

using namespace std;

PSocket* sock;

void wait(unsigned timeout)
{
	timeout += clock();
	while (clock() < timeout)
		continue;
}

int send_create_session(string input_var)
{
	void *fin = nullptr;
	char hack[] = {0x00, 0x00};
	int size = 0;

	envelope *env = new envelope;
	req *rec = new req;
	create_session *ses = new create_session;

	ses->set_allocated_username(&input_var);

	env->set_allocated_uncompressed_data(rec);
	rec->set_type(req_type_enum_create_session);
	rec->set_allocated_create_session_data(ses);

	size = env->ByteSizeLong();
	fin = malloc(size);
	env->SerializeToArray(fin, size);
	hack[1] = size;

	p_socket_send(sock, hack, 2 * sizeof(char), NULL);
	p_socket_send(sock, static_cast<char*>(fin), size, NULL);
	free(fin);
	fin = nullptr;
	return 0;
}

int send_message(string input_var)
{
	void *fin = nullptr;
	char hack[] = { 0x00, 0x00 };
	int size = 0;

	envelope *env = new envelope;
	req *rec = new req;
	server_message *ser = new server_message;

	ser->set_allocated_message(&input_var);

	env->set_allocated_uncompressed_data(rec);
	rec->set_type(req_type_enum_server_message);
	rec->set_allocated_server_message_data(ser);

	size = env->ByteSizeLong();
	fin = malloc(size);
	env->SerializeToArray(fin, size);
	hack[1] = size;

	p_socket_send(sock, hack, 2 * sizeof(char), NULL);
	p_socket_send(sock, static_cast<char*>(fin), size, NULL);
	free(fin);
	fin = nullptr;
	return 0;
}

string parse_message(char *buffer, pssize len)
{
	string fin;
	char* real_buffer = buffer + 2;
	envelope *env = new envelope;
	req *rec = new req;
	server_message *ser = new server_message;

	env->set_allocated_uncompressed_data(rec);
	rec->set_type(req_type_enum_server_message);
	rec->set_allocated_server_message_data(ser);
	ser->set_allocated_message(&fin);
	env->ParseFromArray(real_buffer, len-2);
	
	return fin;
}

int client_loop(string address, string username)
{
	char buffer[MAX_MESSAGE_LENGTH + 1]; // Supports messages up to max message length characters long (plus null character)

	PSocketAddress* addr;
	string message = "init";

	// Construct address for server.
	if ((addr = p_socket_address_new(address.c_str(), SERVER_PORT)) == NULL)
		return -1;

	// Create socket
	if ((sock = p_socket_new(P_SOCKET_FAMILY_INET, P_SOCKET_TYPE_STREAM, P_SOCKET_PROTOCOL_TCP, NULL)) == NULL)
	{
		// Can't construct socket, cleanup and exit.
		p_socket_address_free(addr);

		return -2;
	}


	// Connect to server.
	if (!p_socket_connect(sock, addr, NULL))
	{
		// Couldn't connect, cleanup.
		p_socket_address_free(addr);
		p_socket_free(sock);

		return -3;
	}

	if (send_create_session(username) != 0)
	{
		p_socket_address_free(addr);
		p_socket_free(sock);
		return -4;
	}

	cout << "connection established, type disconnect to leave server" << endl;

	while (1 == 1)
	{
		// Receive our message and print.
		pssize sizeOfRecvData = p_socket_receive(sock, buffer, sizeof(buffer) - 1, NULL);
		buffer[sizeOfRecvData] = '\0'; // Set null character 1 after message
		string result = parse_message(buffer, sizeOfRecvData);

		cout << result << endl;

		getline(cin, message);

		if (message == "disconnect")
		{
			break;
		}

		send_message(message);
		wait(100);

	}

	// Cleanup
	p_socket_address_free(addr);
	p_socket_close(sock, NULL);

	return 0;
}

int main()
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;
	p_libsys_init();

	string username;
	string address;


	cout << "Please type the destination address" << endl;
	getline(cin,address);
	cout << "Please type your username" << endl;
	getline(cin,username);
	

	int runResult;
	runResult = client_loop(address, username);

	p_libsys_shutdown();
	google::protobuf::ShutdownProtobufLibrary();

	return runResult;
}