vdrun_disk: vdrun_disk.c write_to_buffer.c write_flags_to_disk.c vddriver.c getemptyblocks.c insertfile.c setbits.c fetch.c delete.c search.c get_tree_info.c write_metadata.c
	gcc -O3 vdrun_disk.c write_to_buffer.c vddriver.c write_flags_to_disk.c getemptyblocks.c insertfile.c setbits.c fetch.c delete.c search.c get_tree_info.c write_metadata.c -o vdrun_disk -lm -g

vdcreate: vdcreate.c write_to_buffer.c setbits.c vddriver.c write_flags_to_disk.c
	gcc -O3 write_to_buffer.c vddriver.c setbits.c vdcreate.c write_flags_to_disk.c  -o vdcreate -lm


vdcreate_tester: vdcreate_tester.c 
	gcc vdcreate_tester.c -O3 -o vdcreate_tester

test:
	gcc test.c -o test

testdisk:

	make vdcreate
	make vdrun_disk
	make vdcreate_tester
	make test
	
	ls test_files > fl

	./vdcreate disk 32 10
	./vdcreate_tester disk > dump1
	./test fl 2010 1
	./vdrun_disk disk
	./vdcreate_tester disk > dump2
	./test fl 2010 2
	./vdrun_disk disk
	./vdcreate_tester disk > dump3
	./test fl 2010 3
	./vdrun_disk disk
	./vdcreate_tester disk > dump4

	rm vdcreate
	rm vdcreate_tester
	rm vdrun_disk
	rm disk
	rm test