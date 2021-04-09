/*
		---------------------
		| Buffered File I/O |
		---------------------
*/
int IOFD,IOPOS;			/* file descriptor and buffer position */
char IOBUF[SECTORSZ];		/* buffer */
int IOMODE;			/* read or write */

#define FWRITE 1		/* IOMODE write option */
#define FREAD 0 		/* IOMODE read option */
#define FAPPEND 2		/* IOMODE append option */



/*
 *  Terminal Definition
 */
struct terminal *TERM;




/*
 *  Left Margin counter
 */
char MARGIN;		



/*
		--------------------------
		| Output Target Switches |
		--------------------------
*/
char HDCPYSW;		/* Switch to send output to hardcopy unit */
char FILESW;		/* Switch to send output to file */
char CRTSW;		/* Switch to send output to CRT */



char UNDERSW;		/* Switch indicates underlining in effect */




/*
		-----------------------------
		| Carriage Position Counter |
		-----------------------------
*/
int CARRIAGEPOS;
