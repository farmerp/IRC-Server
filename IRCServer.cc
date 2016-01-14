
const char * usage =
"                                                               \n"
"IRCServer:                                                   \n"
"                                                               \n"
"Simple server program used to communicate multiple users       \n"
"                                                               \n"
"To use it in one window type:                                  \n"
"                                                               \n"
"   IRCServer <port>                                          \n"
"                                                               \n"
"Where 1024 < port < 65536.                                     \n"
"                                                               \n"
"In another window type:                                        \n"
"                                                               \n"
"   telnet <host> <port>                                        \n"
"                                                               \n"
"where <host> is the name of the machine where talk-server      \n"
"is running. <port> is the port number you used when you run    \n"
"daytime-server.                                                \n"
"                                                               \n";
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#include "IRCServer.h"
#define MAXWORD 100
char word[MAXWORD];
int QueueLength = 5;
char  users[1000][1000];
char  passwords[1000][1000];

struct ListNode {
	const char * value;
	int counter;
	struct ListNode * next;
};
typedef struct ListNode ListNode;

struct LinkedList {
	ListNode * head;
};
typedef struct LinkedList LinkedList;


struct RoomNode {
	char * roomName;
	int messageCounter;
	//int counter;
	LinkedList * members;
	LinkedList * messages;
	struct RoomNode * after;
};
typedef struct RoomNode RoomNode;

struct LinkedRoom {
	RoomNode * header;
};
typedef struct LinkedRoom LinkedRoom;


LinkedRoom * roomList;
void rlist_init(LinkedRoom * list) {
	list -> header = NULL;
}
void llist_init(LinkedList * list) {
	list -> head = NULL;
}
void rlist_create(LinkedRoom * list, const char * roomName) {
	RoomNode * n = (RoomNode *) malloc(sizeof(RoomNode));
	n->roomName = (char *) malloc(sizeof(char));
	strcpy(n->roomName,strdup( roomName));
	n->after = list->header;
	list->header = n;
	n->messageCounter = 0;
	//n->counter = 0;
	n->members = (LinkedList *) malloc(sizeof(LinkedList));
	llist_init(n->members);
	n->messages = (LinkedList *) malloc(sizeof(LinkedList));
	llist_init(n->messages);
}
void llist_add(LinkedList * list, const char * value) {
	ListNode * n = (ListNode *) malloc(sizeof(ListNode));
	n->value =value;
	n->next = list->head;
	list->head = n;
	n->counter = 0;
}

int llist_remove(LinkedList * list, const char * value) {
	ListNode * e;
	ListNode * ln;
	e = list->head;
	ln = e->next;
	if (ln == NULL) {
		if (strcmp(e->value,value) == 0) {
			e->value = NULL;
			return 1;
		} else {
			return 0;
		}
	}
	while(ln != NULL) {
		if(strcmp(ln->value,value) == 0) {
			e->next = ln->next;
			ln->value = NULL;
			return 1;
		}
		e = e->next;
		ln = ln->next;
	}
	return 0;
}

int llist_number_elements(LinkedList * list) {
	ListNode *e;
	int counter = 0;
	e = list->head;
	while(e != NULL) {
		e = e->next;
		counter++;
	}
	return counter;
}
int llist_remove_first(LinkedList*list, const char * value) {
	ListNode * e;
	e = list->head;
	if (e!=NULL) {
		value = e->value;
		list->head = e->next;
		free(e);
		return 1;
	}
	return 0;
}
void llist_print(LinkedList * list, int fd, int difference,int number)  {
	ListNode * e;
	if (list->head == NULL) {
		const char * empty = "NO-NEW-MESSAGES";
		write(fd,empty,strlen(empty));
		return;
	}

	e = list->head;
	int i;
	char * num_st = (char *) malloc(50);
	for (i = 0; i < number; i++) {
	e = e->next;
	}
	for (i = number; i < difference + number; i++) {

		const char * nodeValue = e->value;
		const char * space = " ";
		sprintf(num_st , "%d", i);
		write(fd, num_st, strlen(num_st));
		write(fd,space,strlen(space));
		write(fd,nodeValue,strlen(nodeValue));
		const char * msg1 = "\r\n";
		write(fd,msg1,strlen(msg1));
		e = e->next;

	}
	/*if (difference < 0) {
	  while (e != NULL){

	  const char * nodeValue = e->value;
	  const char * space = " ";
	  sprintf(num_st , "%d", i);
	  write(fd, num_st, strlen(num_st));
	  write(fd,space,strlen(space));
	  write(fd,nodeValue,strlen(nodeValue));
	  const char * msg1 = "\r\n";
	  write(fd,msg1,strlen(msg1));
	  e = e->next;


	  }
	  }*/
}
void llist_insert_last(LinkedList *list, char * value) {
	ListNode * e;
	ListNode * f = (ListNode *) malloc(sizeof(ListNode));
	e = list->head;
	while (e != NULL) {
		if (e->next != NULL) {
			e = e->next;
		}
		else {
			break;
		}
	}
	//strcpy(f->value,strdup(value));
	f->value = strdup(value);
	f->next = NULL;
	if (e != NULL) {
		e->next =f;
	} else {
		list->head = f;
	}
}

