
/*****************************************************************
 * Dave Mills 11-01-2001                                         *
 *                                                               *
 * Driver for Apogee Intstruments PCI CCD camera comtroller card *
 *                                                               *
 * Create device with:                                           *
 *     mknod /dev/appci c 60 0                                   *
 *     chmod og+rw /dev/appci   				 *
 *****************************************************************/

#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/major.h>
#include <linux/mm.h>
 
#include <linux/init.h>    
#include <linux/module.h>    
#include <linux/fs.h>    
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <linux/vermagic.h>
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

#include "plx9050.h"
#include "ApogeeLinux.h"

/* Version Information */
#define DRIVER_VERSION "v1.2"
#define DRIVER_AUTHOR "Dave Mills, rfactory@theriver.com"
#define DRIVER_DESC "Apogee PCI camera driver"

#if (LINUX_VERSION_CODE >= 0x20400)
static struct pci_device_id apogee_pci_tbl[] __devinitdata = {
        {
          vendor: PCI_VENDOR_ID_PLX,
          device: PCI_DEVICE_ID_PLX_9050,
          subvendor: PCI_ANY_ID,
          subdevice: PCI_ANY_ID,
        },
        { }                     /* Terminating entry */
};
MODULE_DEVICE_TABLE(pci, apogee_pci_tbl);
#endif


u32 m_BaseAddress;		/* pointer to register */
u32 m_PLXAddress;		/* pointer to register */
unsigned char *appci_virt_map_mem;  
				/* pointer to start of actual card information */
unsigned char *appci_virt_mem;  
				/* the next two are for base address 0	*/
unsigned char *appci_virt_map_mem0;
unsigned char *appci_virt_mem0;

/* 
*  variables related to the interrupt handler buffer and operations
*/ 
struct pci_dev *appci_dev = NULL;
static char appci_name[10]="appci"; /* device name in /proc/interrupts */
 
int 
appci_open(struct inode *, struct file *);
int appci_ioctl(struct inode *, struct file *, unsigned int, unsigned long);
static ssize_t appci_write(struct file * file, const char * buffer, 
			   size_t count, loff_t *ppos);
static ssize_t appci_read(struct file * file, char * buffer, 
			  size_t count, loff_t *ppos);
int appci_release(struct inode *, struct file *);


static struct file_operations appci_fops = {
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

	.read =		appci_read,
	.write =	appci_write,
	.ioctl =	appci_ioctl,
	.open =		appci_open,
	.release =	appci_release,
};

static unsigned int boardsInUse = 0;
#define MAXBOARDS 7     /* Increase this and the sizes of the
                           arrays below, if you need more.. */

/* static unsigned int boards[MAXBOARDS]={0,0,0,0,0,0,0}; */
static unsigned int boards[MAXBOARDS]={0,0,0,0,0,0,0};


