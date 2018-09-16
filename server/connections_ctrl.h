#ifndef __CONNECTIONS_CTRL_H__
#define __CONNECTINOS_CTRL_H__

typedef enum _ADD_CONN_ACTION {
    SUCCESS,
    CREATE_CONN,
    OUT_OF_CONN
} ADD_CONN_ACTION;

class connections_ctrl
{
    public:
        static connections_ctrl* get_conn_ctrl(int MaxProcs = 0); 
        static void delete_conn_ctrl();
        static int create_connection(int fds[2]);
        static int send_client_sock(int CommSock, int FD, const char* IpAddr);
        static int receive_client_sock(int CommSock);

        ADD_CONN_ACTION add_client(int SockId, string IpAddr); 
        void add_new_proc(pid_t ProcId, int PrntSock);
        void shut_down_procs();

    private:
        connections_ctrl(int MaxProcs):m_MaxProcs(MaxProcs) {}
        ~connections_ctrl() {}
        connections_ctrl(connections_ctrl& Rhs);
        connections_ctrl& operator=(connections_ctrl& Rhs);

        class proc_io {
            friend class connections_ctrl;

            pid_t m_ProcId;
            int m_PrntSock;
        };

        int m_MaxProcs;
        vector<proc_io> m_Procs;

        static connections_ctrl* m_Instance;
};

#endif
