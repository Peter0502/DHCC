#ifndef __CIFPACK_H__
#define __CIFPACK_H__
/****CIF���Ľṹ����****/
typedef struct{
	char blockhead[3];	/****/
	char length[6];		/**���ĳ���**/
	char use;			/**0**/
	char inout;			/**0-���,1-����**/
	char ywtype;		/**2-CIF**/
	char status[3];		/**000-�ɹ�,001-���Ľṹ����,����ʧ��****/
	char flgno[8];		/**���ı�ʶ��**/
	char refno[20];		/**���Ĳο���**/
	char sendtm[14];	/**����ʱ��**/
	char cifno[3];		/**��������,100-֧Ʊ����,101-ͨ�ý���***/
	char workdate[8];	/**��������**/
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
