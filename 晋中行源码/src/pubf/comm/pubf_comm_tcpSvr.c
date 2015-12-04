#include "find_debug.h" 
/*************************************************
* 文 件 名:    pubf_comm_tcpSvr.c
* 功能描述：
*
* 作    者:    jack
* 完成日期：   2003年12月16日
* 修改记录：
*     1. 日    期:
*        修 改 人:
*        修改内容:
*     2.
*************************************************/

/*
    1. receive server program name  : 16 bytes
    2. receive package length        : 8 bytes
    3. receive package                : package length bytes
    4. receive endflag                : 1 byte

    5. receive filename                : 30 bytes
    6. receive file length            : 8 bytes
    7. receive file                    : file length bytes
    8. receive endflag                : 1 byte
*/

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include "tcpsvr.h"



/*================ tcp read/write =========================*/

static int  tflag=0;         /* if timeout, tflag=1 */    

static void  catch_alrm(int a)
{
    vtcp_log ("Timeout.");
    tflag = 1;    
}

static void  set_alrm (int timeout)
{
    alarm (timeout);
}

static void  clear_alrm ()
{
    alarm (0);
}

int tcp_write ( int sockid, char *buf, int len, int timeout )
{
    int  left;
    int  nwrite;
    FILE *fp;

    left = len;
    while (left > 0)
    {
        nwrite = write (sockid, buf, left);
        if (nwrite < 0)
            return ( -1 );
        left -= nwrite;
        buf  += nwrite;
    }
    
    return ( 0 );
}

int tcp_read ( int sockid, char *buf, int len, int timeout )
{
    int  left, nread;

    signal ( SIGALRM, catch_alrm );

    left = len;
    while ( left > 0 ) 
    {
        set_alrm ( timeout );
        nread = read ( sockid, buf, left );

        clear_alrm ( );

        if (tflag == 1)
        {    /* timeout */
            tflag = 0;
            return ( -1 );
        }
        if ( nread <= 0 ) {
            vtcp_log ("TCPIP read failed.");
            return    ( -1 );
        }

        left -= nread;
        buf += nread;
    }

    return 0;
}

char *get_basename(char *str)
{
    int i;
    for(i=strlen(str)-1; i>=0; i--)
    {
        if (str[i]=='/')
        {
            break;
        }
    }
    return &str[i+1];
}

int TcpRcv( int sockid, char *pack, int *packlen, char *rcvfiles )
{
    char tmpstr[100];
    char fname[30+30+1], fbuf[512];
    int  flen;
    int  rc;
    int  h;

    if ( rcvfiles )
        strcpy( rcvfiles, "" );

    /* read package length : 8 bytes */
    MEMSET_DEBUG(tmpstr, 0, sizeof(tmpstr));
    rc = tcp_read( sockid, tmpstr, 8, DEF_TIMEOUT );
    if (rc) 
    { 
        vtcp_log("TCPIP Receive PackLen Failed!"); 
        return -1;
    }
    tmpstr[8] = 0;
    *packlen = atoi( tmpstr );

    /* read pack : packlen bytes */
    rc = tcp_read( sockid, pack, *packlen, DEF_TIMEOUT );
    if (rc) 
    { 
        vtcp_log("tCPIP Receive Pack Failed!"); 
        return (-1); 
    }
    
    /* read endflag and sndfiles */

    do
    {
        /* read endflag : 1 byte */
        MEMSET_DEBUG( tmpstr, 0, sizeof(tmpstr) );
        rc = tcp_read( sockid, tmpstr, 1, DEF_TIMEOUT );
        if (rc) 
        { 
            vtcp_log("TCPIP Receive EndFlag Failed!"); 
            return (-1); 
        }

        if ((unsigned char)tmpstr[0]!=0x7f && (unsigned char)tmpstr[0]!=0xff) 
        { 
            vtcp_log("TCPIP Receive EndFlag Invalid!"); 
            return (-1); 
        }
        if ((unsigned char)tmpstr[0]==0xff) break;

        /* read filename : 30 bytes */
        MEMSET_DEBUG( tmpstr, 0, sizeof(tmpstr) );
        rc = tcp_read( sockid, tmpstr, 30, DEF_TIMEOUT );
        if (rc) 
        { 
            vtcp_log("TCPIP Receive FileName Failed!"); 
            return (-1); 
        }

        strcpy( fname, getenv("FILDIR") );
        strcat( fname, "/" );
        strcat( fname, tmpstr );

        if ( rcvfiles )
        {
            if ( rcvfiles[0]!='\0' )
                strcat( rcvfiles, "+" );
            strcat( rcvfiles, tmpstr );
        }

        /* read file length : 8 bytes */
        rc = tcp_read( sockid, tmpstr, 8, DEF_TIMEOUT );
        if (rc) 
        { 
            vtcp_log("TCPIP Receive FileLen Failed!"); 
            return (-1); 
        }
        tmpstr[8] = 0;
        flen = atoi( tmpstr );

        /* read file : flen bytes */
        
        h = creat(fname, 0666);
        if (h<0) 
        {
            vtcp_log("Creat file [%s] failed!", fname);
            return (-2); 
        }

        while ( flen > 0 )
        {
            int tmplen;
            tmplen = ( flen<sizeof(fbuf) )? flen : sizeof(fbuf);
            rc = tcp_read( sockid, fbuf, tmplen, DEF_TIMEOUT );
            if (rc) 
            { 
                vtcp_log("TCPIP Receive File Failed!"); 
                return (-1); 
            }
            
            if (write(h, fbuf, tmplen) != tmplen )
            {
                vtcp_log("Write file [%s] failed!", fname); 
                return (-2);
            }
            flen -= tmplen;
        }
        close(h);
    } while (1);

    return 0;
}

