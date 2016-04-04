db.d: ../../include/dbTable.h 
../../include/dbTable.h: dbTable.h
	@rm -f ../../include/dbTable.h
	@ln -fs ../src/db/dbTable.h ../../include/dbTable.h