int
IRCServer::open_server_socket(int port) {

	// Set the IP address and port for this server
	struct sockaddr_in serverIPAddress; 
	memset( &serverIPAddress, 0, sizeof(serverIPAddress) );
	serverIPAddress.sin_family = AF_INET;
	serverIPAddress.sin_addr.s_addr = INADDR_ANY;
	serverIPAddress.sin_port = htons((u_short) port);

	// Allocate a socket
	int masterSocket =  socket(PF_INET, SOCK_STREAM, 0);
	if ( masterSocket < 0) {
		perror("socket");
		exit( -1 );
	}

	// Set socket options to reuse port. Otherwise we will
	// have to wait about 2 minutes before reusing the sae port number
	int optval = 1; 
	int err = setsockopt(masterSocket, SOL_SOCKET, SO_REUSEADDR, 
			(char *) &optval, sizeof( int ) );

	// Bind the socket to the IP address and port
	int error = bind( masterSocket,
			(struct sockaddr *)&serverIPAddress,
			sizeof(serverIPAddress) );
	if ( error ) {
		perror("bind");
		exit( -1 );
	}

	// Put socket in listening mode and set the 
	// size of the queue of unprocessed connections
	error = listen( masterSocket, QueueLength);
	if ( error ) {
		perror("listen");
		exit( -1 );
	}

	return masterSocket;
}

	void
IRCServer::runServer(int port)
{
	int masterSocket = open_server_socket(port);
	initialize();
	while ( 1 ) {

		// Accept incoming connections
		struct sockaddr_in clientIPAddress;
		int alen = sizeof( clientIPAddress );
		int slaveSocket = accept( masterSocket,
				(struct sockaddr *)&clientIPAddress,
				(socklen_t*)&alen);

		if ( slaveSocket < 0 ) {
			perror( "accept" );
			exit( -1 );
		}

		// Process request.
		processRequest( slaveSocket );		
	}
}

	int
main( int argc, char ** argv )
{
	// Print usage if not enough arguments
	if ( argc < 2 ) {
		fprintf( stderr, "%s", usage );
		exit( -1 );
	}

	// Get the port from the arguments
	int port = atoi( argv[1] );

	IRCServer ircServer;

	// It will never return
	ircServer.runServer(port);

}

