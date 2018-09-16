#ifndef __CONNECTIONS_H__
#define __CONNECTIONS_H__

class connections {

    public:
        static connections* get_connection(int ChldSock, int MaxConnections);
        static void delete_connection();
        int run();

    private:
        connections(int MaxConnections, int ChldSock);
        ~connections() {}
        connections(connections& Rhs);
        connections& operator=(connections& Rhs);

        int process_ctrl_msg(char* Buff, int BuffLength);

        class client_info {
            friend class connections;
            int    num_msgs;
            string desc;
        };

        static connections* connections_pool;

        int m_MaxConnections;
        int m_ChldSock;
        int m_MaxFd;

        map<int,client_info> m_FdDescMap;
};

#endif
