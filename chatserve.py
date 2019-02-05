import socket


def main():
	
	SERVER_PORT = 12000
	MSG_SIZE = 1024
	SERVER_HANDLE = 'Server> '

	# setting up socket
	server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	server_socket.bind(('', SERVER_PORT))
	server_socket.listen(1)

	# awaiting connection loop
	while True:

		print 'server: Awaiting new connections...'

		# accept new connection
		connection_socket, address = server_socket.accept()
		client_handle = connection_socket.recv(MSG_SIZE)

		# communication loop
		while True:
			# receive message from client
			client_msg = connection_socket.recv(MSG_SIZE)

			# client requested quit
			if client_msg == (client_handle + '> \\quit'):
				print 'Client closed connection'
				break
			else:  # display client message
				print client_msg

			# send message
			message = raw_input('Message: ')
			connection_socket.send(SERVER_HANDLE + message)

			# server(self) requested quit
			if message == '\\quit':
				print 'Closing connection'
				break

		print 'server: Closing connection...'
		connection_socket.close()


if __name__ == '__main__':
	main()