/*void
//llist_sort(LinkedList * list, int fd) {
int i;
int j;
int n; 
char * exchange;
n = llist_number_elements(list);
ListNode * listtwo = list->head;
char * temp = (char *) malloc(sizeof(char) * 100);
char* t = (char*)malloc(sizeof(char)*100);
listtwo = list->head;
ListNode* list3 = list->head;
for (i = 0; i < (n - 1); i++) {
strcpy(t,strdup(list3->value));
for (j = i+1; j < n-1; j++) {
if (strcmp(t,strdup(listtwo->next->value))>0){
strcpy(temp ,t);
strcpy(t , strdup(listtwo->next->value));
strcpy(strdup(listtwo->next->value) , temp);
//listtwo = listtwo->next;
}
listtwo = listtwo->next;
}
listtwo = list->head;
list3=list3->next;
//listtwo = list->head;
}
llist_print(list,fd);
return;
}
*/
void
bubbleSortTwo(char * array[1000],int fd) {
	int i;
	int j;
	int k = 0;
	int count;
	int counter = 0;
	while (array[k] != NULL) {
		k++;
	}
	char * exchange;

	for (i = 0; i < (k - 1); i++) {
		for (j = 0; j <(k - i - 1); j++) {
			if(strcmp(array[j],array[j+1]) >  0) {
				exchange = array[j];
				array[j] = array[j+1];
				array[j+1] = exchange;
			}
		}
	}
	while (array[counter] != NULL) {
		const char * returnSpace = "\r\n";
		write(fd,array[counter],strlen(array[counter]));
		write(fd,returnSpace,strlen(returnSpace));
		counter++;
	}
	return;

}
void
bubbleSort(char ** array,int fd) {
	int i;
	int j;
	int k = 0;
	int count;
	int counter = 0;
	while (array[k] != NULL) {
		k++;
	}
	char * exchange;

	for (i = 0; i < (k - 1); i++) {
		for (j = 0; j <(k - i - 1); j++) {
			if(strcmp(array[j],array[j+1]) >  0) {
				exchange = array[j];
				array[j] = array[j+1];
				array[j+1] = exchange;
			}
		}
	}
	while (array[counter] != NULL) {
		const char * returnSpace = "\r\n";
		write(fd,array[counter],strlen(array[counter]));
		write(fd,returnSpace,strlen(returnSpace));
		counter++;
	}
	return;

}
//int compare(const void *a, const void *b) {
//	char *x = (char *) a;
//	char *y = (char *) b;
//	return strcmp(x, y);
//}
//
// Commands:
//   Commands are started y the client.
//
//   Request: ADD-USER <USER> <PASSWD>\r\n
//   Answer: OK\r\n or DENIED\r\n
//
//   REQUEST: GET-ALL-USERS <USER> <PASSWD>\r\n
//   Answer: USER1\r\n
//            USER2\r\n
//            ...
//            \r\n
//
//   REQUEST: CREATE-ROOM <USER> <PASSWD> <ROOM>\r\n
//   Answer: OK\n or DENIED\r\n
//
//   Request: LIST-ROOMS <USER> <PASSWD>\r\n
//   Answer: room1\r\n
//           room2\r\n
//           ...
//           \r\n
//
//   Request: ENTER-ROOM <USER> <PASSWD> <ROOM>\r\n
//   Answer: OK\n or DENIED\r\n
//
//   Request: LEAVE-ROOM <USER> <PASSWD>\r\n
//   Answer: OK\n or DENIED\r\n
//
//   Request: SEND-MESSAGE <USER> <PASSWD> <MESSAGE> <ROOM>\n
//   Answer: OK\n or DENIED\n
//
//   Request: GET-MESSAGES <USER> <PASSWD> <LAST-MESSAGE-NUM> <ROOM>\r\n
//   Answer: MSGNUM1 USER1 MESSAGE1\r\n
//           MSGNUM2 USER2 MESSAGE2\r\n
//           MSGNUM3 USER2 MESSAGE2\r\n
//           ...\r\n
//           \r\n
//
//    REQUEST: GET-USERS-IN-ROOM <USER> <PASSWD> <ROOM>\r\n
//    Answer: USER1\r\n
//            USER2\r\n
//            ...
//            \r\n
//

	void
