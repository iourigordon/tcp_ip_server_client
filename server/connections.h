#ifndef __CONNECTIONS_H__
#define __CONNECTIONS_H__

class connections {
    connections(int ChldSock);
    ~connections();

public:
    static connections* get_connection(int ChldSock);
    int run();
    int add_connection(int client_socket);

private:

    class client_info {
        friend class connections;
        int    num_msgs;
        string desc;
    };

    static connections* connections_pool;

    int m_ChldSock;
    int m_MaxFd;

    map<int,client_info> m_FdDescMap;
};

#endif
