#include "serversock.h"

ServerSock::ServerSock(std::function<std::array<char, sizeOutBuff>(std::array<char, sizeInBuff>&)> _callback): callback(_callback)
{

}

ServerSock::~ServerSock()
{
    stopServer();
}

void ServerSock::stopServer()
{
    isRunning = false;
    shutdown(socketFileDescriptor, SHUT_RDWR);
}

void ServerSock::startServerSock()
{
    isRunning = true;

    sockaddr_un local, remote;


    socketFileDescriptor = socket(AF_UNIX, SOCK_STREAM, 0);
    if(-1 == socketFileDescriptor)
    {
        printf("Error on socket() call \n");
        return;
    }

    local.sun_family = AF_UNIX;
    strcpy(local.sun_path, socket_path);
    unlink(local.sun_path);
    int len = strlen(local.sun_path) + sizeof(local.sun_family);
    if(bind(socketFileDescriptor, (struct sockaddr*)&local, len) != 0)
    {
        printf("Error on binding socket \n");
        return;
    }

    if(listen(socketFileDescriptor, nIncomingConnections) != 0)
    {
        printf("Error on listen call \n");
    }

    while (isRunning)
    {
        unsigned int sock_len = 0;
        printf("Waiting for connection.... \n");
        if((socketDescriptor = accept(socketFileDescriptor, (sockaddr*)&remote, &sock_len)) == -1)
        {
            printf("Error on accept() call \n");
            return;
        }

        printf("Server connected \n");

        int data_recv = 0;
        do{
            memset(recv_buf.data(), 0, sizeInBuff * sizeof(char));
            data_recv = recv(socketDescriptor, recv_buf.data(), sizeInBuff, 0);
            if(data_recv > 0)
            {
                printf("Data received: %d : %s \n", data_recv, recv_buf.data());

                char* res = strstr(recv_buf.data(), "quit");
                if(res != 0)
                {
                    isRunning = false;
                    break;
                }

                std::array<char, sizeOutBuff> send_buf = callback(recv_buf);
                ssize_t sendRes = send(socketDescriptor, send_buf.data(), strlen(send_buf.data()) * sizeof(char), 0);
                if(sendRes == -1)
                {
                    printf("Error on send() call \n");
                }
            }
            else
            {
                printf("Error on recv() call \n");
            }
        }while(data_recv > 0);

        close(socketDescriptor);
    }
}


