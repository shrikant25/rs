full_prg2: full_prg2.c getemptyblocks.c setbits.c insertfile.c  vdread.c vdwrite.c 
	gcc full_prg2.c getemptyblocks.c setbits.c insertfile.c vdread.c vdwrite.c -o full_prg2  -lm

vdcreate: vdcreate.c write_to_buffer.c setbits.c vddriver.c write_flags_to_disk.c
	gcc write_to_buffer.c vddriver.c setbits.c vdcreate.c write_flags_to_disk.c -o vdcreate -lm


vdcreate_tester: vdcreate_tester.c 
	gcc vdcreate_tester.c -o vdcreate_tester