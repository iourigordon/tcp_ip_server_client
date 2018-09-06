#include <pthread.h>
#include <cstddef>

#include "connections.h"

using namespace std;

connections::connections(int ctrl_in, int ctrl_out):
                         m_CtrlIn(ctrl_in), m_CtrlOut(ctrl_out)
{   
}

connections::~connections()
{
}

int
connections::add_connection(int client_socket)
{
    return 0;
}

int
connections::run()
{
    //add pipe and sockets to fd_set and start listening
    return 0;
}
