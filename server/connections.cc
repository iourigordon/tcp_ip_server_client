#include <string.h>
#include <unistd.h>
#include <sys/select.h>

#include <map>
#include <cstddef>
#include <iostream>

using namespace std;

#include "connections.h"

#define BUF_LENGTH 1024

connections* connections::connections_pool = NULL;

connections::connections(int ctrl_in, int ctrl_out):
                         m_CtrlIn(ctrl_in), m_CtrlOut(ctrl_out)
{   
    m_MaxFd = m_CtrlIn;

    m_FdDescMap[m_CtrlIn] = "Control In";
    m_FdDescMap[m_CtrlOut] = "Control Out";
}

connections::~connections()
{
}

connections*
connections::get_connection(int ctrl_in, int ctrl_out)
{
    if (connections_pool == NULL)
        connections_pool = new connections(ctrl_in,ctrl_out);

    return connections_pool;
}

int
connections::add_connection(int client_socket)
{
    return 0;
}

int
connections::run()
{
    char buff[BUF_LENGTH];
    fd_set working_set;

    cout << "Connections Proc is running" << endl;

    for(;;) {
        for (map<int,string>::iterator fd = m_FdDescMap.begin(); fd != m_FdDescMap.end(); fd ++) {
            if (fd->first != m_CtrlOut){
                FD_SET(fd->first,&working_set);
                if (fd->first > m_MaxFd)
                    m_MaxFd = fd->first;
            }
        }
        memset(buff,0,BUF_LENGTH);
        if (select(m_MaxFd+1,&working_set,NULL,NULL,NULL) != -1) {
            for (map<int,string>::iterator fd = m_FdDescMap.begin(); fd != m_FdDescMap.end(); fd++) {
               if (FD_ISSET(fd->first,&working_set)) {
                    if (::read(fd->first,buff,BUF_LENGTH) != -1) {
                        cout << "Received " << (char*)buff << " from " << fd->second << endl;
                    }                
                }
            }
        }
    }
    return 0;
}
