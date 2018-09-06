#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>

#include <iostream>

using namespace std;

#define MAX_PEND_CONNS 10

bool exit_mainloop;

void signal_hdlr(int signum)
{
    cout << "Received " << signum << " signal" << endl;
    exit_mainloop = true;
}

int main(int argc, char* argv[])
{
    char* buff[1024];

    int ret;
    int server_sock, client_sock;

    int child_pid;
    int to_chld_pipe[2];
    int to_prnt_pipe[2];

    fd_set server_fds, client_fds;
    socklen_t client_addr_size;
    struct sockaddr_in server_addr, client_addr;

    exit_mainloop = false;

    signal(SIGINT,signal_hdlr);

    if (argc < 2) {
        cout << "Need to provide a port to listen on" << endl;
        return 0;
    }

    if ((server_sock = socket(AF_INET,SOCK_STREAM,0)) == -1) {
        cout << "ERROR: creating socket " << strerror(errno) << endl;
        return 0;
    }

    memset(buff,0,1024);
    memset(&server_addr,0,sizeof(struct sockaddr_in));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[1]));	
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(server_sock,(struct sockaddr*)&server_addr,sizeof(struct sockaddr_in))) {
        cout << "ERROR: binding socket " << strerror(errno) << endl;
        if (close(server_sock)) {
            cout << "ERROR: closing socket " << strerror(errno) << endl;
        }
        return 0;
    }

    if (listen(server_sock,MAX_PEND_CONNS)) {
        cout << "ERROR: setting socket to passive mode " << strerror(errno) << endl;
        if (close(server_sock)) {
            cout << "ERROR: closing socket " << strerror(errno) << endl;
        }
        return 0;
    }
    
    FD_ZERO(&client_fds);
    for (;;) {
        FD_ZERO(&server_fds);
        FD_SET(server_sock,&server_fds);

        if ((ret = select(server_sock,&server_fds,NULL,NULL,NULL)) != -1) {
            client_addr_size = sizeof(sockaddr_in);
            if ((client_sock = accept(server_sock,(struct sockaddr*)&client_addr,&client_addr_size)) != -1) {
                cout << "Incoming connection from " << inet_ntoa(client_addr.sin_addr) << endl;
                if (child_pid) {
                    //send new socket fd to child
                } else {
                    if (pipe(to_chld_pipe) != 0) {
                        if (close(client_sock)) {
                            fprintf(stderr,"ERROR: couldn't create comm pipe\n");
                            break;
                        }                        
                    }
                    if (pipe(to_prnt_pipe) != 0) {
                        if (close(to_chld_pipe[0]))
                                fprintf(stderr,"ERROR: failed to close fd\n");
                        if (close(to_chld_pipe[1]))
                                fprintf(stderr,"ERROR: failes to close fd\n");
                        if (close(client_sock)) {
                            fprintf(stderr,"ERROR: couldn't create comm pipe\n");
                            break;
                        }                        
                    }
                    //there is on connections proc yet, let's fork one
                    if ((child_pid = fork()) == -1) {
                        fprintf(stderr, "ERROR: failed to fork connections proc\n");
                        for (int i=0;i<2;i++) {
                            if (close(to_chld_pipe[i]))
                                fprintf(stderr,"ERROR: failed to close fd\n");
                            if (close(to_prnt_pipe[i]))
                                fprintf(stderr,"ERROR: failed to close fd\n");
                        }    
                        if (close(client_sock)) {
                            fprintf(stderr,"ERROR: couldn't fork connections proc\n");
                            break;
                        }
                    } else if (child_pid) {
                        close(to_chld_pipe[0]);
                        close(to_prnt_pipe[1]);

                        //add connects proc to the vector of available ones
                        //send client socket to the forked process
                    } else {
                        //child process
                        close(to_chld_pipe[1]);
                        close(to_prnt_pipe[0]);                        

                        //create connections
                    }
                }
            } else {
                cout << "ERROR: accepting client connection " << strerror(errno) << endl;
            }

            FD_SET(client_sock,&client_fds);
            if (select(client_sock+1,&client_fds,NULL,NULL,NULL) != -1) {
                if (FD_ISSET(client_sock,&client_fds)) {
                    if (read(client_sock,buff,1024) != -1) {
                        cout << "Received " << (char*)buff << endl;
                    }                
                }
            }
            close(client_sock);
        } else {
            if ((errno = EINTR) && exit_mainloop) {
                cout << "Server is shutting down" << endl;
                break;
            }
        }
    }
    if (close(server_sock)) {
        cout << "ERROR: closing socket " << strerror(errno) << endl;
    }
}
