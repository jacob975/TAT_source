
/*****************************************************************
 * Dave Mills 11-01-2001                                         *
 *                                                               *
 * Driver for Apogee Intstruments ISA CCD camera comtroller card *
 *                                                               *
 * Create device with:                                           *
 *     mknod /dev/apisa c 62 0                                   *
 *     chmod og+rw /dev/apisa   				 *
 *****************************************************************/
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/major.h>
#include <linux/mm.h>

#include <linux/module.h>    
#include <linux/fs.h>    
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <linux/pci.h>

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
 
#ifdef MODULE
MODULE_AUTHOR("Dave Mills (rfactory@theriver.com)");
MODULE_DESCRIPTION("Apogee CCD camera ISA Interface");
MODULE_LICENSE("GPL");
#endif

u32 m_BaseAddress;		/* pointer to register */
unsigned char *apisa_virt_map_mem;  
				/* pointer to start of actual card information */
unsigned char *apisa_virt_mem;  
				/* the next two are for base address 0	*/
unsigned char *apisa_virt_map_mem0;
unsigned char *apisa_virt_mem0;

/* 
*  variables related to the interrupt handler buffer and operations
*/ 
struct pci_dev *apisa_dev = NULL;
static char apisa_name[10]="apisa"; /* device name in /proc/interrupts */

int 
apisa_open(struct inode *, struct file *);
int apisa_ioctl(struct inode *, struct file *, unsigned int, unsigned long);
static ssize_t apisa_write(struct file * file, const char * buffer, 
			   size_t count, loff_t *ppos);
static ssize_t apisa_read(struct file * file, char * buffer, 
			  size_t count, loff_t *ppos);
static int apisa_release(struct inode *, struct file *);


struct file_operations apisa_fops = {
#if (LINUX_VERSION_CODE >= 0x20400)
  NULL,                 /* owner 2.4.x+ kernels */
#endif
  NULL,    		/* apisa_lseek,*/
  apisa_read,
  apisa_write,
  NULL,    		/* apisa_readdir */
  NULL,    		/* apisa_select */
  apisa_ioctl,
  NULL,    		/* apisa_mmap */
  apisa_open,
  NULL,
  apisa_release,
  NULL,			/* fsync */
  NULL,			/* fasync */
  NULL,			/* lock */
  			/* nothing more, fill with NULLs */
};

static unsigned int boardsInUse = 0;
static unsigned int apisa_addr[7] = {0,0,0,0,0,0,0};
#define MAXBOARDS 7     /* Increase this and the sizes of the
                           arrays below, if you need more.. */

static unsigned int bases[MAXBOARDS]={0x200,0x210,0x280,0x290,0x300,0x310,0x390};
static unsigned int boards[MAXBOARDS]={0,0,0,0,0,0,0};
#ifdef MODULE
MODULE_PARM(apisa_addr, "1-7i");
MODULE_PARM_DESC(apisa_addr, "Board io addresses (max of 7)");
#endif 

int init_module(void) { 
  int result;
  int i,j, ii;
  int baddr;
  int read32;

  printk("<1>apisa: ********** Initializing module...\n");

  for(i=0;i<MAXBOARDS;i++) {
     if (apisa_addr[i]) {
        baddr = 0;
        for(j=0;j<MAXBOARDS;j++) {
          if (apisa_addr[i] == bases[j]) {
             if(check_region(apisa_addr[i],32)<0)
             {
               printk("<1>apisa I/O region (0x%X) is already reserved by another device\n",apisa_addr[i]);
               continue;
             }
             boards[boardsInUse] = apisa_addr[i];
             boardsInUse++;
             baddr = 1;
             printk("<1>apisa: Board %d registered at 0x%x\n",i+1,apisa_addr[i]); 
             for (ii=0;ii<34;ii=ii+2) {
                read32 = inw(apisa_addr[i]+ii);
                printk("<1>apisa: reg %d = 0x%x\n",ii,read32); 
             }
          }
        }
        if (baddr == 0) { 
 	       printk("<1>apisa: Illegal port number for board %d , 0x%x\n",i+1,apisa_addr[i]);
        }
     }
  }


  if (boardsInUse == 0) { 
	printk("<1>apisa: No boards registered, module not loaded\n");
	return(-1);
  }

  for(i=0;i<boardsInUse;i++) {
      request_region(boards[i],16,"apisa");
  }



/* 
*     		Allocate major number.
*     		I'm just too lazy to perform dynamic allocation so
*     		the major number is fixed to apisa_major_number 
*/
  result = register_chrdev(apisa_major_number, apisa_name, &apisa_fops);
  if (result < 0) {
    printk("<1>apisa: cannot get major %d", apisa_major_number);
    return(result);
  }

 
  printk("<1>apisa: ********** Driver successfully initialized.\n");

  return(0);
}

