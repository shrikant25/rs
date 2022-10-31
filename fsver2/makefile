vdrun_disk: vdrun_disk.c write_to_buffer.c write_flags_to_disk.c vddriver.c getemptyblocks.c insertfile.c setbits.c fetch.c delete.c search.c get_tree_info.c write_metadata.c
	gcc vdrun_disk.c write_to_buffer.c  vddriver.c write_flags_to_disk.c getemptyblocks.c insertfile.c setbits.c fetch.c delete.c search.c get_tree_info.c write_metadata.c -o vdrun_disk -lm

vdcreate: vdcreate.c write_to_buffer.c setbits.c vddriver.c write_flags_to_disk.c
	gcc write_to_buffer.c vddriver.c setbits.c vdcreate.c write_flags_to_disk.c -o vdcreate -lm


vdcreate_tester: vdcreate_tester.c 
	gcc vdcreate_tester.c -o vdcreate_tester