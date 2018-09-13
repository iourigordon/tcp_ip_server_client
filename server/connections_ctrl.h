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
        connections_ctrl(int MaxProcs):m_MaxProcs(MaxProcs) {}
    
        add_new_conn_action add_client(int SockId, string IpAddr); 
    private:

        class proc_io {
            friend class connections_ctrl;

            pid_t m_ProcId;
            int m_ToChld;
            int m_ToPrnt;
        };

        int m_MaxProcs;
        vector<proc_io> m_Procs;
};

#endif
