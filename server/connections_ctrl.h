#ifndef __CONNECTIONS_CTRL_H__
#define __CONNECTINOS_CTRL_H__

typedef enum _add_new_conn_action {
    SUCCESS,
    CREATE_CONN,
    OUT_OF_CONN
} add_new_conn_action;

class connections_ctrl
{
    public:
        static connections_ctrl* get_conn_ctrl(int MaxProcs = 0); 
        add_new_conn_action add_client(int SockId, string IpAddr); 
        void add_new_proc(pid_t ProcId, int ToChld, int ToPrnt);

    private:
        connections_ctrl(int MaxProcs):m_MaxProcs(MaxProcs) {}
        class proc_io {
            friend class connections_ctrl;

            pid_t m_ProcId;
            int m_ToChld;
            int m_ToPrnt;
        };

        int m_MaxProcs;
        vector<proc_io> m_Procs;

        static connections_ctrl* m_Instance;
};

#endif
