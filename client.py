import socket

SERVER_NAME = '127.0.0.1'
SERVER_PORT = 12000

username = raw_input('Provide username: ')
client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client.connect((SERVER_NAME, SERVER_PORT))
client.send(username)

while True:
	message = raw_input('Message: ')

	client.send(username + '> ' + message)

	if message == '\\quit':
		print 'Closing connection'
		break

	response = client.recv(1024)
	
	if response == 'Server> \\quit':
		print 'Server closed connection'
		break
	else:
		print response

client.close()