IRCServer::processRequest( int fd )
{

	// Buffer used to store the comand received from the client
	const int MaxCommandLine = 1024;
	char commandLine[ MaxCommandLine + 1 ];
	int commandLineLength = 0;
	int n;

	// Currently character read
	unsigned char prevChar = 0;
	unsigned char newChar = 0;

	//
	// The client should send COMMAND-LINE\n
	// Read the name of the client character by character until a
	// \n is found.
	//

	// Read character by character until a \n is found or the command string is full.
	while ( commandLineLength < MaxCommandLine &&
			read( fd, &newChar, 1) > 0 ) {

		if (newChar == '\n' && prevChar == '\r') {
			break;
		}

		commandLine[ commandLineLength ] = newChar;
		commandLineLength++;

		prevChar = newChar;
	}

	// Add null character at the end of the string
	// Eliminate last \r
	commandLineLength--;
	commandLine[ commandLineLength ] = 0;
	printf("RECEIVED: %s\n", commandLine);

	//printf("The commandLine has the following format:\n");
	//printf("COMMAND <user> <password> <arguments>. See below.\n");
	//printf("You need to separate the commandLine into those components\n");
	//printf("For now, command, user, and password are hardwired.\n");

	char * command = (char *) malloc(sizeof(char)*1000);
	char * user = (char *) malloc(sizeof(char)*1000);
	char * password = (char *)malloc(sizeof(char)*1000);
	char * args = (char *)malloc(sizeof(char)*1000);
	const char * space = " ";
	int i = 0;
	char * a = command;
	char * b = user;
	char * c = password;
	char * d = args;
	for (; i < commandLineLength; i++) {
		if(commandLine[i] == ' ') {
			break;
		} else {
			*a = commandLine[i];
			a++;
		}
	}
	*a = 0;
	i++;
	for (; i < commandLineLength; i++) {
		if (commandLine[i] == ' ') {
			break;
		} else {
			*b = commandLine[i];
			b++;
		}
	}
	*b = 0;
	i++;
	for (; i < commandLineLength; i++) {
		if (commandLine[i] == ' ') {
			break;
		} else {
			*c = commandLine[i];
			c++;
		}
	}
	*c = 0;
	i++;
	for (; i < commandLineLength; i++) {
		if (commandLine[i] == '\0') {
			break;	
		} else {
			*d = commandLine[i];
			d++;
		}
	}
	*d = 0;

	printf("command=%s\n", command);
	printf("user=%s\n", user);
	printf( "password=%s\n", password );
	printf("args=%s\n", args);

	if (!strcmp(command, "ADD-USER")) {
		addUser(fd, user, password, args);
	}
	else if (!strcmp(command, "LIST-ROOMS")) {
		listRooms(fd,user,password,args);
	}
	else if (!strcmp(command, "ENTER-ROOM")) {
		enterRoom(fd, user, password, args);
	}
	else if (!strcmp(command, "LEAVE-ROOM")) {
		leaveRoom(fd, user, password, args);
	}
	else if (!strcmp(command, "SEND-MESSAGE")) {
		sendMessage(fd, user, password, args);
	}
	else if (!strcmp(command, "GET-MESSAGES")) {
		getMessages(fd, user, password, args);
	}
	else if (!strcmp(command, "GET-USERS-IN-ROOM")) {
		getUsersInRoom(fd, user, password, args);
	}
	else if (!strcmp(command, "CREATE-ROOM")) {
		createRoom(fd, user, password, args);
	}
	else if (!strcmp(command, "GET-ALL-USERS")) {
		getAllUsers(fd, user, password, args);
	}
	else {
		const char * msg =  "UNKNOWN COMMAND\r\n";
		write(fd, msg, strlen(msg));
	}

	// Send OK answer

	//const char * msg =  "OK\n";
	//const char * msg2 = "DENIED\n";
	//if (spaceCounter == 3) {
	//write(fd, msg, strlen(msg));
	//} else {
	//write(fd,msg,strlen(msg2));
	//}

	close(fd);	
}

	void
IRCServer::initialize()
{
	//	users = (char **) malloc(sizeof(char *) * 1000);
	//	passwords = (char **) malloc(sizeof(char *) * 1000);
	FILE * f;

	f = fopen("password.txt","a");
	int i = 0;
	while((fscanf(f,"%s %s ",users[i],passwords[i]))!= -1) {
		i++;
	}


	// Initialize users in room

	roomList = (LinkedRoom *) malloc(sizeof(LinkedRoom));
	rlist_init(roomList);

	// Initalize message list

}


static char * nextWord(FILE * fd) {
	//TAKEN FROM PREVIOUS LAB
	int count;
	int wordLen = 0;
	while ((count = fgetc(fd))!= -1) {
		if ((count <= 'z' && count >= 'a') || (count <= 'Z' && count >= 'A')||(count <= '9' && count > '0')) {
			word[wordLen] = count;
			wordLen = wordLen + 1;
		} else {
			if(wordLen != 0) {
				word[wordLen] = '\0';
				return word;
			}
		}
	}
	if (wordLen != 0) {
		word[wordLen] = '\0';
		return word;
	}
	return NULL;
}

