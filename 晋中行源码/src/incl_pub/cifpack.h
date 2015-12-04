#ifndef __CIFPACK_H__
#define __CIFPACK_H__
/****CIF报文结构定义****/
typedef struct{
	char blockhead[3];	/****/
	char length[6];		/**报文长度**/
	char use;			/**0**/
	char inout;			/**0-提出,1-提入**/
	char ywtype;		/**2-CIF**/
	char status[3];		/**000-成功,001-报文结构错误,其他失败****/
	char flgno[8];		/**报文标识号**/
	char refno[20];		/**报文参考号**/
	char sendtm[14];	/**发送时间**/
	char cifno[3];		/**报文类型,100-支票截留,101-通用截留***/
	char workdate[8];	/**工作日期**/
	char blocktail;		/**	}**/
}TCIFHEAD;
typedef struct{
	char blockhead[3];	/****/
	char buffer[1024*20];
}TCIFBODY;
typedef struct{
	TCIFHEAD head;
	TCIFBODY body;
}TCIFPACK;
void vInitCifPack(TCIFPACK *);
void vAddCifTag(TCIFPACK *pack,char *tagname,char *tagdata);
void vGetCifTag(TCIFPACK *pack,char *tagname,char *tagdata);
#endif
