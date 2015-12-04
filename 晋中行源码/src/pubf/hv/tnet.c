#include"hvnet.h"
#include"hvini.h"
#include"stdlib.h"
#include "find_debug.h"
void main(int argc,char *argv[]){
	int iSocketID;
	int iRc;
	int i;
	char sendBuf[20480];
	for(i=0;i<10;i++){
		MEMSET_DEBUG(sendBuf+i*1001,i+'0',1000);
		sendBuf[(i+1)*1001-1]='\n';
	}
	sendBuf[10011]='\0';
	iRc=hv_initPara();
	hv_netOpen();
	if(argv[1][0]=='0'){
		/*¿Í»§¶Ë*/
		iSocketID=hv_connect(HV_SERVER);
		iRc=hv_send(iSocketID,sendBuf);
		printf("Client:iRc=%d\n",iRc);
	}else{
		MEMSET_DEBUG(sendBuf,'\0',sizeof(sendBuf));
		iSocketID=hv_accept();
		iRc=hv_recv(iSocketID,sendBuf);
		printf("Server:iRc=%d\n",iRc);
		printf("Get:\n%s\n",sendBuf);
	}
	hv_netClose(iSocketID);
}
