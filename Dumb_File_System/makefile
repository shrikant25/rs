CC = gcc
FS_DIR = $(shell pwd)
INCLUDE_DIR = $(FS_DIR)/include

CFLAGS = -I$(INCLUDE_DIR) -O3 -lm -g

VDRUN_DISK_TARGET =  vdrun_disk
VDRUN_DISK_FILES  =  vdrun_disk.c write_to_buffer.c write_flags_to_disk.c vddriver.c getemptyblocks.c insertfile.c setbits.c fetch.c delete.c search.c get_tree_info.c write_metadata.c

VDCREATE_FILES = vdcreate.c write_to_buffer.c setbits.c vddriver.c write_flags_to_disk.c
VDCREATE_TARGET =  vdcreate


vdrun_disk: $(CC) $(VDRUN_DISK_FILES) -o $(VDRUN_DISK_TARGET) $(CFLAGS)
 
vdcreate:  $(CC) $(VDCREATE_FILES) -o $(VDCREATE_TARGET) $(CFLAGS)
	

vdcreate_tester: vdcreate_tester.c 
	gcc vdcreate_tester.c -O3 -o vdcreate_tester

test:
	gcc test.c -o test -O3

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
