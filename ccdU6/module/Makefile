# 		 APPCI Kernel module free public release
# 
# 		       Copyright (C) 2001 The Random Factory
# 	         P.O. Box 44070, Tusson AZ 85733-4070 USA
# 		            All rights reserved
# 
#  $Id: Makefile,v 0.5.0 2001/11/01 08:34:41 rfactory Exp $
# 
#  



CC	= gcc -mcpu=i486 -pipe
AR	= ar
LD	= ld
RANLIB	= ranlib
MAKE	= make
LEX	= flex -l
YACC	= bison -y
MKDEP	= $(CC) -M
GROFF	= groff -man -t
PIC	= pic
EQN	= eqn
INSTALL = install -o root -g root -m 644

CFLAGS	= -Wall -I../../../include -I/lib/modules/`uname -r`/build/include
OFLAGS  = -fomit-frame-pointer -O2
UFLAGS	= -D__CONFIG__
DFLAGS	= 
GFLAGS	= -g -DDEBUG_APMOD

ARFLAGS	= rcs
TTFLAGS	= -Tlatin1 -P-b -P-u
PSFLAGS	= -Tps

KFLAGS  = -D__KERNEL__ -DMODULE

M_SRCS	= apogeeISA.c apogeePCI.c apogeePPI.c apogeeUSB.c
M_OBJS	= apogeeISA.o apogeePCI.o apogeePPI.o apogeeUSB.o

TARGET	= apogeeISA.o apogeePCI.o apogeePPI.o apogeeUSB.o

#adjust language of the installed manual page here (de or us)
MANU_L	= us

MANU_1	= apogee-modules.us.4
DOCU_1	= apogee-modules.us.txt
DOCU_2	= apogee-modules.us.ps


all:		module

module:		$(TARGET)


manual:		$(DOCU_1) $(DOCU_2)

$(DOCU_1):	$(MANU_1)
		cat $< | $(GROFF) $(TTFLAGS) > $@

$(DOCU_2):	$(MANU_1)
		cat $< | $(PIC) | $(EQN) | $(GROFF) $(PSFLAGS) > $@


.c.o:
		$(CC) $(CFLAGS) $(OFLAGS) $(KFLAGS) $(DFLAGS) \
		-o $@ -c $<

.c_g.o:
		$(CC) $(CFLAGS) $(OFLAGS) $(KFLAGS) $(DFLAGS) $(GFLAGS) \
		-o $@ -c $<

clean:
		-rm -f *~ *.orig *.o \
		$(DOCU_1) $(DOCU_2)  \
		$(TEST_1) *.cpio.gz *.tar.gz

install: 	all
		REL_NUM=`grep UTS_RELEASE /usr/include/linux/version.h | \
		  sed -e 's/.*\"\(.*\)\".*/\1/'`; \
		$(INSTALL) -d /lib/modules/$$REL_NUM/misc; \
		$(INSTALL) $(TARGET) /lib/modules/$$REL_NUM/misc; \
		$(INSTALL) $(TARG_G) /lib/modules/$$REL_NUM/misc
		$(INSTALL) $(MANU_1) /usr/man/man4
		rm -f /usr/man/man4/apogeePCI.4
		ln -s /usr/man/man4/$(MANU_2) /usr/man/man4/api.4;
		./MAKEDEV.apisa 0 all
		./MAKEDEV.apppi 0 all
		./MAKEDEV.appci 0 all


release:	clean
		@rm -f .system .depend; \
		cp -f ~/src/CVSROOT/CVSROOT/ChangeLog.appci ./ChangeLog; \
		if [ -d release ]; then \
		echo -n "please enter release number (e.g. 0.1.0): "; \
		read REL_NUM; \
		echo "release number is $$REL_NUM, creating tar archive ..."; \
		rm -rf apogee-modules-$$REL_NUM; \
		rm -f *.tar.gz; \
		mkdir apogee-modules-$$REL_NUM; \
		cp `find -type f -maxdepth 1` apogee-modules-$$REL_NUM; \
		cp release/[!C]* apogee-modules-$$REL_NUM; \
		cp patches/$(PATCH1) apogee-modules-$$REL_NUM; \
		cp patches/$(PATCH2) apogee-modules-$$REL_NUM; \
		tar cfz apogee-modules-$$REL_NUM.tar.gz apogee-modules-$$REL_NUM; \
		rm -rf apoge-modules-$$REL_NUM; fi

archive:	clean
		@rm -f .system .depend
		find -type f | grep -v versions | \
		cpio -o | gzip > apogee-modules-`date +%m%d`.cpio.gz


depend:		
		$(MKDEP) $(CFLAGS) $(KFLAGS) $(DFLAGS) \
		$(GFLAGS) $(M_SRCS) > .depend
		$(MKDEP) $(OFLAGS) $(UFLAGS) \
		$(GFLAGS) $(C_SRCS) >> .depend
		$(MKDEP) $(CFLAGS) $(OFLAGS)  \
		sed -e '/\.o:/s//_g.o:/' .depend >> .tmpdep; \
		cat .tmpdep >> .depend; rm .tmpdep

###-include .depend

