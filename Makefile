all:
	make -C src all

uspd:
	make -C src uspd

test:
	make -C src test

unit-test: coverage
	make -C test/cmocka unit-test USPD_LIB_DIR=$(PWD)

coverage:
	make -C src coverage

clean:
	make -C src clean
	make -C test/cmocka clean
	rm -f uspd
	find -name '*.gcda' -exec rm {} -fv \;
	find -name '*.gcno' -exec rm {} -fv \;
	find -name '*.gcov' -exec rm {} -fv \;
	find -name '*.so' -exec rm {} -fv \;
	rm -f timestamp.log
	rm -f unit-test-coverage.xml
	rm -f functional-test-coverage.xml
	rm -f memory-report.xml
	rm -f api-result.log
	rm -f api-test-coverage.xml
	rm -rf report
	rm -f api-test-memory-report.xml
