#include "processbar.h"

const char *lable = "|/-\\";

void process(){
  char buffer[NUM];
  memset(buffer,'\0',sizeof(buffer));
  int cnt=0;
  int n=strlen(lable);
  buffer[0]=Head;
  while(cnt<=100){
    printf("[%-100s][%3d%%][%c]\r",buffer,cnt,lable[cnt%n]);
    fflush(stdout);
    buffer[cnt++]=Body;
    if(cnt<100) buffer[cnt]=Head;
    usleep(50000);
  }
  printf("\n");
}

char buffer[NUM]={0};
void process_flush(double rate){
  static int cnt =0;
  int n =strlen(lable);
  if(rate<=1.0) buffer[0]=Head;
  printf("[%-100s][%.1f%%][%c]\r",buffer,rate,lable[cnt%n]);
  fflush(stdout);

  buffer[(int)rate]=Body;
  if((int)rate+1<100) buffer[(int)(rate+1)]=Head;
  if(rate>=100.0) printf("\n");

  cnt++;
  cnt%=n;

}

void download(callback_t cb){
  srand(time(NULL)^1023);
  int total=FILESIZE;
  while(total){
    usleep(10000);
    int one =rand()%(1024*1024*10);
    total-=one;
    if(total<0)total=0;

    int download=FILESIZE-total;
    double rate = (download*1.0/(FILESIZE))*100.0;
    //cb(rate);
    process_flush(rate);
  }
}
