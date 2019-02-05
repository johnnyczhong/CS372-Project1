'''
Programmer Name: Johnny Zhong
Program Name: chatserve.py
Program Description: Starts a chat server for communication with a chat client.
Course Name: CS372 - Introduction to Networks
Last Modified: 2019-02-04
Citations: CS372 14th Lecture Notes, Page 9
'''

import argparse
import socket


MSG_SIZE = 1024
SERVER_HANDLE = 'Server> '


def startup_server_socket(port):
	# setting up socket
	server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	server_socket.bind(('', port))
	server_socket.listen(1)

	return server_socket


# 
def receive_msg(connection_socket, client_handle):
	client_msg = connection_socket.recv(MSG_SIZE)

	# client requested quit
	if client_msg == (client_handle + '> \\quit'):
		return False
	else:  # display client message
		print client_msg
		return True


def send_msg(connection_socket):
	# send message
	message = raw_input('Message: ')
	connection_socket.send(SERVER_HANDLE + message)

	# server(self) requested quit
	if message == '\\quit':
		return False
	else:
		return True


def main(port):

	server_socket = startup_server_socket(port)

	# awaiting connection loop
	while True:

		print 'server: Awaiting new connections...'

		# accept new connection
		connection_socket, address = server_socket.accept()
		client_msg = connection_socket.recv(MSG_SIZE)
		print client_msg

		# parse client handle
		client_handle = client_msg.split(': ')[1]

		# communication loop
		while True:
			# receive message from client
			if not receive_msg(connection_socket, client_handle):
				print 'Client closed connection.'
				break

			# send message
			if not send_msg(connection_socket):
				print 'Closing connection.'
				break

		print 'server: Closing connection...'
		connection_socket.close()


if __name__ == '__main__':
	parser = argparse.ArgumentParser()
	parser.add_argument('port', type=int, help='port to listen on for incoming connections')
	args = parser.parse_args()

	main(args.port)
