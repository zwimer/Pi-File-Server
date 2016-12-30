# Possible plans

## Client

- Can thread
- One file per command

## Privileges abstract class

- Sub-classes
	- Command
	- User
	- File

## Files

- Size limit
- Limit number of files
- Must record which user is owner

## Server/Client interaction

- Handshake on connect
	- If no handshake, display info to user about nc ip > e.zip; unzip, make 
	- RSA handshake keys if handshake is accepted
- Client's packet to server will contain a thread bool
	- Server will start the command in another thread if it is set
		- Limit number of threads
- Packets must be able to tell the server which thread they are being sent to
- Timeout logout?

## Possible commands

- cmds, fetch, send, login, newUser, create rsa, hide, unhide
- zip, unzip
	- Must affect size used
- Commands which require higher privilege levels
	- chmod, sudo, resetSizeLimit

