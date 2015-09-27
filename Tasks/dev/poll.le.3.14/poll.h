#define DEVNAME "poll"
#define LEN_MSG 160

#ifndef __KERNEL__         // only user space applications
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h> 
#include <errno.h> 
#include "user.h"

#else                      // for kernel space module
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/poll.h>
#include <linux/sched.h>

#endif

