#include bdscio.h
#define DEL1	0x1f	/* C-<underbar> */
#define DEL2	0x1e	/* C-<caret> */

main(argc,argv)
char **argv;
{
	char ibuf[BUFSIZ];
	char line[132];
	int ifd, on, lcount, i;
	on = i = lcount = 0;
	if (argc != 2) {
		printf("Usage:\nMlist infile outfile");
		exit();
	}

	if (fopen(argv[1],ibuf) == -1){
		printf ("File open error on %s",argv[1]);
		exit();
	}
	printf ("%s",argv[1]);
	while (fgets(line,ibuf) != 0){
		lcount++;
		if (DEL2 == line[0]){
			if (on) do {
				lcount++;
				fputs("     \n",2);
			}while (lcount <= 12);
			on = 0;
		}else if (DEL1 == line[0]) {
			on = 1;
			lcount = 0;
		}else if (on){
			fputs(line,2);
		}
	}
	fclose (ibuf);
}
