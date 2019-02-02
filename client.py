import socket

SERVER_NAME = '127.0.0.1'
SERVER_PORT = 12000

client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client.connect((SERVER_NAME, SERVER_PORT))

while True:
	sentence = raw_input('Input lowercase sentence: ')

	client.send(sentence)

	response = client.recv(1024)
	
	if response == '\\quit':
		print 'Server closed connection'
		break
	else:
		print response

client.close()


