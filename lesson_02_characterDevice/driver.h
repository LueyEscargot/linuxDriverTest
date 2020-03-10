
#ifndef __DRIVER_H__
#define __DRIVER_H__

#include <linux/semaphore.h>

typedef struct Test_Dev
{
     void **data;
     struct Test_Dev *next;
     int quantum;
     int qset;
     unsigned long size;
     void *handle;
     unsigned int access_key;
     struct semaphore sem;
} Test_Dev;

#define TEST_MAX_TYPE 1
#ifndef TEST_NR_DEVS
#define TEST_NR_DEVS 4
#endif

#endif /* __DRIVER_H__ */
