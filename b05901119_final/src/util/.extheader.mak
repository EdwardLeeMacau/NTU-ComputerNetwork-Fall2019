util.d: ../../include/util.h ../../include/myUsage.h 
../../include/util.h: util.h
	@rm -f ../../include/util.h
	@ln -fs ../src/util/util.h ../../include/util.h
../../include/myUsage.h: myUsage.h
	@rm -f ../../include/myUsage.h
	@ln -fs ../src/util/myUsage.h ../../include/myUsage.h
