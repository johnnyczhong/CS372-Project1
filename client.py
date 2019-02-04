import socket


def main():
	
	SERVER_NAME = '127.0.0.1'
	SERVER_PORT = 12000

	username = raw_input('Provide username: ')

	# initial request/handshake
	client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	client.connect((SERVER_NAME, SERVER_PORT))
	client.send(username)

	# communication loop
	while True:
		message = raw_input('Message: ')

		client.send(username + '> ' + message)

		# client requested quit
		if message == '\\quit':
			print 'Closing connection'
			break

		response = client.recv(1024)
		
		# server requested quit
		if response == 'Server> \\quit':
			print 'Server closed connection'
			break
		else:
			print response

	client.close()


if __name__ == '__main__':
	main()