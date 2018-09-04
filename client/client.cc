#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <iostream>

using namespace std;

#define BUFF_LENGTH 1024

int main(int argc, char* argv[])
{
    int client_sock;
    char buff[BUFF_LENGTH];
    struct sockaddr_in server_addr;

    if (argc < 3) {
        cout << "Need to provide server ip and port" << endl;
        return 0;
    }

    if ((client_sock = socket(AF_INET,SOCK_STREAM,0)) == -1) {
        cout << "ERROR: creating socket " << strerror(errno) << endl;
        return 0;
    }

    memset(&server_addr, 0, sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons(atoi(argv[2]));

    if (connect(client_sock,(struct sockaddr*)&server_addr,sizeof(struct sockaddr_in))) {
        cout << "ERROR: connecting to socket " << strerror(errno) << endl;
        if (close(client_sock)) {
            cout << "ERROR: closing socket " << strerror(errno) << endl;
        }
       return 0;
    }

    memset(buff,0,BUFF_LENGTH);
    sprintf(buff,"Hello");
    if (write(client_sock,buff,strlen(buff)+1) != -1) {
        cout << "Message Sent" << endl;
    } 

    if (close(client_sock)) {
        cout << "ERROR: closing socket " << strerror(errno) << endl;
    }
    cout << "Hello" << endl;    
}
