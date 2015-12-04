#include <stdio.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>

/** in UnionSck.c **/


int UnionHSMCmd(int sckid,char *inbuf,int inlen,char *outbuf,long timeout)
{
	unsigned char aCmdBuf[3000];
	    char buf1[3000];
        
	int	 rc;
	memset(aCmdBuf,0,sizeof(aCmdBuf));
    memset(buf1,0,sizeof(buf1));
                
	aCmdBuf[0] = (inlen+8) / 256;
	aCmdBuf[1] = (inlen+8) % 256;
	strncpy(aCmdBuf+2,"12345678",8);
	memcpy(aCmdBuf+10,inbuf,inlen);
        /**Send command to HSM**/

	if ((rc = UnionSendToSocket(sckid,aCmdBuf,(inlen+ 10),timeout)) != (inlen + 10))
           {
		HsmErrLog("In UnionHSMCmd::UnionSendToSocket fail!");	
	    return(-1);
	    }

           /**Receive data from HSM**/
	if((rc = UnionReceiveFromSocket(sckid,buf1,3000,timeout))<0)
	   {
		HsmErrLog("In UnionHSMCmd::UnionReceiveFromSocket fail!");	
	    return(-1);
	    }
              
	memcpy(outbuf,buf1+10,(rc-10));
	/****
	printf("outbuf = %s\n",outbuf);
	****/
	
	return((rc-10));
        }  

