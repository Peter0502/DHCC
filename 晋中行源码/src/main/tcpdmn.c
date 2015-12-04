#include "find_debug.h"
/*
 *	函数名称		:	通讯程序
 *	作    者		:	
 *	日    期		:	
 *	功能描述		:	主机通讯程序
 *
 *	函数原型		:	tcpdmn( int argc , char * argv[] )
 *	入口参数		:
 *	出口参数		:
 *	返 回 值		:
 *
 *	修 改 人		:
 *	修改日期		:
 *	修改内容		:
 *
 */

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <varargs.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "tcpsvr.h"
#include <dlfcn.h>
#include <hvnet.h>

#define QUITPGM "trnull"

static 	int		Portno    = DEF_PORTNO_RC;
static 	int 	Timeout   = DEF_TIMEOUT;
static 	int 	ListenLen = DEF_LISTENLEN;

static int kill_daemon(void);

main(int argc, char *argv[])
{
	int 		i;
	int			ret;
	int			optlen, optvar;	
	int  		sockid, newsockid, childpid;
	unsigned long   clilen;
	char		fna[16];
	struct  	sockaddr_in  cli_addr, localaddr;
	char		Ip_Address[18];

	vtcp_log("TCP ID[%d] [%d]",sizeof(int),sizeof(long));
/*	vtcp_log("TCP ID[%d]",childpid);
*/
	strcpy ( Ip_Address , "" );

	for( i = 1 ; i < argc ; i++ ){
		if ( ! strcmp(argv[i],"-i") ){
				strncpy( Ip_Address , argv[++i] , sizeof( Ip_Address ) );
		}
		else if ( ! strcmp(argv[i],"-p") ){
				Portno = atoi( argv[++i] );
				}
		else if ( ! strcmp(argv[i],"-l") ){
				ListenLen = atoi( argv[++i] );
				}
		else if ( ! strcmp(argv[i],"-k") ){
				ret = kill_daemon();
				if (ret){
			vtcp_log("[%s][%d]  here,ip_address=[%s]",__FILE__,__LINE__,Ip_Address);
					fprintf(stdout,"\nKill TCPIP daemon failed!\n\n");
					}
				else{
			vtcp_log("[%s][%d]  here",__FILE__,__LINE__);
			vtcp_log("[%s][%d]  here,ip_address=[%s]",__FILE__,__LINE__,Ip_Address);
					fprintf(stdout,"\nAll TCPIP daemon have been killed!\n\n");
					}
				return 0;
		}else if(!strcmp(argv[i],"-run")){
			/****Add by SSH,2006.9.14,因为fork不好使,所以改为system调用****/
			/****子进程****/
			vtcp_log("[%s][%d]  here",__FILE__,__LINE__);
			vtcp_log("[%s][%d]  here,ip_address=[%s]",__FILE__,__LINE__,Ip_Address);
			tx_main(atoi(argv[i+1]),argv[i+2]);
			vtcp_log("[%s][%d]  here",__FILE__,__LINE__);
			vtcp_log("[%s][%d]  here,ip_address=[%s]",__FILE__,__LINE__,Ip_Address);
			close(atoi(argv[i+1]));
			vtcp_log("[%s][%d]  here",__FILE__,__LINE__);
			vtcp_log("[%s][%d]  here,ip_address=[%s]",__FILE__,__LINE__,Ip_Address);
			return(0);
		} else{
			vtcp_log("[%s][%d]  here",__FILE__,__LINE__);
			vtcp_log("[%s][%d]  here,ip_address=[%s]",__FILE__,__LINE__,Ip_Address);
			fprintf(stdout,"\nUsage: %s[-i Ip Address] [-p portno] [-l listenbuflen] [-k].\n\n", argv[0]);
			return 2;
			}
		}

	if (Portno   ==0) Portno    = DEF_PORTNO_RC;
	if (Timeout  ==0) Timeout   = DEF_TIMEOUT;
	if (ListenLen==0) ListenLen = DEF_LISTENLEN;

	if (getenv("LOGDIR")==NULL || getenv("FILDIR")==NULL ||
		getenv("BINDIR")==NULL ){
			vtcp_log("[%s][%d]  here",__FILE__,__LINE__);
			vtcp_log("[%s][%d]  here,ip_address=[%s]",__FILE__,__LINE__,Ip_Address);
		fprintf(stdout, "\nLOGDIR, FILDIR and BINDIR should be set.\n\n");
		return 1;
		}

	/* ignore the terminal stop signal */
	
	signal(SIGINT, SIG_IGN);
	signal(SIGPIPE, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);

# ifdef  SIGTTOU
	signal(SIGTTOU, SIG_IGN);
# endif

# ifdef  SIGTTIN
	signal(SIGTTIN, SIG_IGN);
# endif

# ifdef  SIGTSTP
	signal(SIGTSTP, SIG_IGN);
# endif

	if( ( childpid=fork() ) < 0 ){
			vtcp_log("[%s][%d]  here",__FILE__,__LINE__);
			vtcp_log("[%s][%d]  here,ip_address=[%s]",__FILE__,__LINE__,Ip_Address);
		vtcp_log("%s: Can't fork first child process!",argv[0]);
		return ( -1 );
		}
	else{
			/*vtcp_log("SON ID[%d]",childpid );*/
			vtcp_log("[%s][%d]  here",__FILE__,__LINE__);
			vtcp_log("[%s][%d]  here,ip_address=[%s]",__FILE__,__LINE__,Ip_Address);
		if( childpid > 0 )	{
			vtcp_log("[%s][%d]  here",__FILE__,__LINE__);
			vtcp_log("[%s][%d]  here,ip_address=[%s]",__FILE__,__LINE__,Ip_Address);
			return(0);
		}else{
		}
	}

	/* first child */

	if( setpgrp() == -1 ){
		vtcp_log("%s: Can't change process group!",argv[0]);
			vtcp_log("[%s][%d]  here",__FILE__,__LINE__);
			vtcp_log("[%s][%d]  here,ip_address=[%s]",__FILE__,__LINE__,Ip_Address);
		return ( -1 );
		}

	signal(SIGHUP, SIG_IGN);	/* immune from pgrp leader death */

	errno = 0;		/* probably got set to EBADF from a close */

	chdir( "/" );
	umask( 0 );

	signal( SIGCLD, SIG_IGN );

	/* create  stream  socket */

	if( (sockid = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		vtcp_log("%s: Creat socket failed!", argv[0]);
			vtcp_log("[%s][%d]  here",__FILE__,__LINE__);
			vtcp_log("[%s][%d]  here,ip_address=[%s]",__FILE__,__LINE__,Ip_Address);
		return ( -1 );
		}

	/* Allow socket option SO-REUSEADDR. */
	optlen = sizeof(optvar);
	optvar = 1;
	ret = setsockopt(sockid,SOL_SOCKET,SO_REUSEADDR,(char *)&optvar,optlen);
	if (ret == -1){ 
			vtcp_log("[%s][%d]  here",__FILE__,__LINE__);
			vtcp_log("[%s][%d]  here,ip_address=[%s]",__FILE__,__LINE__,Ip_Address);
		vtcp_log ("\n%s: setsockopt error!", argv[0]);
		return(-1);
		}

	/* bind out local address */

	MEMSET_DEBUG( (char *)&localaddr, 0, sizeof(localaddr));

	localaddr.sin_family = AF_INET;

	if ( Ip_Address[0] == NULL ){
			vtcp_log("[%s][%d]  here",__FILE__,__LINE__);
			vtcp_log("[%s][%d]  here,ip_address=[%s]",__FILE__,__LINE__,Ip_Address);
		localaddr.sin_addr.s_addr = htonl( INADDR_ANY );
		}
	else{
			vtcp_log("[%s][%d]  here",__FILE__,__LINE__);
			vtcp_log("[%s][%d]  here,ip_address=[%s]",__FILE__,__LINE__,Ip_Address);
		localaddr.sin_addr.s_addr = inet_addr( Ip_Address );
		if ( localaddr.sin_addr.s_addr == -1 ){
			vtcp_log("[%s][%d]  here",__FILE__,__LINE__);
			vtcp_log("[%s][%d]  here,ip_address=[%s]",__FILE__,__LINE__,Ip_Address);
			fprintf (stdout, "\nThe Ip Address Is Wrong!\n\n");
			close ( sockid );
			vtcp_log("[%s][%d]  here",__FILE__,__LINE__);
			vtcp_log("[%s][%d]  here,ip_address=[%s]",__FILE__,__LINE__,Ip_Address);
			return ( -1 );
			}
		}

	localaddr.sin_port = htons( Portno );

	if ( bind( sockid,( struct sockaddr * )&localaddr,sizeof(localaddr) ) < 0 ){
			vtcp_log("[%s][%d]  here",__FILE__,__LINE__);
			vtcp_log("[%s][%d]  here,localaddr=[%s]",__FILE__,__LINE__,localaddr);
		fprintf (stdout, "\nBind Ip Address failed!\n\n");
		close ( sockid );
		return ( -1 );
		}

	 /* Accept a connect request */

	if ( listen ( sockid, ListenLen ) < 0 ){
			vtcp_log("[%s][%d]  here",__FILE__,__LINE__);
			vtcp_log("[%s][%d]  here,sockid=[%d] listenlen=[%d]",__FILE__,__LINE__,sockid,ListenLen);
		fprintf ( stdout, "\nListen socket port failed!\n\n");
		close ( sockid );
		return ( -1 );
		}

		vtcp_log("[%s][%d]  here",__FILE__,__LINE__);
		vtcp_log("[%s][%d]  here,sockid=[%d] listenlen=[%d]",__FILE__,__LINE__,sockid,ListenLen);
	printl(stdout,2,"\n");
	vtcp_log("[%s][%d]  here std==[%s]",__FILE__,__LINE__,stdout);
	printl(stdout,2,"+");
	vtcp_log("[%s][%d]  here std==[%s]",__FILE__,__LINE__,stdout);
	printl(stdout,140,"-");
	vtcp_log("[%s][%d]  here std==[%s]",__FILE__,__LINE__,stdout);
	printl(stdout,2,"+");
	vtcp_log("[%s][%d]  here std==[%s]",__FILE__,__LINE__,stdout);
	printl(stdout,2,"\n");
	vtcp_log("[%s][%d]  here std==[%s]",__FILE__,__LINE__,stdout);
	printl(stdout,2,"|");
	vtcp_log("[%s][%d]  here std==[%s]",__FILE__,__LINE__,stdout);
	printl(stdout,50," ");
	vtcp_log("[%s][%d]  here std==[%s]",__FILE__,__LINE__,stdout);
	fprintf (stdout,"Tcp/Ip Daemon Started.");
	vtcp_log("[%s][%d]  here std==[%s]",__FILE__,__LINE__,stdout);
	printl(stdout,46," ");
	vtcp_log("[%s][%d]  here std==[%s]",__FILE__,__LINE__,stdout);
	printl(stdout,2,"|");
	vtcp_log("[%s][%d]  here std==[%s]",__FILE__,__LINE__,stdout);
	printl(stdout,2,"\n");
	vtcp_log("[%s][%d]  here std==[%s]",__FILE__,__LINE__,stdout);
	printl(stdout,2,"|");
	vtcp_log("[%s][%d]  here std==[%s]",__FILE__,__LINE__,stdout);
	printl(stdout,8," ");
	vtcp_log("[%s][%d]  here std==[%s]",__FILE__,__LINE__,stdout);
	fprintf (stdout,"Ip Address = %-18s Port = %-5d ListenBufLen = %-5d",inet_ntoa(*(struct in_addr *)&(localaddr.sin_addr.s_addr)),Portno, ListenLen);
	vtcp_log("[%s][%d]  here std==[%s]",__FILE__,__LINE__,stdout);
	printl(stdout,2," ");
	vtcp_log("[%s][%d]  here std==[%s]",__FILE__,__LINE__,stdout);
	printl(stdout,2,"|");
	vtcp_log("[%s][%d]  here std==[%s]",__FILE__,__LINE__,stdout);
	printl(stdout,2,"\n");
	vtcp_log("[%s][%d]  here std==[%s]",__FILE__,__LINE__,stdout);
	printl(stdout,2,"+");
	vtcp_log("[%s][%d]  here std==[%s]",__FILE__,__LINE__,stdout);
	printl(stdout,140,"-");
	vtcp_log("[%s][%d]  here std==[%s]",__FILE__,__LINE__,stdout);
	printl(stdout,2,"+");
	vtcp_log("[%s][%d]  here std==[%s]",__FILE__,__LINE__,stdout);
	printl(stdout,2,"\n");
	vtcp_log("[%s][%d]  here std==[%s]",__FILE__,__LINE__,stdout);
	printl(stdout,2,"\n");
	vtcp_log("[%s][%d]  here std==[%s]",__FILE__,__LINE__,stdout);
	printf("%s,%d,Gethere!\n",__FILE__,__LINE__);
	vtcp_log("[%s][%d]  here std==[%s]",__FILE__,__LINE__,stdout);
	while ( 1 ){
			vtcp_log("[%s][%d]  here",__FILE__,__LINE__);
		MEMSET_DEBUG( (char *)&cli_addr, 0x00, sizeof(cli_addr) );
			vtcp_log("[%s][%d]  here",__FILE__,__LINE__);
		clilen = sizeof(cli_addr);
			vtcp_log("[%s][%d]  here",__FILE__,__LINE__);

		newsockid = accept( sockid, ( struct sockaddr * ) &cli_addr, & clilen );
			vtcp_log("[%s][%d]  here",__FILE__,__LINE__);
		if ( newsockid < 0 ){
			vtcp_log("[%s][%d]  here",__FILE__,__LINE__);
			vtcp_log("Accept request failed.");	
			continue;
			}

  		sprintf( fna, "%s",
			inet_ntoa(*(struct in_addr *)&(cli_addr.sin_addr.s_addr)) );

		errno =0;
		/****Add by SSH,2006.9.14,因为fork不好使,所以改为system调用****/
		{
			char cmd[128];
			sprintf(cmd,"%s -run %d %s",argv[0],newsockid,fna);
			vtcp_log("[%s][%d]  here  cmd==[%s]",__FILE__,__LINE__,cmd);
			system(cmd);
		}
		close ( newsockid ); 	/* parent process */
		vtcp_log("[%s][%d]  here  newsockid==[%d]",__FILE__,__LINE__,newsockid);
	}	/* end socket manipulate */
}

static int kill_daemon(void)
{
	char fname[100];
	char cmd[100];
	FILE *fp;
	int  ret;

	tmpnam( fname );
	fp = fopen(fname,"w");
	if (fp==NULL) 
	{
		unlink(fname);
		return -1;
	}
			vtcp_log("[%s][%d]  here",__FILE__,__LINE__);
	
	fprintf(fp, "ps -aef | grep \"[-?].*tcpappsvr\" >/tmp/$$ 2>/dev/null");
	fprintf(fp, "\n");
	fprintf(fp, "while read a1 a2 a3");
	fprintf(fp, "\n");
	fprintf(fp, "do");
	fprintf(fp, "\n");
	fprintf(fp, "	kill -9 $a2 >/dev/null 2>&1");
	fprintf(fp, "\n");
	fprintf(fp, "done < /tmp/$$");
	fprintf(fp, "\n");
	fprintf(fp, "rm -f /tmp/$$");
	fprintf(fp, "\n");

	fclose( fp );

	sprintf(cmd, "sh %s", fname);
	ret = system( cmd );
	unlink( fname );

	return ret;
}

int printl(FILE * fp,int num, char * ch )
{
	register i;
			vtcp_log("[%s][%d]  here",__FILE__,__LINE__);

	for(i=1;i<=num/2;i++) fprintf(fp,ch);

	return (0);
}

int delblk(char *tmpstr)
{
	register i;


			vtcp_log("[%s][%d]  here",__FILE__,__LINE__);
	i=0;
	while(1) {
			vtcp_log("[%s][%d]  here",__FILE__,__LINE__);

		if(tmpstr[i]==' ') {tmpstr[i]=0; break;}
		i++;
	}


	return 0;

}
