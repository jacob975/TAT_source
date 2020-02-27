
/*****************************************************************
 * Dave Mills 11-01-2001                                         *
 *                                                               *
 * Driver for Apogee Intstruments PPI CCD camera comtroller card *
 *                                                               *
 * Create device with:                                           *
 *     mknod /dev/apppi c 61 0                                   *
 *     chmod og+rw /dev/apppi   				 *
 *****************************************************************/
#include <linux/config.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/major.h>
#include <linux/mm.h>

#include <linux/module.h>    
#include <linux/fs.h>    
#include <linux/slab.h>
#include <linux/smp_lock.h>
#include <linux/spinlock.h>
#include <linux/completion.h>
#include <asm/uaccess.h>
#include <linux/vermagic.h>
#include <linux/init.h>

				/* interrupts */
#include <linux/sched.h>
#include <asm/irq.h>

				/* I/O commands */
#include <linux/ioport.h>
#include <asm/io.h>
#include <asm/segment.h>
#include <asm/system.h>
#include <linux/fs.h>
#include <linux/stat.h>
#include "ApogeeLinux.h"

/* Version Information */
#define DRIVER_VERSION "v1.2"
#define DRIVER_AUTHOR "Dave Mills, rfactory@theriver.com"
#define DRIVER_DESC "Apogee Parallel Port camera driver"
 

u32 m_BaseAddress;		/* pointer to register */
u32 m_RegisterOffset;		/* register offset */

#define LOBYTE(x) ((x) & 0xff)
#define HIBYTE(x) ((x >> 8) & 0xff)

/* 
*  variables related to the interrupt handler buffer and operations
*/ 
static char apppi_name[10]="apppi"; /* device name in /proc/interrupts */


static ssize_t apppi_read	(struct file *file, char *buffer, size_t count, loff_t *ppos);
static ssize_t apppi_write	(struct file *file, const char *buffer, size_t count, loff_t *ppos);
static int apppi_ioctl		(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg);
static int apppi_open		(struct inode *inode, struct file *file);
static int apppi_release		(struct inode *inode, struct file *file);

static struct file_operations apppi_fops = {
	/*
	 * The owner field is part of the module-locking
	 * mechanism. The idea is that the kernel knows
	 * which module to increment the use-counter of
	 * BEFORE it calls the device's open() function.
	 * This also means that the kernel can decrement
	 * the use-counter again before calling release()
	 * or should the open() function fail.
	 */
	.owner =	THIS_MODULE,

	.read =		apppi_read,
	.write =	apppi_write,
	.ioctl =	apppi_ioctl,
	.open =		apppi_open,
	.release =	apppi_release,
};

static unsigned int boardsInUse = 0;
static unsigned int apppi_addr[7] = {0,0,0,0,0,0,0};
static unsigned int apppi_offs[7] = {0,0,0,0,0,0,0};
#define MAXBOARDS 7     /* Increase this and the sizes of the
                           arrays below, if you need more.. */

static unsigned int bases[MAXBOARDS]={0x378,0x278,0x3bc,0x2bc,0,0,0};
static unsigned int offsets[MAXBOARDS]={0,0,0,0,0,0,0};
static unsigned int boards[MAXBOARDS]={0,0,0,0,0,0,0};
#ifdef MODULE
MODULE_PARM(apppi_addr, "1-7i");
MODULE_PARM_DESC(apppi_addr, "Board io addresses (max of 7)");
MODULE_PARM(apppi_offs, "1-7i");
MODULE_PARM_DESC(apppi_offs, "Board register offsets (max of 7)");
#endif 


static int __init apppi_init_module(void) { 
  int result;
  int i,j, ii;
  int baddr;
  int read32;

  printk("<1>apppi: ********** Initializing module...\n");

  for(i=0;i<MAXBOARDS;i++) {
     if (apppi_addr[i]) {
        baddr = 0;
        for(j=0;j<MAXBOARDS;j++) {
          if (apppi_addr[i] == bases[j]) {
             boards[boardsInUse] = apppi_addr[i];
             offsets[boardsInUse] = apppi_offs[i];
             boardsInUse++;
             baddr = 1;
             printk("<1>apppi: Board %d registered at 0x%x offset 0x%x\n",i+1,apppi_addr[i],apppi_offs[i]); 
             for (ii=0;ii<34;ii=ii+2) {
                read32 = inw(apppi_addr[i]+ii);
                printk("<1>apppi: reg %d = 0x%x\n",ii,read32); 
             }
          }
        }
        if (baddr == 0) { 
 	       printk("<1>apppi: Illegal port number for board %d , 0x%x\n",i+1,apppi_addr[i]);
        }
     }
  }


  if (boardsInUse == 0) { 
	printk("<1>apppi: No boards registered, module not loaded\n");
	return(-1);
  }

  for(i=0;i<boardsInUse;i++) {
      request_region(boards[i],32,"apppi");
  }



/* 
*     		Allocate major number.
*     		I'm just too lazy to perform dynamic allocation so
*     		the major number is fixed to apppi_major_number 
*/
  result = register_chrdev(apppi_major_number, apppi_name, &apppi_fops);
  if (result < 0) {
    printk("<1>apppi: cannot get major %d", apppi_major_number);
    return(result);
  }

 
  printk("<1>apppi: ********** Driver successfully initialized.\n");

  return(0);
}

static void __exit apppi_exit_module(void)
{	
	int i;
   
        for(i=0;i<boardsInUse;i++) {
          release_region(boards[i],32);  
        } 
  					/* unregister major number */
  	unregister_chrdev(apppi_major_number, apppi_name); 


  	printk("<1>apppi: Driver unloaded --------------> \n");
}




