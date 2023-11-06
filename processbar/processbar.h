#pragma once

#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>
#include<time.h>

#define NUM 103
#define Body '='
#define Head '>'
#define FILESIZE 1024*1024*1024

typedef void (*callback_t)(double);

void process();

void process_flush(double rate);

void download(callback_t cb);