int TcpSnd ( int sockid, char *pack, int packlen, char *sndfiles )
{
    int     rc;
    char     tmpstr[100];
    char    fname[100], fbuf[512];
    int     h;
    char    *p, *q;
    struct    stat statbuf;
    int        len;
    FILE    *fp;

    /* write packlen : 8 bytes */
    sprintf(tmpstr, "%08d", packlen);
    rc = tcp_write(sockid, tmpstr, 8, DEF_TIMEOUT);
    if (rc) 
    { 
        vtcp_log("TCPIP Send PackLen Failed!"); 
        return -1; 
    }

    /* write pack: packlen bytes */
    rc = tcp_write(sockid, pack, packlen, DEF_TIMEOUT);
    if (rc) 
    { 
        vtcp_log("TCPIP Send Pack Failed!"); 
        return -1; 
    }

    /* write endflag and sndfiles */

    p = sndfiles ;

    do
    {
        tmpstr[0] = ( p==NULL || *p=='\0' )? 0xff : 0x7f ;
        tmpstr[1] = '\0';

        /* write endflag : 1 byte */
        rc = tcp_write( sockid, tmpstr, 1, DEF_TIMEOUT );
        if (rc) 
        { 
            vtcp_log("TCPIP Write EndFlag Failed!"); 
            return -1; 
        }

        if ( p==NULL || *p==0 )    /* no more files need to send */
            break;

        MEMSET_DEBUG( fname, 0, sizeof(fname) ); 
        strcpy( fname, getenv("FILDIR") );
        strcat( fname, "/" );

        q = strchr(p, '+');
        if (q==NULL) q = p+strlen(p);
        strncat(fname, p, q-p);

        p = (*q=='\0')? q:q+1;

        if (stat( fname, &statbuf )!=0)
        {
            vtcp_log( "File [%s] not found to send!", fname );
            return -2;
        }
        /* write file name : 30 bytes */
        rc = tcp_write( sockid, get_basename(fname), 30, DEF_TIMEOUT );
        if (rc) 
        { 
            vtcp_log("TCPIP Send Filename Failed!"); 
            return -1; 
        }

        /* write file length : 8 bytes */
        sprintf(tmpstr, "%08d", statbuf.st_size);
        rc = tcp_write( sockid, tmpstr, 8, DEF_TIMEOUT );
        if (rc) 
        { 
            vtcp_log("TCPIP Send FileLen Failed!"); 
            return -1; 
        }

        /* write file */
        
        h = open(fname, O_RDONLY);
        if (h<0)
        {
            vtcp_log( "File [%s] can't be opened for read!", fname );
            return -2;
        }
        while ( (len = read(h, fbuf, sizeof(fbuf))) > 0 )
        {
            /* write a block of file : len bytes */
            rc = tcp_write( sockid, fbuf, len, DEF_TIMEOUT );
            if (rc)
            {
                vtcp_log("TCPIP Send File Failed!"); 
                close(h);
                return -1;
            }
        }
        close(h);
    } while (1);
    
    return 0;
}