int apppi_open(struct inode *inode, struct file *filp) {
 
  int minor;

  minor = MINOR(inode->i_rdev);
  printk("<1>apppi: camera/board %d open\n",minor);
  outb( 0x034 , boards[minor] + 0x402);

  return(0);
}




int apppi_release(struct inode *inode, struct file *filp) {

  printk("<1>apppi: release\n");

  return(0);

  
}


static ssize_t apppi_read(struct file * file, char * buf, 
			  size_t count, loff_t *ppos) {

  char kerbuf[1];
 
  if(count != 1) {
    printk("<1>apppi: only 1 byte readouts are supported!\n");
    return(-1);
  }

  copy_to_user(buf, kerbuf, 1);
  return(count);
}


static ssize_t apppi_write(struct file * file, const char * buf, 
			   size_t count, loff_t *ppos) {

  /* we assume we write 1 byte -> count always = 1 */
  /*  unsigned char data_to_write; */

  if(count != 1) {
    printk("<1>apppi: only 1 byte writeouts are supported!\n");
    return(-1);
  }

/*    printk("<1>apppi: write, data_to_write = %d \n", data_to_write); */

  return(count);
}


int apppi_ioctl(struct inode *inode, 
	        struct file *filp, 
	        unsigned int cmd, 
	        unsigned long arg) {
  u32 read32;
  struct apIOparam request;
  int minor;
  unsigned char Val1, Val2;

  minor = MINOR(inode->i_rdev);

  switch(cmd) {
  case APISA_READ_USHORT:
    							/* Read hardware status register */
    {
    unsigned int *udat;
    int port, bp2, i;
    copy_from_user(&request,(struct ioparam *)arg,sizeof(request));
    port=boards[minor]+request.reg;
    bp2 = boards[minor] + 2;
    udat=(unsigned int *)request.param1;
    outb( 0x0E , bp2);         				/* Get ready for register select */
    outb( ( offsets[minor] & 0xF0 ) + request.reg , boards[minor]);
    for (i = 0; i < request.param2; i++ )
        outb( 0x0F , bp2 ); 				/* Latch the previous value */

    outb( 0x0E , bp2 );         			/* unlatch */
/*    outb( 0x0C , bp2 );         			*** Leave in port in transfer mode */
/*    outb( 0xAC , bp2 );         			*** Set read mode for low byte */
    outb( 0xA8 , bp2 );
    for (i = 0; i < request.param2; i++ )
                outb( 0xA9 , bp2); 			/* Latch the previous value */

    Val1 = inw( (unsigned short) boards[minor] );       /* Read low byte */
    outb( 0xA8 , bp2 );         			/* Unlatch */
    outb( 0xA0 , bp2 );         			/* Set read mode for high byte */
    for (i = 0; i < request.param2; i++ )
                outb( 0xA1 , bp2 ); 			/* Latch the previous value */

    Val2 = inw( (unsigned short) boards[minor] );       /* Read high byte */
    outb( 0xA0 , bp2 );         			/* Unlatch */
/*    outb( 0xA4 , bp2 );         			*** Leave port as it was */
/*    outb( 0xAC , bp2 );         			*** Leave port as it was */
    read32 = ( Val2 << 8 ) + Val1;
    put_user(read32,udat);
/*     printk("<1>apppi: hw status register %d: 0x%x\n",request.reg, read32); */
    break;
    }
  case APISA_READ_LINE:
    			/* Read hardware status register */
    {
    read32 = 0;
    /*printk("<1>apppi: hw status register: 0x%x\n", read32);*/
    return(read32);
    }
  case APISA_WRITE_USHORT:
    			/* Write argument to hardware control register */
    {
    unsigned int dat; int port, bp2, i;
    copy_from_user(&request,(struct ioparam *)arg,sizeof(request));
    port=boards[minor]+request.reg;
    dat=request.param1;
    bp2 = boards[minor] + 2;
    outb( 0x0E , bp2);         				/* Get ready for register select */
    outb( ( offsets[minor] & 0xF0 ) + request.reg , boards[minor]);
    for (i = 0; i < request.param2; i++ )
        outb( 0x0F , bp2 ); 				/* Latch the previous value */

    outb( 0x0E , bp2 );         			/* unlatch */
    outb( 0x0C , bp2 );         			/* Set write mode for low byte */
    outb( LOBYTE( dat ) , boards[minor] ); 		/* Write low byte */
    for (i = 0; i < request.param2; i++ )
                outb( 0x0D , bp2 ); 			/* Latch the previous value */

    outb( 0x0C , bp2 );         			/* Unlatch */
    outb( 0x04 , bp2 );         			/* Set write mode for high byte */
    outb( HIBYTE( dat ), boards[minor] ); 		/* Write high byte */
    for (i = 0; i < request.param2; i++ )
                outb( 0x05 , bp2 ); 			/* Latch the previous value */

    outb( 0x04 , bp2 );         			/* Unlatch */
/*    printk("<1>apppi: hw reg %d = 0x%x\n",request.reg, dat);  */
    break;
    }
	

  default:
    return(-EINVAL);
  }

  return(0);
}
module_init (apppi_init_module);
module_exit (apppi_exit_module);


MODULE_INFO(vermagic,VERMAGIC_STRING);
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
#ifdef MODULE_LICENSE
MODULE_LICENSE("GPL");
#endif

