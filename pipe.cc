#include <iostream>
#include <string>
#include <cstdio>
#include <cstring>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;

// 为什么不定义全局buffer来进行通信呢？？ 因为有写时拷贝的存在，无法更改通信！

int main()
{
    // 1. 创建管道
    int pipefd[2] = {0}; // pipefd[0(嘴巴，读书)]: 读端 , pipefd[1(钢笔，写)]: 写端
    int n = pipe(pipefd);
    assert(n != -1); // debug assert, release assert
    (void)n;

#ifdef DEBUG
    cout << "pipefd[0]: " << pipefd[0] << endl; // 3
    cout << "pipefd[1]: " << pipefd[1] << endl; // 4
#endif
    // 2. 创建子进程
    pid_t id = fork();
    assert(id != -1);
    if (id == 0)
    {
        //子进程 - 读
        // 3. 构建单向通信的信道，父进程写入，子进程读取
        // 3.1 关闭子进程不需要的fd
        close(pipefd[1]);
        char buffer[1024 * 8];
        while (true)
        {
            // sleep(20);
            // 写入的一方，fd没有关闭，如果有数据，就读，没有数据就等
            // 写入的一方，fd关闭, 读取的一方，read会返回0，表示读到了文件的结尾！
            ssize_t s = read(pipefd[0], buffer, sizeof(buffer) - 1);
            if (s > 0)
            {
                buffer[s] = 0;
                cout << "child get a message[" << getpid() << "] Father# " << buffer << endl;
            }
            else if(s == 0)
            {
                cout << "writer quit(father), me quit!!!" << endl;
                break;
            }
        }
        // close(pipefd[0]);
        exit(0);
    }
    //父进程 - 写
    // 3. 构建单向通信的信道
    // 3.1 关闭父进程不需要的fd
    close(pipefd[0]);
    string message = "我是父进程，我正在给你发消息";
    int count = 0;
    char send_buffer[1024 * 8];
    while (true)
    {
        // 3.2 构建一个变化的字符串
        snprintf(send_buffer, sizeof(send_buffer), "%s[%d] : %d",
                 message.c_str(), getpid(), count++);
        // 3.3 写入
        write(pipefd[1], send_buffer, strlen(send_buffer));
        // 3.4 故意sleep
        sleep(1);
        cout << count << endl;
        if (count == 5){
            cout << "writer quit(father)" << endl;
            break;
        }
    }
    close(pipefd[1]);
    pid_t ret = waitpid(id, nullptr, 0);
    cout << "id : " << id << " ret: " << ret <<endl;
    assert(ret > 0); 
    (void)ret;

    return 0;
}
