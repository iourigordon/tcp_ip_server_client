#ifndef __CONNECTIONS_H__
#define __CONNECTIONS_H__

class connections {
    connections();
    ~connections();

    pthread_t m_ThreadId;

    static connections* connections_pool;
public:
    static connections* get_connection() {return (connections_pool==NULL)?(connections_pool = new connections()):connections_pool;}

    int add_connection(int client_socket);
};

#endif