static int __init appci_init_module(void) { 
  int result;
  int ii;
  unsigned int lbam, lbar;
  unsigned int PLXBAR0, PLXBAR1, PLXBAR2, APGread;
  unsigned int PLXlocal=0;
  int PLXrange;

  printk("<1>appci: ********** Initializing module...\n");

/* 
*    		Find pci device.
*/
  while ((appci_dev = pci_find_device(PCI_VENDOR_ID_PLX, PCI_DEVICE_ID_PLX_9050, appci_dev))!=NULL) {
     printk("<1>appci: Device found: bus %d, device %d, function %d\n", 
  	 appci_dev->bus->number,
         PCI_SLOT(appci_dev->devfn), PCI_FUNC(appci_dev->devfn));

/* 
* 		Get card configuration values. 
* 		Memory is located on PCI_BASE_ADDRESS_1 
*/
     pci_read_config_dword(appci_dev, PCI_BASE_ADDRESS_1, &PLXBAR1);
     PLXBAR1&=PCI_BASE_ADDRESS_IO_MASK; 
     printk("<1>appci: PLX configuration registers at 0x%08lx\n", (unsigned long) PLXBAR1);

/*     outl(PLX_CNTRL_RESET,      PLXBAR1+PLX_CNTRL);   */
/*     outl(PLX_CNTRL_EEPROM_RELOAD,PLXBAR1+PLX_CNTRL); */
     pci_write_config_dword(appci_dev,PCI_COMMAND,(unsigned long) 0x02800003);

     pci_write_config_dword(appci_dev,PCI_BASE_ADDRESS_2,(unsigned long) 0xffffffff);
     pci_read_config_dword(appci_dev, PCI_BASE_ADDRESS_2, &PLXrange);
     printk("<1>appci: PLX reg range is  0x%08lx\n", (unsigned long) ~PLXrange);
     pci_write_config_dword(appci_dev,PCI_BASE_ADDRESS_2,(unsigned long) 0xfeed0000 + boardsInUse*4096);
     lbar = ~PLXrange + 1;
     lbam = PLXlocal/lbar;
     outl(PLX_CHIP_SELECT_ENABLE | lbar | lbam*lbar*2,PLXBAR1+PLX_CS0BASE);

     outl(0x00000001 | PLXlocal,PLXBAR1+PLX_LAS0BA);
     outl(0x0fffff00,           PLXBAR1+PLX_LAS0RR);
     outl(PLX_LASBRD_BUS_32BIT, PLXBAR1+PLX_LAS0BRD);
     outl(0x00000000,           PLXBAR1+PLX_INITCSR);
     outl( PLX_CNTRL_BAR0_ISMEM | PLX_CNTRL_BAR1_ISIO | 
           PLX_CNTRL_PREFETCH   | PLX_CNTRL_WRT_NOWAIT , 
                                PLXBAR1+PLX_CNTRL);
 
     pci_write_config_dword(appci_dev,PCI_CACHE_LINE_SIZE,8);
     pci_read_config_dword(appci_dev, PCI_BASE_ADDRESS_0, &PLXBAR0);
     pci_read_config_dword(appci_dev, PCI_BASE_ADDRESS_1, &PLXBAR1);
     PLXBAR0&=PCI_BASE_ADDRESS_MEM_MASK; 
     PLXBAR1&=PCI_BASE_ADDRESS_IO_MASK; 
     printk("<1>appci: memory 0 starts at 0x%08lx\n", (unsigned long) PLXBAR0);
     printk("<1>appci: memory 1 starts at 0x%08lx\n", (unsigned long) PLXBAR1);

#ifdef DEBUG_APMOD
     for (ii=0;ii<0x40;ii=ii+4) {
        pci_read_config_dword(appci_dev, ii, &PCIreg);
         printk("<1>appci: PCI reg 0x%04x =  0x%08x\n",ii,PCIreg); 
     }
     for (ii=0;ii<0x54;ii=ii+4) {
         PLXin   = inl(PLXBAR1+ii);
         printk("<1>appci: PLX in 0x%04x  =  0x%08x\n",ii,PLXin); 
     }
#endif

    pci_read_config_dword(appci_dev, PCI_BASE_ADDRESS_2, &PLXBAR2);
    PLXBAR2&=(unsigned long)PCI_BASE_ADDRESS_MEM_MASK; 
    printk("<1>appci: memory 2 starts at 0x%08lx\n", (unsigned long) PLXBAR2);
    boards[boardsInUse] = (unsigned long)ioremap(PLXBAR2,256);  
    printk("<1>appci: memory 2 remapped to 0x%08lx\n", (unsigned long) boards[boardsInUse] );


    for (ii=0;ii<0x10;ii=ii+1) {
         APGread = readl(boards[boardsInUse]+ii*4);
         printk("<1>appci: APOGEE read 0x%04x  =  0x%08x\n",ii,APGread); 
    }

    boardsInUse++;
  }  

  if (boardsInUse == 0) {
        printk("<1>appci: No boards registered, module not loaded\n");
        return(-1);
  }


/*
*               Allocate major number.
*               I'm just too lazy to perform dynamic allocation so
*               the major number is fixed to appci_major_number
*/
  result = register_chrdev(appci_major_number, appci_name, &appci_fops);
  if (result < 0) {
    printk("<1>appci: cannot get major %d", appci_major_number);
    return(result);
  }

  
  printk("<1>appci: ********** Driver successfully initialized.\n");

  return(0);
}

