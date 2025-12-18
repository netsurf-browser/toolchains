# ...

.PHONY:	all tests-novapis tests-vapis clean

TESTS_NOVAPIS := Test1 Test readvarval
TESTS_VAPIS := TestVapis

all:
	$(foreach dir, ${TESTS_NOVAPIS} ${TESTS_VAPIS}, set -e; ${MAKE} -C ${dir}; )

tests-novapis:
	$(foreach dir, ${TESTS_NOVAPIS}, set -e; ${MAKE} -C ${dir}; )

tests-vapis:
	$(MAKE) -C ${TESTS_VAPIS}

clean:
	$(foreach dir, ${TESTS_NOVAPIS} ${TESTS_VAPIS}, ${MAKE} -C ${dir} clean; )
