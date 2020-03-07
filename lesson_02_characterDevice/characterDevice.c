#ifndef __KERNEL__
#define __KERNEL__
#endif
#ifndef MODULE
#define MODULE
#endif

#include <asm/current.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/types.h>
#include "driver.h"

MODULE_LICENSE("Dual MIT/GPL");
MODULE_AUTHOR("Liu Yu - source@liuyu.com");
MODULE_DESCRIPTION("Linux Kernel Module: Character Device Driver.");

typedef struct file *FILEPTR;
typedef struct inode *INODEPTR;

dev_t gDev = 0;

loff_t _llseek(struct file *filp, loff_t off, int whence)
{
    Test_Dev *dev = (Test_Dev *)filp->private_data;
    loff_t newpos;

    switch (whence)
    {
    case 0:
        newpos = off;
        break;

    case 1:
        newpos = filp->f_pos + off;
        break;

    case 2:
        newpos = dev->size + off;
        break;

    default:
        return -EINVAL;
    }
    if (newpos < 0)
        return -EINVAL;
    filp->f_pos = newpos;
    return newpos;
}

ssize_t _read(struct file *filp,
              char *buf,
              size_t count,
              loff_t *f_pos)
{
    Test_Dev *dev = filp->private_data;
    Test_Dev *dptr;
    int quantum = dev->quantum;
    int qset = dev->qset;
    int itemsize = quantum * qset;
    int item, s_pos, q_pos, rest;
    ssize_t ret = 0;

    if (down_interruptible(&dev->sem))
        return -ERESTARTSYS;
    if (*f_pos >= dev->size)
        goto out;
    if (*f_pos + count > dev->size)
        count = dev->size - *f_pos;

    item = (long)*f_pos / itemsize;
    rest = (long)*f_pos % itemsize;
    s_pos = rest / quantum;
    q_pos = rest % quantum;

    dptr = alloc_device(dev, item);

    if (!dptr->data)
        goto out;
    if (!dptr->data[s_pos])
        goto out;

    if (count > quantum - q_pos)
        count = quantum - q_pos;

    if (copy_to_user(buf, dptr->data[s_pos] + q_pos, count))
    {
        ret = -EFAULT;
        goto out;
    }
    *f_pos += count;
    ret = count;

out:
    up(&dev->sem);
    return ret;
}

ssize_t _write(struct file *filp,
               const char *buf,
               size_t count,
               loff_t *f_pos)
{
    Test_Dev *dev = filp->private_data;
    Test_Dev *dptr;
    int quantum = dev->quantum;
    int qset = dev->qset;
    int itemsize = quantum * qset;
    int item, s_pos, q_pos, rest;
    ssize_t ret = -ENOMEM;

    if (down_interruptible(&dev->sem))
        return -ERESTARTSYS;

    item = (long)*f_pos / itemsize;
    rest = (long)*f_pos % itemsize;
    s_pos = rest / quantum;
    q_pos = rest % quantum;

    dptr = alloc_device(dev, item);
    if (!dptr->data)
    {
        dptr->data = kmalloc(qset * sizeof(char *), GFP_KERNEL);
        if (!dptr->data)
            goto out;
        memset(dptr->data, 0, qset * sizeof(char *));
    }
    if (!dptr->data[s_pos])
    {
        dptr->data[s_pos] = kmalloc(quantum, GFP_KERNEL);
        if (!dptr->data[s_pos])
            goto out;
    }

    if (count > quantum - q_pos)
        count = quantum - q_pos;

    if (copy_from_user(dptr->data[s_pos] + q_pos, buf, count))
    {
        ret = -EFAULT;
        goto out;
    }
    *f_pos += count;
    ret = count;

    if (dev->size < *f_pos)
        dev->size = *f_pos;

out:
    up(&dev->sem);
    return ret;
}

long _ioctl(FILEPTR pf, unsigned int i, unsigned long l)
{
    // Test_Dev *dev; 
    // int num = NUM(inode->i_rdev);
    // int type = TYPE(inode->i_rdev);

    
    // if (!filp->private_data && type) {
    //     if (type > SCULL_MAX_TYPE) return -ENODEV;
    //     filp->f_op = scull_fop_array[type];
    //     return filp->f_op->open(inode, filp); 
    // }

    
    // dev = (Test_Dev *)filp->private_data;
    // if (!dev) {
    //     if (num >= scull_nr_devs) return -ENODEV;
    //     dev = &Test_devices[num];
    //     filp->private_data = dev; 
    // }

    // MOD_INC_USE_COUNT;  
    
    // if ( (filp->f_flags & O_ACCMODE) == O_WRONLY) {
    //     if (down_interruptible(&dev->sem)) {
    //         MOD_DEC_USE_COUNT;
    //         return -ERESTARTSYS;
    //     }
    //     scull_trim(dev); 
    //     up(&dev->sem);
    // }

    return 0;          
}

int _open(struct inode *inode, struct file *filp)
{
    return 1;
}

int _release(INODEPTR nod, FILEPTR pf)
{
    return 1;
}

int snprintf(char *buf, size_t size, const char *fmt, ...)
{
    va_list args;
    int i;

    va_start(args, fmt);
    i = vsnprintf(buf, size, fmt, args);
    va_end(args);

    return i;
}
// EXPORT_SYMBOL(snprintf);

static __init int driver_init(void)
{
    int result;

    printk(KERN_ALERT "Character Device Driver\n");
    printk(KERN_ALERT "current Process is: %s, PID is: %d\n", current->comm, current->pid);

    // alloc device id
    printk(KERN_INFO "alloc device id\n");
    result = alloc_chrdev_region(&gDev, 0, 4, "_test_02_character_device_driver_");
    if (result < 0)
    {
        printk(KERN_ERR "alloc device id fail\n");
        return result;
    }
    printk(KERN_INFO "device id: %u\n", gDev);

    // file operation
    printk(KERN_INFO "file operation\n");
    struct file_operations fops = {
        .owner = THIS_MODULE,
        .llseek = _llseek,
        .read = _read,
        .write = _write,
        .compat_ioctl = _ioctl,
        .open = _open,
        .release = _release,
    };
    printk(KERN_INFO "file operation: %p\n", &fops);

    return 0;
}

static __exit void driver_exit(void)
{
    printk(KERN_ALERT "current Process is: %s, PID is: %d\n", current->comm, current->pid);
    printk(KERN_ALERT "Character Device Driver Exist\n");

    // release device id
    printk(KERN_INFO "release device id[%u]\n", gDev);
    unregister_chrdev_region(gDev, 4);
}

Test_Dev *alloc_device(Test_Dev *dev, int n)
{
    while (n--)
    {
        if (!dev->next)
        {
            dev->next = kmalloc(sizeof(Test_Dev), GFP_KERNEL);
            memset(dev->next, 0, sizeof(Test_Dev));
        }
        dev = dev->next;
    }
    return dev;
}

EXPORT_SYMBOL(_llseek);
EXPORT_SYMBOL(_read);
EXPORT_SYMBOL(_write);
EXPORT_SYMBOL(_ioctl);
EXPORT_SYMBOL(_open);

module_init(driver_init);
module_exit(driver_exit);
