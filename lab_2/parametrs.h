#ifndef __parametrs_h__
#define __parametrs_h__

#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <errno.h>
#include <sys/time.h>

#ifndef BEG
#define BEG 0.0001
#endif

#ifndef END
#define END 1
#endif

#ifndef F
#define F(__a) (sin(1/(__a))*sin(1/(__a))/(__a)/(__a))
#endif

#ifndef THREAD_COUNT
#define THREAD_COUNT 10
#endif

#ifndef EPS
#define EPS 0.00001
#endif

#ifndef SPK 
#define SPK 5
#endif

#endif
