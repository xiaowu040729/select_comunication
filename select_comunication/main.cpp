#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>




int main()
{
    //创建套接字
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1)
    {
        perror("socket");
    }

    //绑定
    struct  sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(9999);
    saddr.sin_addr.s_addr = INADDR_ANY;
    socklen_t len = sizeof(saddr);
    int ret =bind(fd, (struct sockaddr*) & saddr, len);
    if (ret == -1)
    {
        perror("bind");
    }
    //监听
    ret =listen(fd, 100);
    if (ret == -1)
    {
        perror("listen");
    }

    struct sockaddr_in caddr;
    socklen_t lens = sizeof(caddr);

    fd_set reset;//文件描述符集合
    FD_ZERO(&reset);//初始化集合
    FD_SET(fd, &reset);//把fd放入集合中
   
    int maxi = fd;      //记录最大的那个文件描述符
   
    while (1)
    {
        fd_set tmp = reset;     //因为内核会修改数据所以要先把它存起来
        int ret =select(maxi + 1, &tmp, NULL, NULL, NULL);         //先判断文件描述符是否可读
        //判断是不是监听的文件描述符
        if (FD_ISSET(fd, &tmp))
        {
            int fds =accept(fd, (struct sockaddr*)&saddr, &len);
            FD_SET(fds, &reset);        //把新的文件描述符存入集合中
            if (fds > maxi)
            {
                maxi = fds;
            }
        }
        for (int i = 0; i <= maxi; i++)
        {
            if (i != fd && FD_ISSET(i, &tmp))       //首先检测文件描述它不是用于监听的 而且它要在检测后的集合中
            {
                //接收数据
                char buff[1024];
                int len = recv(i, buff, sizeof(buff), 0);//接收客户端数据

                if (len > 0)
                {
                    printf("client says:%s\n", buff);
                    send(i, buff, strlen(buff) + 1, 0); //再把数据发送回去

                }
                else if (len == 0)
                {
                    printf("client disconnected");
                    FD_CLR(i, &reset);      //断开连接之后要把文件描述符从集合里删除
                    close(i);
                    break;
                }
                else {
                    perror("recv");
                }
            }
        }

    }

    close(fd); 
    return 0;
}