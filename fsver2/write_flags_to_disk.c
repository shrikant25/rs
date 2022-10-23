int write_flags_todisk(unsigned int flg_datasz){
	
	char *chptr;
	unsigned int fd = open(DSKINF.diskname, O_WRONLY);
	flg_datasz /= 8;
	int i, j, k;
	printf("\nflags  %lx\n", flags[6]);
	printf("\nflags  %lx\n", flags[7]);
	printf("\nflags  %lx\n", flags[8]);
	printf("\n%u\n", flg_datasz);	
	if(fd){
		i = 1;
		k = 0;
		while(k<flg_datasz){

			memset(buffer, '\0', DSKINF.blksz);
			j = 0;
//			printf("here");
			while(j<DSKINF.blksz){
				chptr = (char *)&(flags[k]);
				buffer[j++] = *chptr++;
				buffer[j++] = *chptr++;
				buffer[j++] = *chptr++;
				buffer[j++] = *chptr++;
				buffer[j++] = *chptr++;
				buffer[j++] = *chptr++;
				buffer[j++] = *chptr++;
				buffer[j++] = *chptr;
				k++;
			}
			
			vdwrite(fd, buffer, i, DSKINF.blksz);
			i++;

		}
	printf("doe");
		close(fd);
		return 0;
	}
	return -1;
}	