#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>


#include <map>
#include <sstream>
#include <iostream>
#include <iomanip>

using namespace std;

#include "connections.h"
#include "ctrl_msg_fact.h"
#include "ctrl_message.h"

#define MAX_PEND_CONNS 10

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

    fd_set server_fds;
    socklen_t client_addr_size;
    struct sockaddr_in server_addr, client_addr;

    ctrl_msg_add_client* msg = dynamic_cast<ctrl_msg_add_client*>(ctrl_msg_fact::create_msg(CTRL_MSG_ADD_CLIENT));
    msg->set_socket_id(1234);
    string ip_addr = "10.10.10.10";
    msg->set_client_ip_addr(ip_addr);
    ostringstream& str_stream = ctrl_msg_fact::serialize_message(msg);
    cout << printable(str_stream.str()) << endl;
   

    istringstream ser_stream(str_stream.str());
    ctrl_msg* deser_msg = ctrl_msg_fact::deserialize_stream(ser_stream);

    ctrl_msg_add_client* client_msg = dynamic_cast<ctrl_msg_add_client*>(deser_msg);
    cout << "SockID = " << client_msg->get_socket_id() << "; IP addr = " << client_msg->get_client_ip_addr() << endl;

    return 0;
#if 0


    cout << printable(str_stream.str()) << endl;
    deser_msg.deserialize(ser_stream);

    return 0; 
#endif

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
                        write(to_chld_pipe[1],"hello",6);
                    } else {
                        //child process
                        close(to_chld_pipe[1]);
                        close(to_prnt_pipe[0]);                        

                        connections *client_connections = connections::get_connection(to_chld_pipe[0],to_prnt_pipe[1]);
                        client_connections->run();
                    }
                }
            } else {
                cout << "ERROR: accepting client connection " << strerror(errno) << endl;
            }
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
