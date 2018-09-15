#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>

#include <string>
#include <vector>
#include <cstddef>
#include <sstream>
#include <iostream>

using namespace std;

#include "connections_ctrl.h"
#include "ctrl_msg_fact.h"
#include "ctrl_message.h"

#define BUFF_LENGTH 1024

connections_ctrl* connections_ctrl::m_Instance = NULL;

connections_ctrl* 
connections_ctrl::
get_conn_ctrl(int MaxProcs)
{
    if (!m_Instance)
        m_Instance = new connections_ctrl(MaxProcs);

    return m_Instance;
}

ADD_CONN_ACTION
connections_ctrl::
add_client(int SockId, string IpAddr)
{
    int ret;
    fd_set conn_set;
    char buff[BUFF_LENGTH];

    if (m_Procs.empty())
        return CREATE_CONN;

    ctrl_msg_add_client* msg = dynamic_cast<ctrl_msg_add_client*>(ctrl_msg_fact::create_msg(CTRL_MSG_ADD_CLIENT));
    msg->set_client_ip_addr(IpAddr);
    ostringstream& msg_stream = ctrl_msg_fact::serialize_message(msg);
    for(vector<proc_io>::iterator curr = m_Procs.begin();curr<m_Procs.end();curr++) {
        if (write(curr->m_PrntSock,msg_stream.str().c_str(),msg_stream.str().size()) != -1) {
            FD_ZERO(&conn_set);
            FD_SET(curr->m_PrntSock,&conn_set);
            if (select(curr->m_PrntSock+1,&conn_set,NULL,NULL,NULL) != -1) {
                if (FD_ISSET(curr->m_PrntSock,&conn_set) && ((ret = read(curr->m_PrntSock,buff,BUFF_LENGTH)) != -1)) {
                    istringstream in_stream(string(buff,ret));
                    ctrl_msg* msg = ctrl_msg_fact::deserialize_stream(in_stream);
                    if (msg->get_msg_id() == CTRL_MSG_ACK) {
                        cout << "New client was added to connections" << endl;

                        //send socket_descriptor as well
                        connections_ctrl::send_client_sock(curr->m_PrntSock,SockId,IpAddr.c_str());
                        delete msg;
                        return SUCCESS;
                    }
                    delete msg;
                }
            }
        }
    }

    return OUT_OF_CONN;
}

void
connections_ctrl::
add_new_proc(pid_t ProcId, int PrntSock)
{
    proc_io desc;

    desc.m_ProcId = ProcId;
    desc.m_PrntSock = PrntSock;

    m_Procs.push_back(desc);
}

void
connections_ctrl::
shut_down_procs()
{
    int status;

    ctrl_msg_shutdown* msg = dynamic_cast<ctrl_msg_shutdown*>(ctrl_msg_fact::create_msg(CTRL_MSG_SHUT_DOWN));
    ostringstream& msg_stream = ctrl_msg_fact::serialize_message(msg);
    for(vector<proc_io>::iterator curr = m_Procs.begin();curr<m_Procs.end();curr++) {
        if (write(curr->m_PrntSock,msg_stream.str().c_str(),msg_stream.str().size()) != -1) {
            cout << "shut down message sent to " << curr->m_ProcId << endl;
        }
    }
    for(vector<proc_io>::iterator curr = m_Procs.begin();curr<m_Procs.end();curr++) {
        cout << "Waiting for " << curr->m_ProcId << " to shut down" << endl;
        if (waitpid(curr->m_ProcId,&status,0) == -1) {
            cout << "ERROR: wait for " << curr->m_ProcId << " to terminate " << strerror(errno) << endl;
        }
        cout << "Proc " << curr->m_ProcId << " is over " << endl;
    }
}

int
connections_ctrl::
create_connection(int fd[2])
{
    int ret;
    if (ret = socketpair(AF_UNIX,SOCK_STREAM,0,fd) !=0 ) {
        cout << "Failed to create linux sockets" << endl;
    }

    return ret;
}

int
connections_ctrl::
send_client_sock(int CommSock, int FD, const char* IpAddr)
{
    struct msghdr msg = { 0 };
    char buf[CMSG_SPACE(sizeof(FD))];

    memset(buf, '\0', sizeof(buf));
    struct iovec io = { .iov_base = (void*)IpAddr, .iov_len = strlen(IpAddr)+1 };

    msg.msg_iov = &io;
    msg.msg_iovlen = 1;
    msg.msg_control = buf;
    msg.msg_controllen = sizeof(buf);

    struct cmsghdr * cmsg = CMSG_FIRSTHDR(&msg);
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;
    cmsg->cmsg_len = CMSG_LEN(sizeof(FD));

    memcpy(CMSG_DATA(cmsg),&FD,sizeof(FD));
    msg.msg_controllen = cmsg->cmsg_len;

    if (sendmsg(CommSock, &msg, 0) == -1)
        cout << "ERROR: sendmsg failed " << strerror(errno) << endl;

    return 0;
}

int
connections_ctrl::
receive_client_sock(int CommSock)
{
    int fd;
    struct msghdr msg;
    struct cmsghdr* cmsg;
    char c_buffer[BUFF_LENGTH];
    char m_buffer[BUFF_LENGTH];
    struct iovec io = { .iov_base = m_buffer, .iov_len = sizeof(BUFF_LENGTH) };

    memset(&msg,0,sizeof(struct msghdr));
    msg.msg_iov = &io;
    msg.msg_iovlen = 1;

    msg.msg_control = c_buffer;
    msg.msg_controllen = BUFF_LENGTH;

    if (recvmsg(CommSock, &msg, 0) < 0)
        cout << "ERROR: recvmsg failed " << strerror(errno) << endl;

    cmsg = CMSG_FIRSTHDR(&msg);
    memcpy(&fd,CMSG_DATA(cmsg),sizeof(fd));

    return fd;
}
