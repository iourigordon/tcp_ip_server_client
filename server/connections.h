#ifndef __CONNECTIONS_H__
#define __CONNECTIONS_H__

class connections {
    connections(int ctrl_in, int ctrl_out);
    ~connections();

    pthread_t m_ThreadId;

    static connections* connections_pool;
public:
    static connections* get_connection(int ctrl_in, int ctrl_out) {return (connections_pool==NULL)?(connections_pool = new connections(ctrl_in, ctrl_out)):connections_pool;}

    int run();
    int add_connection(int client_socket);

private:
    int m_CtrlIn;
    int m_CtrlOut;    
};

#endif
