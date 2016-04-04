LIBPKGS  = db cmd util 
MAIN     = main
TESTMAIN = test

LIBS     = $(addprefix -l, $(LIBPKGS))
LIBFILES = $(addsuffix .a, $(addprefix lib, $(LIBPKGS)))

EXEC     = mydb
TESTEXEC = testdb

all: libs main

test: libs testmain
	
libs:
	@for lib in $(LIBPKGS); \
	do \
		echo "Checking $$lib..."; \
		cd src/$$lib; \
                make -f make.$$lib --no-print-directory PKGNAME=$$lib; \
		cd ../..; \
	done

main:
	@echo "Checking $(MAIN)..."
	@cd src/$(MAIN);  \
            make -f make.$(MAIN) --no-print-directory INCLIB="$(LIBS)" EXEC=$(EXEC);
	@ln -fs bin/$(EXEC) .
#	@strip bin/$(EXEC)

testmain:
	@echo "Checking $(TESTMAIN)..."
	@cd src/$(TESTMAIN); \
            make -f make.$(TESTMAIN) --no-print-directory INCLIB="$(LIBS)" EXEC=$(TESTEXEC);
	@ln -fs bin/$(TESTEXEC) .
#	@strip bin/$(TESTEXEC)

clean:
	@for lib in $(LIBPKGS); \
	do \
		echo "Cleaning $$lib..."; \
		cd src/$$lib; \
                make -f make.$$lib --no-print-directory PKGNAME=$$lib clean; \
                cd ../..; \
	done
	@echo "Cleaning $(MAIN)..."
	@cd src/$(MAIN); make -f make.$(MAIN) --no-print-directory clean
	@echo "Cleaning $(TESTMAIN)..."
	@cd src/$(TESTMAIN); make -f make.$(TESTMAIN) --no-print-directory clean
	@echo "Removing $(LIBFILES)..."
	@cd lib; rm -f $(LIBFILES)
	@echo "Removing $(EXEC)..."
	@rm -f bin/$(EXEC) 
	@echo "Removing $(TESTEXEC)..."
	@rm -f bin/$(TESTEXEC) 

ctags:          
	@rm -f src/tags
	@for lib in $(LIBPKGS); \
	do \
		echo "Tagging $$lib..."; \
		cd src; ctags -a $$lib/*.cpp $$lib/*.h; cd ..; \
	done
	@echo "Tagging $(MAIN)..."
	@cd src; ctags -a $(MAIN)/*.cpp
	@echo "Tagging $(TESTMAIN)..."
	@cd src; ctags -a $(TESTMAIN)/*.cpp

32:
	@cd src/cmd; ln -sf cmdReader-32.o cmdReader.o
	@cd ref; ln -sf $(EXEC)-32 $(EXEC)-ref

64:
	@cd src/cmd; ln -sf cmdReader-64.o cmdReader.o
	@cd ref; ln -sf $(EXEC)-64 $(EXEC)-ref

mac:
	@cd src/cmd; ln -sf cmdReader-mac.o cmdReader.o
	@cd ref; ln -sf $(EXEC)-mac $(EXEC)-ref