void cleanup_module(void)  
{	
	int i;
   
        for(i=0;i<boardsInUse;i++) {
          release_region(boards[i],32);  
        } 
  					/* unregister major number */
  	unregister_chrdev(apisa_major_number, apisa_name); 

  					/* Release mapped memory */
/*  	vfree(apisa_virt_map_mem); */

  	printk("<1>apisa: Driver unloaded --------------> \n");
}




int apisa_open(struct inode *inode, struct file *filp) {
 
  int minor;

  minor = MINOR(inode->i_rdev);
  printk("<1>apisa: camera/board %d open\n",minor);

  MOD_INC_USE_COUNT;
  return(0);
}




int apisa_release(struct inode *inode, struct file *filp) {

  printk("<1>apisa: release\n");

  MOD_DEC_USE_COUNT;
  return(0);

  
}


static ssize_t apisa_read(struct file * file, char * buf, 
			  size_t count, loff_t *ppos) {

  char kerbuf[1];
 
  if(count != 1) {
    printk("<1>apisa: only 1 byte readouts are supported!\n");
    return(-1);
  }

  copy_to_user(buf, kerbuf, 1);
  return(count);
}


static ssize_t apisa_write(struct file * file, const char * buf, 
			   size_t count, loff_t *ppos) {

  /* we assume we write 1 byte -> count always = 1 */
  /*  unsigned char data_to_write; */

  if(count != 1) {
    printk("<1>apisa: only 1 byte writeouts are supported!\n");
    return(-1);
  }

/*    printk("<1>apisa: write, data_to_write = %d \n", data_to_write); */

  return(count);
}


int apisa_ioctl(struct inode *inode, 
	        struct file *filp, 
	        unsigned int cmd, 
	        unsigned long arg) {
  u32 read32;
  struct apIOparam request;
  int minor;

  minor = MINOR(inode->i_rdev);

  switch(cmd) {
  case APISA_READ_USHORT:
    			/* Read hardware status register */
    {
    unsigned long *udat;
    int port;
    copy_from_user(&request,(struct ioparam *)arg,sizeof(request));
    port=boards[minor]+request.reg;
    udat=(unsigned long *)request.param1;
    read32 = (int)inw(port);
    put_user(read32,udat);
/*     printk("<1>apisa: hw status register %d: 0x%x\n",request.reg, read32); */
    break;
    }
  case APISA_READ_LINE:
    			/* Read hardware status register */
    {
    read32 = 0;
    /*printk("<1>apisa: hw status register: 0x%x\n", read32);*/
    return(read32);
    }
  case APISA_WRITE_USHORT:
    			/* Write argument to hardware control register */
    {
    unsigned int dat; int port;
    copy_from_user(&request,(struct ioparam *)arg,sizeof(request));
    port=boards[minor]+request.reg;
    dat=request.param1;
    outw(dat,port);
/*    printk("<1>apisa: hw reg %d = 0x%x\n",request.reg, dat);  */
    break;
    }
	

  default:
    return(-EINVAL);
  }

  return(0);
}