bool
IRCServer::checkPassword(int fd, const char * user, const char * password) {

	FILE *f;
	char * ch;
	char * ch2;
	const char * msg = "OK\r\n";
	const char * msg2 = "ERROR (Wrong password)\r\n";

	f = fopen("password.txt","r");
	if (f == NULL) {
		return false;
	}
	while ((ch = nextWord(f))!= NULL) {
		if (strcmp(ch,user) == 0) {
			ch2 = nextWord(f);
			if (strcmp(ch2,strdup(password))== 0) {
				fclose(f);
				return true;
			} else {
				fclose(f);
				return false;
			}
		}
	} 
	fclose(f);
	return false;

	// Here check the password
}



	void
IRCServer::addUser(int fd, const char * user, const char * password, const char * args)
{
	FILE *f;
	const char * space = " ";
	f = fopen("password.txt","r+");
	char * ch = (char *) malloc(sizeof(char));
	while ((ch = nextWord(f))!= NULL) {
		if(strcmp(ch,strdup(user)) == 0) {
			const char * msg2 = "DENIED\r\n";
			write(fd,msg2,strlen(msg2));
			return;
		} 
	}

	fprintf(f,"%s", user);
	fprintf(f,"%s", space);
	fprintf(f, "%s", password);
	fprintf(f,"%s", space);
	fflush(f);
	const char * msg =  "OK\r\n";
	write(fd, msg, strlen(msg));
	fclose(f);
	return;		
}

void
IRCServer::enterRoom(int fd, const char * user, const char * password, const char * args) {
	RoomNode * n;
	n = roomList->header;
	ListNode * ln;
	if (checkPassword(fd,user,password) == true) {
		while(n != NULL) {
			if (strcmp(n->roomName,args) == 0) {
				ln = n->members->head;
				while(ln != NULL) {
					if (strcmp((ln->value),user) == 0) {
						const char * msg3 = "OK\r\n";
						write(fd,msg3,strlen(msg3));
						return;
					}
					ln = ln->next;
				}
				llist_add(n->members,strdup(user));
				const char * msg = "OK\r\n";
				write(fd,msg,strlen(msg));
				return;
			}
			n = n->after;
		}

	} else {
		const char * msg2 = "ERROR (Wrong password)\r\n";
		write(fd,msg2,strlen(msg2));
		return;
	}
	const char * msg3 = "ERROR (No room)\r\n";
	write (fd,msg3,strlen(msg3));
	return;
}

	void
IRCServer::leaveRoom(int fd, const char * user, const char * password, const char * args)
{
	RoomNode * n;
	n = roomList->header;
	ListNode * e;
	int i = 0;
	if (checkPassword(fd,user,password) == true) {
		while(n != NULL) {
			if (strcmp(n->roomName,strdup(args)) == 0) {
				e = n->members->head;
				while (e != NULL) {
					if (strcmp((e->value),strdup(user)) == 0) { 
						llist_remove(n->members, strdup(user));
						const char * msg = "OK\r\n";
						write (fd,msg,strlen(msg));
						return;
					}
					e = e->next;

					//const char* msg3 = "DENIED\r\n";
					//write (fd,msg3,strlen(msg3));
					//return;
				}
			}
			n = n-> after;

		}
		const char * msg3 = "ERROR (No user in room)\r\n";
		write (fd,msg3,strlen(msg3));
		return;
	} else {
		const char * msg2 = "ERROR (Wrong password)\r\n";
		write(fd,msg2,strlen(msg2));
		return;
	}
	const char * msg3 = "DENIED\r\n";
	write (fd,msg3,strlen(msg3));
	return;
}

	void
