#include "comm.hpp"

int main()
{
    Log("child pid is : ", Debug) << getpid() << endl;
    key_t k = ftok(PATH_NAME, PROJ_ID);
    if (k < 0)
    {
        Log("create key failed", Error) << " client key : " << k << endl;
        exit(1);
    }
    Log("create key done", Debug) << " client key : " << k << endl;

    // 获取共享内存
    int shmid = shmget(k, SHM_SIZE, 0);
    if(shmid < 0)
    {
        Log("create shm failed", Error) << " client key : " << k << endl;
        exit(2);
    }
    Log("create shm success", Error) << " client key : " << k << endl;

    // sleep(10);

    char *shmaddr = (char *)shmat(shmid, nullptr, 0);
    if(shmaddr == nullptr)
    {
        Log("attach shm failed", Error) << " client key : " << k << endl;
        exit(3);
    }
    Log("attach shm success", Error) << " client key : " << k << endl;
    // sleep(10);

    int fd = OpenFIFO(FIFO_NAME, WRITE);
    // 使用
    // client将共享内存看做一个char 类型的buffer
    while(true)
    {
        ssize_t s = read(0, shmaddr, SHM_SIZE-1);
        if(s > 0)
        {
            shmaddr[s-1] = 0;
            Signal(fd);
            if(strcmp(shmaddr,"quit") == 0) break;
        }
    }

    CloseFifo(fd);
    // char a = 'a';
    // for(; a <= 'z'; a++)
    // {
    //     shmaddr[a-'a'] = a;
    //     // 我们是每一次都向shmaddr[共享内存的起始地址]写入
    //     // snprintf(shmaddr, SHM_SIZE - 1,\
    //     //     "hello server, 我是其他进程,我的pid: %d, inc: %c\n",\
    //     //     getpid(), a);
    //     sleep(5);
    // }

    // strcpy(shmaddr, "quit");

    // 去关联
    int n = shmdt(shmaddr);
    assert(n != -1);
    Log("detach shm success", Error) << " client key : " << k << endl;
    // sleep(10);

    // client 要不要chmctl删除呢？不需要！！

    return 0;
}