#ifndef  HV_PUB_H
#define HV_PUB_H
#include "hvpack.h"
#define FLAG_SENDER 1			/*����*/
#define FLAG_RECEIVER 0			/*����*/
#define JOIN_FORENO  "916"		/*ֱ�Ӳ�����ǰ�û����*/


/*���ļ�����һЩͨ�õ��뽻���޹صĺ���*/
/*��������Ԫ��*/
int  findElement(int arr[],int element);
/*��֯ͨ��Ӧ����*/
int  hv_pack910(THVPACKAGE *pReqPack,char *pResCode,char *pAdd,THVPACKAGE *pResPack);
/*BASE64����ı��뺯��  */
int base64_encode(unsigned char *pSrcCode,unsigned char *pTgtCode,int nSrcLen);
/*BASE64����Ľ��뺯��*/
int base64_decode(unsigned char *pSrcCode,unsigned char *pTgtCode,int *nTgtLen);
/*�õ����ı�ʶ��*/
int hv_get_idtno();
/*�õ����Ĳο���*/
int hv_get_refno();
/*�õ�С������*/
int hv_get_ltxno();
/*�õ��������*/
int hv_get_htxno();
int hv_get_htxno_eis();/*��������ר��*/
/*�õ�����/�˻������*/
int hv_get_cancno();
/*�õ���ѯ���*/
int hv_get_queryno();
/*�õ��鸴���*/
int hv_get_callbackno();
/*��ָ���������ݴ�ŵ�һ���ļ��У��������ļ���*/
char* fcopy(char *pcFileName,THVPACKAGE *pHvPack,int flag);
char *DBC2SBC(char *DBCstr,int cplen);



#endif /*end HV_PUB_H*/

