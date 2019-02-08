#ifdef _MSC_VER
#include <windows.h>
#endif

#include <string>
#include <vector>
#include <iostream>

#ifdef _MSC_VER
#include <glib.h>
#include <gio/gio.h>
#else
#include <glib.h>
#include <gio/gio.h>
#endif

using namespace std;

#define LISTEN_PORT	(39217)

int task_should_exit = 0;

#ifdef _MSC_VER
// https://stackoverflow.com/questions/18291284/handle-ctrlc-on-win32
BOOL WINAPI consoleHandler(DWORD dwCtrlType)
{
	switch (dwCtrlType)
	{
	case CTRL_C_EVENT:
		printf("Ctrl-C\n");
		task_should_exit = 1;
		return TRUE;
		break;
	case CTRL_BREAK_EVENT:
		printf("Pause-Break\n");
		return TRUE;
		break;
	case CTRL_CLOSE_EVENT:
		printf("Close\n");
		break;
	default:
		printf("Other case: %d\n", dwCtrlType);
	}

	return FALSE;
}
#endif

int main(int argc, char* argv[])
{
	std::cout << "This is UDP socket sender!!" << std::endl;

	gchar *buffer = "Hi, this is sender.";
	int transfer_len = 0;
	GInetAddress *address = g_inet_address_new_from_string("127.0.0.1");
	//GInetAddress *address = g_inet_address_new_from_string("192.168.0.255");
	GSocketAddress *socket_address = g_inet_socket_address_new(address, LISTEN_PORT);
	GError *error = NULL;

	g_object_unref(address);

	GSocket *socket = g_socket_new(G_SOCKET_FAMILY_IPV4,
		G_SOCKET_TYPE_DATAGRAM,
		G_SOCKET_PROTOCOL_UDP,
		&error);
	g_object_ref(socket);
	g_object_ref(socket_address);
	if (error) {
		g_error(error->message);
	}
	else {
		g_message("Socket creation ok");
	}

#ifdef _MSC_VER
	if (!SetConsoleCtrlHandler(consoleHandler, TRUE)) {
		printf("\nERROR: Could not set control handler");
		return 1;
	}
#endif

	transfer_len = g_socket_send_to(socket, socket_address, buffer, strlen(buffer) + 1, NULL, &error);

	if (error) {
		g_error(error->message);
	}
	else {
		g_message("Socket send ok, send len: %d", transfer_len);
	}

	while (cin) {
		if (task_should_exit) {
			break;
		}

		string in_str;
		cin >> in_str;

		if (in_str.length() <= 1) {
			continue;
		}
		cout << "you entered: [" << in_str << "], len: " << in_str.length() << endl;

		// length should include null character
		transfer_len = g_socket_send_to(socket, socket_address, &in_str[0], in_str.length() + 1, NULL, &error);

		if (error)
			g_error(error->message);
		else {
			g_message("Socket send ok, send len: %d", transfer_len);
		}

	}

	g_object_unref(socket_address);
	g_object_unref(socket);

	std::cout << "end of UDP socket sender!" << std::endl;

	return 0;
}
