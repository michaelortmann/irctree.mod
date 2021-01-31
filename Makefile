# Makefile for src/mod/irctree.mod/
# $Id: Makefile,v 1.10 1999/12/15 02:33:00 guppy Exp $

doofus:
	@echo ""
	@echo "Let's try this from the right directory..."
	@echo ""
	@cd ../../../; make

static: ../irctree.o

modules: ../../../irctree.so

../irctree.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -DMAKING_MODS -c irctree.c
	rm -f ../irctree.o
	mv irctree.o ../

../../../irctree.so: ../irctree.o
	$(LD) -o ../../../irctree.so ../irctree.o
	$(STRIP) ../../../irctree.so

depend:
	$(CC) $(CFLAGS) $(CPPFLAGS) -MM *.c > .depend

clean:
	@rm -f .depend *.o *.so *~

#safety hash
../irctree.o: irctree.c ../module.h ../../../config.h ../../main.h \
 ../../lang.h ../../eggdrop.h ../../flags.h ../../proto.h \
 ../../../lush.h ../../cmdt.h ../../tclegg.h ../../tclhash.h \
 ../../chan.h ../../users.h ../modvals.h ../../tandem.h irctree.h