IRCServer::sendMessage(int fd, const char * user, const char * password, const char * args)
{

	/*char * msgNum = (char *) malloc(sizeof(char) * 1000);
	  char * c = msgNum;
	  while ( *args != '\0') {
	  if (*args == ' ') {
	  args++;
	  break;
	  }
	 *c++ = *args++;
	 }
	 *c = 0;
	 */
	char * roomNumba = (char *) malloc(sizeof(char) * 1000);
	char * a = roomNumba;
	char * stuff = (char *) malloc(sizeof(char) * 1000);

	while( *args != '\0') {
		if (*args == ' ') {
			args++;
			break;
		}
		*a++=*args++;
	}
	*a =0;
		
	char * userMsg = (char *) malloc(sizeof(char) * 1000);
	//char * a = (char *) malloc(sizeof(char));
	//a = strdup(args);
	//char * g = (char *) malloc(sizeof(char));	
	//char * h = (char *) malloc(sizeof(char));
	//char * b = (char *) malloc(sizeof(char));
	//const char * d;
	//const char * i;
	//const char * j;
	char * b;
	b = userMsg; 
	while (*args != '\0') {
		*b++ =*args++;
	}
	*b = '\0';
	//b = strtok(a, " ");
	//h = strtok(NULL, "\0");
	//stuff = strdup(user);
	//strcat(g, strdup(user));
	//strcat(g, " ");
	//if (h != NULL) {
	//strcat(g, h);
	//}
	
	//i = strtok(g, "xf\335\367\377\177");
	//j = strtok(NULL, "\0");
	
	//d = strdup(g);

	RoomNode * n;
	n = roomList->header;
	ListNode * e;
	if(checkPassword(fd,user,password) == true) {
		while (n != NULL) {
			if (strcmp(n->roomName, roomNumba) == 0) {
				e=n->members->head;
				while(e != NULL) {
					if(strcmp(e->value,strdup(user))==0) {
						//char * stuff = (char*)malloc(sizeof(char)*200);
						strcpy(stuff,user);

						strcat(stuff, " ");
						strcat(stuff, userMsg );
						llist_insert_last(n->messages,stuff);
						/*ListNode *a;
						ListNode * b = (ListNode*) malloc(sizeof(ListNode));
						a = n->messages->head;
						while(a != NULL) {
							if(a->next != NULL) {
								a= a->next;
							}
							break;
						}
						//ListNode * b
						//a->next->value = stuff;
					
						b->value = stuff;
						b->next = NULL;
						if (a != NULL) {
							a->next =b;
						} else {
						n->messages->head = b;
						}
						free(b); */


						const char * msg = "OK\r\n";
						write(fd,msg,strlen(msg));
						(n->messageCounter)++;
						return;
					}
					e = e->next;
				}
			}
			n = n->after;
		}

	} else {
		const char * msg2 = "ERROR (Wrong password)\r\n";
		write(fd,msg2,strlen(msg2));
		return;
	}
	const char * msg3 = "ERROR (user not in room)\r\n";
	write(fd,msg3,strlen(msg3));
	return;
}

	void
IRCServer::getMessages(int fd, const char * user, const char * password, const char * args)
{
	char * msgNum = (char *) malloc(sizeof(char) * 100);
	char * c = msgNum;
	while ( *args != '\0') {
		if (*args == ' ') {
			args++;
			break;
		}

		*c++ = *args++;
	}
	*c = 0;
	char * roomNumba = (char *) malloc(sizeof(char)*100);
	char * a = roomNumba;
	while( *args != '\0') {
		if (*args == '\0') {
			args++;
			break;
		}
		*a++=*args++;
	}
	*a =0;

	int number = atoi(msgNum);
	RoomNode * n;
	n = roomList->header;
	ListNode * e;
	if(checkPassword(fd,user,password) == true) {
		while (n != NULL) {
			if (strcmp(strdup(n->roomName), strdup(msgNum)) == 0) {
				e=n->members->head;
				while(e != NULL) {
					if (strcmp(strdup(e->value),strdup(roomNumba)) == 0) {
						int listnum = llist_number_elements(n->messages);
						int difference = listnum - number;
						//if (number > 100) {
						//	const char * msg4 = "NO-NEW-MESSAGES\r\n";
						//	write(fd,msg4,strlen(msg4));
						//	return;
						//}
						if (n->messageCounter == e->counter) {
							const char * msg5 = "NO-NEW-MESSAGES\r\n";
							write(fd,msg5,strlen(msg5));
							return;
						}
						llist_print(n->messages,fd,difference,number);
						const char * msg = "\r\n";
						write(fd,msg,strlen(msg));
						e->counter = n->messageCounter;
						return;
				}	
					e = e->next;
				}
			}
			n = n->after;
		}

	} else {
		const char * msg2 = "ERROR (Wrong password)\r\n";
		write(fd,msg2,strlen(msg2));
		return;
	}
	const char * msg3 = "ERROR (User not in room)\r\n";
	write(fd,msg3,strlen(msg3));
	return;
}

	void