static void __exit appci_exit_module(void)
{	
    int ii;

     for (ii=0;ii<boardsInUse;ii++) {
        iounmap((unsigned long *)boards[ii]);	
     }
  					/* unregister major number */
  	unregister_chrdev(appci_major_number, appci_name); 

  					/* Release mapped memory */

  	printk("<1>appci: Driver unloaded --------------> \n");
}




int appci_open(struct inode *inode, struct file *filp) {
 
  printk("<1>appci: open\n");

  return(0);
}




int appci_release(struct inode *inode, struct file *filp) {

  printk("<1>appci: release\n");

  return(0);

  
}


static ssize_t appci_read(struct file * file, char * buf, 
			  size_t count, loff_t *ppos) {

  char kerbuf[1];
 
  if(count != 1) {
    printk("<1>appci: only 1 byte readouts are supported!\n");
    return(-1);
  }

  copy_to_user(buf, kerbuf, 1);
  return(count);
}


static ssize_t appci_write(struct file * file, const char * buf, 
			   size_t count, loff_t *ppos) {

  /* we assume we write 1 byte -> count always = 1 */
  /*  unsigned char data_to_write; */

  if(count != 1) {
    printk("<1>appci: only 1 byte writeouts are supported!\n");
    return(-1);
  }

/*    printk("<1>appci: write, data_to_write = %d \n", data_to_write); */

  return(count);
}


int appci_ioctl(struct inode *inode, 
	        struct file *filp, 
	        unsigned int cmd, 
	        unsigned long arg) {
  u32 read32;
  struct apIOparam request;
  int minor;
    
  minor = MINOR(inode->i_rdev);
 

  switch(cmd) {
  case APPCI_READ_USHORT:
                        /* Read hardware status register */
    {
    unsigned long *udat;
    int port;
    copy_from_user(&request,(struct ioparam *)arg,sizeof(request));
    port=boards[minor]+request.reg;
    udat=(unsigned long *)request.param1;
    read32 = (int)readl(port);
    put_user(read32,udat);
/*     printk("<1>appci: port %x register %d: 0x%x\n",port,request.reg, read32);  */
    break;
    }
  case APPCI_READ_LINE:
                        /* Read hardware status register */
    {
    read32 = 0;
    /*printk("<1>appci: hw status register: 0x%x\n", read32);*/
    return(read32);
    }
  case APPCI_WRITE_USHORT:
                        /* Write argument to hardware control register */
    {
    unsigned int dat; 
    int port;
    copy_from_user(&request,(struct ioparam *)arg,sizeof(request));
    port=boards[minor]+request.reg;
    dat=request.param1;
    writel(dat,port);
/*    printk("<1>appci: hw reg %d = 0x%x\n",request.reg, dat); */
    break;
    }

	

  default:
    return(-EINVAL);
  }

  return(0);
}

/*
void fix_plx (pci_dev *pdev)
{
  unsigned int hwbase;
  unsigned long rebase;
  unsigned int t;

#define CNTRL_REG_OFFSET        0x50
#define CNTRL_REG_GOODVALUE     0x18260000

  pci_read_config_dword(pdev, PCI_BASE_ADDRESS_0, &hwbase);
  hwbase &= PCI_BASE_ADDRESS_MEM_MASK;
  rebase =  (ulong) ioremap(hwbase, 0x80);
  t = readl (rebase + CNTRL_REG_OFFSET);
  if (t != CNTRL_REG_GOODVALUE) {
    printk (KERN_DEBUG "appci: performing cntrl reg fix: %08x -> %08x\n",
            t, CNTRL_REG_GOODVALUE);
    writel (CNTRL_REG_GOODVALUE, rebase + CNTRL_REG_OFFSET);
  }
  my_iounmap (hwbase, rebase);
}
*/


module_init (appci_init_module);
module_exit (appci_exit_module);

MODULE_INFO(vermagic,VERMAGIC_STRING);
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
#ifdef MODULE_LICENSE
MODULE_LICENSE("GPL");
#endif


