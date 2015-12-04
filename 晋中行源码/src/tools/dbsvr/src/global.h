#ifndef __GLOBAL_H__
#define __GLOBAL_H__
#define	INCLDIR "usr/ssh/incl"
#define	TABNDIR "usr/ssh"
#define	DBPCDIR "usr/ssh/src"
void  db_connect();
char *toupper(char *s);
char *tolower(char *s);
char *zip_space(char *s);
extern char acErrMsg[];
#define WRITEMSG \
        vtcp_log( "[%s,%d]|",__FILE__,__LINE__); \
        if ( strlen( acErrMsg ) ) \
	        vtcp_log( "[%s]\n", acErrMsg  ); \
		else \
	        vtcp_log( "[No message]\n"); \
        memset( (char *)acErrMsg , 0x00 ,128 ); 
#endif
