# Makefile for src/mod/irctree.mod/

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
irctree.o: irctree.c ../module.h ../../../src/main.h ../../../config.h \
 ../../../eggint.h ../../../lush.h ../../../src/lang.h \
 ../../../src/eggdrop.h ../../../src/compat/in6.h ../../../src/flags.h \
 ../../../src/cmdt.h ../../../src/tclegg.h ../../../src/tclhash.h \
 ../../../src/chan.h ../../../src/users.h ../../../src/compat/compat.h \
 ../../../src/compat/snprintf.h ../../../src/compat/strlcpy.h \
 ../modvals.h ../../../src/tandem.h ../server.mod/server.h irctree.h
