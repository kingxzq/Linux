#include "comm.hpp"

// 是不是对应的程序，在加载的时候，会自动构建全局变量，就要调用该类的构造函数 -- 创建管道文件
// 程序退出的时候，全局变量会被析构，自动调用析构函数，会自动删除管道文件
Init init; 

string TransToHex(key_t k)
{
    char buffer[32];
    snprintf(buffer, sizeof buffer, "0x%x", k);
    return buffer;
}

int main()
{
    // 我们之前为了通信，所做的所有的工作，属于什么工作呢：让不同的进程看到了同一份资源(内存)
    // 1. 创建公共的Key值
    key_t k = ftok(PATH_NAME, PROJ_ID);
    assert(k != -1);

    Log("create key done", Debug) << " server key : " << TransToHex(k) << endl;

    // 2. 创建共享内存 -- 建议要创建一个全新的共享内存 -- 通信的发起者
    int shmid = shmget(k, SHM_SIZE, IPC_CREAT | IPC_EXCL | 0666); //
    if (shmid == -1)
    {
        perror("shmget");
        exit(1);
    }
    Log("create shm done", Debug) << " shmid : " << shmid << endl;

    // sleep(10);
    // 3. 将指定的共享内存，挂接到自己的地址空间
    char *shmaddr = (char *)shmat(shmid, nullptr, 0);
    Log("attach shm done", Debug) << " shmid : " << shmid << endl;

    // sleep(10);

    // 这里就是通信的逻辑了
    // 将共享内存当成一个大字符串
    // char buffer[SHM_SIZE];
    // 结论1： 只要是通信双方使用shm，一方直接向共享内存中写入数据，另一方，就可以立马看到对方写入的数据。
    //         共享内存是所有进程间通信(IPC)，速度最快的！不需要过多的拷贝！！（不需要将数据给操作系统）
    // 结论2： 共享内存缺乏访问控制！会带来并发问题 【如果我想一定程度的访问控制呢? 能】
    
    int fd = OpenFIFO(FIFO_NAME, READ);
    for(;;)
    {
        Wait(fd);

        // 临界区
        printf("%s\n", shmaddr);
        if(strcmp(shmaddr, "quit") == 0) break;
        // sleep(1);
    }
    // 4. 将指定的共享内存，从自己的地址空间中去关联
    int n = shmdt(shmaddr);
    assert(n != -1);
    (void)n;
    Log("detach shm done", Debug) << " shmid : " << shmid << endl;
    // sleep(10);

    // 5. 删除共享内存,IPC_RMID即便是有进程和当下的shm挂接，依旧删除共享内存
    n = shmctl(shmid, IPC_RMID, nullptr);
    assert(n != -1);
    (void)n;
    Log("delete shm done", Debug) << " shmid : " << shmid << endl;

    CloseFifo(fd);
    return 0;
}