IRCServer::getUsersInRoom(int fd, const char * user, const char * password, const char * args)
{
	RoomNode * n;
	n = roomList->header;
	char ** array = (char **) malloc(sizeof(char *) * 100); 
	const char * msg3 = "\r\n";
	char ** tempArray = (char **) malloc(sizeof(char *) * 100);
	tempArray = array;
	ListNode * ln;
	int i = 0;
	if(checkPassword(fd,user,password) == true) {
		while( n != NULL) {
			if(strcmp(strdup(n->roomName),strdup(args)) == 0) {
				//	llist_sort(n->members,fd);
				ln = n->members->head;
				while (ln != NULL) {
					*tempArray = strdup(ln->value); 
					tempArray++;
					ln = ln->next;
				}
				bubbleSort(array,fd);		
				const char * msg3 = "\r\n";
				write(fd,msg3,strlen(msg3));
				//const char * msg = "OK\r\n";
				//write(fd,msg,strlen(msg));
				return;

			}
			n = n->after;
		}
	} else {
		const char * msg2 = "ERROR (Wrong password)\r\n";
		write(fd,msg2,strlen(msg2));
		return;
	}
	const char * msg5 = "ERROR (user not in room)\r\n";
	write(fd,msg5,strlen(msg5));
	return;


}

	void
IRCServer::getAllUsers(int fd, const char * user, const char * password,const  char * args)
{
	FILE * f;
	f = fopen("password.txt", "r");
	char * userloadout[1000];
	int i = 0;
	if (checkPassword(fd,user,password) == true) { 
		while(fscanf(f,"%s %s ",users[i],passwords[i]) != EOF) {
			i++;
		}
		int j = 0;
		while (j < i) {
			userloadout[j] = users[j];
			j++;
		}
	} else {
		const char * deny = "ERROR (Wrong password)\r\n";
		write(fd,deny,strlen(deny));
		return;
	}
	int n;
	char ** usersTwo;
	bubbleSortTwo(userloadout,fd);
	const char * returnSpace = "\r\n";
	write(fd,returnSpace,strlen(returnSpace));
	//for(n = 0; n < i; n++) {
	//	char * point;
	//	point = users[n];
	//	strcat(point, "\n");
	//	write(fd,point,strlen(point));
	//}
	return;

}
void
IRCServer::createRoom(int fd, const char * user, const char * password,const char * args) {
	RoomNode * n;
	n=roomList->header;
	ListNode * e;
	if(checkPassword(fd,user,password) == true) {
		while (n !=NULL) {
			if (strcmp(strdup(n->roomName),strdup(args)) == 0) {
				const char * msg3 = "DENIED\r\n";
				write(fd,msg3,strlen(msg3));
				return;
			}
			n = n->after;
		}
	} else {
		const char * msg2 = "ERROR (Wrong password)\r\n";
		write(fd,msg2,strlen(msg2));
		return;
	}
	rlist_create(roomList,strdup(args));
	const char * msg = "OK\r\n";
	write(fd,msg,strlen(msg));
	return;
}
void
IRCServer::listRooms(int fd, const char * user, const char * password, const char * args) {
	RoomNode * n;
	n = roomList->header;
	char ** array = (char **) malloc(sizeof(char *)* 100);
	char ** tempArray = (char **) malloc(sizeof(char *) * 100);
	tempArray = array;
	if (checkPassword(fd,user,password) == true) {
		while(n != NULL) {
			*tempArray = strdup(n->roomName);
			tempArray++;
			n= n->after;
			//char * nombre = (char*) malloc(sizeof(char)*1000);
			//strcpy(nombre,strdup(n->roomName));
			//strcat(nombre,"\n");
			//write(fd,nombre,strlen(nombre));
			//n=n->after
		}
		bubbleSort(array,fd);
		const char * msg = "OK\r\n";
		write(fd,msg,strlen(msg));
		return;
	} else {
		const char * msg2 = "ERROR (Wrong password)\r\n";
		write(fd,msg2,strlen(msg2));
		return;
	}
}
