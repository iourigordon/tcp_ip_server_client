#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>


#include <map>
#include <vector>
#include <sstream>
#include <iostream>
#include <iomanip>

using namespace std;

#include "connections.h"
#include "ctrl_msg_fact.h"
#include "ctrl_message.h"
#include "connections_ctrl.h"

#define MAX_PEND_CONNS 10
#define MAX_PROCS 2
#define PRNT_COMM_SOCK 0
#define CHLD_COMM_SOCK 1

bool exit_mainloop;

void signal_hdlr(int signum)
{
    cout << "Received " << signum << " signal" << endl;
    exit_mainloop = true;
}

std::string printable(char c)
{
    std::ostringstream oss;
    if (isprint(c))
        oss << c;
    else
        oss << "\\x" << std::hex << std::setw(2) << std::setfill('0')
            << (int)(uint8_t)c << std::dec;
    return oss.str();
}

std::string printable(const std::string& s)
{
    std::string result;
    for (std::string::const_iterator i = s.begin(); i != s.end(); ++i)
        result += printable(*i);
    return result;
}

int main(int argc, char* argv[])
{
    int ret;
    int server_sock, client_sock;

    int child_pid;
    int to_chld_pipe[2];
    int to_prnt_pipe[2];
    int comm_socket[2];

    fd_set server_fds;
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
    
    for (;;) {
        FD_ZERO(&server_fds);
        FD_SET(server_sock,&server_fds);

        if ((ret = select(server_sock+1,&server_fds,NULL,NULL,NULL)) != -1) {
            client_addr_size = sizeof(sockaddr_in);
            if ((client_sock = accept(server_sock,(struct sockaddr*)&client_addr,&client_addr_size)) != -1) {
                cout << "Incoming connection from " << inet_ntoa(client_addr.sin_addr) <<  " on Sock " << client_sock  << endl;
                connections_ctrl* ctrl = connections_ctrl::get_conn_ctrl(MAX_PROCS);
                switch (ctrl->add_client(client_sock,inet_ntoa(client_addr.sin_addr))) {
                    case SUCCESS:
                        cout << "Client was added to server connections" << endl;
                        break;
                    case CREATE_CONN:
                        if (connections_ctrl::create_connection(comm_socket)) {
                            fprintf(stderr,"ERROR: couldn't create comm pipe\n");
                            break;
                        }                        
                        
                        if ((child_pid = fork()) == -1) {
                            fprintf(stderr, "ERROR: failed to fork connections proc\n");
                            for (int i=0;i<2;i++) {
                                if (close(comm_socket[i]))
                                    fprintf(stderr,"ERROR: failed to close fd\n");
                            }    
                            if (close(client_sock)) {
                                fprintf(stderr,"ERROR: couldn't fork connections proc\n");
                                break;
                            }
                        } else if (child_pid) {
                            close(comm_socket[CHLD_COMM_SOCK]);

                            ctrl->add_new_proc(child_pid,comm_socket[PRNT_COMM_SOCK]); 
                            ctrl->add_client(client_sock,inet_ntoa(client_addr.sin_addr));
                        } else {
                            //child process
                            close(comm_socket[PRNT_COMM_SOCK]);

                            connections *client_connections = connections::get_connection(comm_socket[CHLD_COMM_SOCK]);
                            client_connections->run();
                            connections::delete_connection();

                            connections_ctrl::delete_conn_ctrl();
                            exit(SUCCESS);
                        }
                        break;
                    case OUT_OF_CONN:
                        cout << "Server is out of capacity, closing client socket" << endl;
                        if (close(client_sock)) {
                            fprintf(stderr,"ERROR: couldn't close client socket\n");
                        } 
                        break;
                }

            } else {
                cout << "ERROR: accepting client connection " << strerror(errno) << endl;
            }
        } else {
            if ((errno = EINTR) && exit_mainloop) {
                connections_ctrl* ctrl = connections_ctrl::get_conn_ctrl();
                ctrl->shut_down_procs();
                connections_ctrl::delete_conn_ctrl();
                cout << "Server is shutting down" << endl;
                break;
            }
        }
    }
    if (close(server_sock)) {
        cout << "ERROR: closing socket " << strerror(errno) << endl;
    }
}
