#include "find_debug.h" 
/*************************************************
* �� �� ��: pubf_base_output
* ����������1�����������׷�ӵ��ļ���
* ��    ��: Seagle  
* ������ڣ�2006��06��14��
* �޸ļ�¼��    
* 1. ��    ��:
*    �� �� ��:
*    �޸�����:
*************************************************/

#include "stdio.h"
#include <varargs.h>
#include <sys/stat.h>
#include <sys/types.h>

#define EXTERN
#include "public.h"

/**********************************************************************
* ��������    pub_base_putmsg
* �������ܣ�  ��һ����׷�ӵ��ļ���
* ����/ʱ�䣺    
* 
* ������
*      msgend : ��������־
*      msgtype: �����ͱ�ʶ
*      msgdata: ������
*      msglen : ������
*           
* �� �� ֵ: >=0:�ɹ� <0:ʧ��
*           
* ˵��:  ƴ��֮��İ��ṹΪ[1λ������ʶ+5λ������+3λ������+������]
********************************************************************/
int pub_base_putmsg(char msgend,char *msgtype,char *msgdata,int msglen)
{
	char pcFileName[256];
	FILE *fp;
	pub_base_AllDwnFilName(pcFileName);
	fp=fopen(pcFileName,"a");
	if(fp==NULL){
		sprintf(acErrMsg,"%s.%d,���ļ�[%s]ʧ��!\n",__FILE__,__LINE__,pcFileName);
		WRITEMSG
		return(-1);
	}
	fprintf(fp,"%c%.5s%03d",msgend,msgtype,msglen+9);
	fwrite(msgdata,msglen,1,fp);
	fclose(fp);
	return(0);	
}
