/*
 * scull.h -- definitions for the char module
 *
 * Copyright (C) 2001 Alessandro Rubini and Jonathan Corbet
 * Copyright (C) 2001 O'Reilly & Associates
 *
 * The source code in this file can be freely used, adapted,
 * and redistributed in source or binary form, so long as an
 * acknowledgment appears in derived source files.  The citation
 * should list that the code comes from the book "Linux Device
 * Drivers" by Alessandro Rubini and Jonathan Corbet, published
 * by O'Reilly & Associates.   No warranty is attached;
 * we cannot take responsibility for errors or fitness for use.
 *
 * $Id: scull.h,v 1.11 2001/07/18 22:28:18 rubini Exp $
 */

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

#define SCULL_MAX_TYPE 5

#endif /* __DRIVER_H__ */
