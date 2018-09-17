#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <cstddef>
#include <iostream>

using namespace std;

#define MAX_MESSAGES 30
#define BUFF_LENGTH 1024

int main(int argc, char* argv[])
{
    int server_sock;
    pid_t client_pid;
    fd_set server_fd;
    char buff[BUFF_LENGTH];
    struct sockaddr_in server_addr;

    if (argc < 3) {
        cout << "Need to provide server ip and port" << endl;
        return 0;
    }

    if ((server_sock = socket(AF_INET,SOCK_STREAM,0)) == -1) {
        cout << "ERROR: creating socket " << strerror(errno) << endl;
        return 0;
    }

    memset(&server_addr, 0, sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons(atoi(argv[2]));

    if (connect(server_sock,(struct sockaddr*)&server_addr,sizeof(struct sockaddr_in))) {
        cout << "ERROR: connecting to socket " << strerror(errno) << endl;
        if (close(server_sock)) {
            cout << "ERROR: closing socket " << strerror(errno) << endl;
        }
       return 0;
    }

    client_pid = getpid();
        
    for (int i=0; i<MAX_MESSAGES; i++) {
        memset(buff,0,BUFF_LENGTH);
        sprintf(buff,"Client: %ld",client_pid);
        sleep(1);
        if (write(server_sock,buff,strlen(buff)+1) != -1) {
            cout << "Message Sent" << endl;
            memset(buff,0,BUFF_LENGTH);
            FD_ZERO(&server_fd);
            FD_SET(server_sock,&server_fd);
            if (select(server_sock+1,&server_fd,NULL,NULL,NULL) != -1) {
                if (read(server_sock,buff,BUFF_LENGTH) != -1) {
                    cout << "Reply from server: " << buff << endl;    
                }
            } 
        } 
    }
    if (close(server_sock)) {
        cout << "ERROR: closing socket " << strerror(errno) << endl;
    }
}
