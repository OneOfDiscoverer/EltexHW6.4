#ifndef _MAIN_H
#define _MAIN_H

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <mqueue.h>
#include "list.h"
#include <pthread.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <semaphore.h>
#include <time.h>
#include <sys/epoll.h>

#define NUMOFTREAD  10
#define MAX_EVENTS  100
#define BUFSIZE     1024

#endif