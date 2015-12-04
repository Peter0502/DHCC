#ifndef  HV_PUB_H
#define HV_PUB_H
#include "hvpack.h"
#define FLAG_SENDER 1			/*往账*/
#define FLAG_RECEIVER 0			/*来账*/
#define JOIN_FORENO  "916"		/*直接参与者前置机编号*/


/*此文件声明一些通用的与交易无关的函数*/
/*查找数组元素*/
int  findElement(int arr[],int element);
/*组织通用应答报文*/
int  hv_pack910(THVPACKAGE *pReqPack,char *pResCode,char *pAdd,THVPACKAGE *pResPack);
/*BASE64编码的编码函数  */
int base64_encode(unsigned char *pSrcCode,unsigned char *pTgtCode,int nSrcLen);
/*BASE64编码的解码函数*/
int base64_decode(unsigned char *pSrcCode,unsigned char *pTgtCode,int *nTgtLen);
/*得到报文标识号*/
int hv_get_idtno();
/*得到报文参考号*/
int hv_get_refno();
/*得到小额交易序号*/
int hv_get_ltxno();
/*得到大额交易序号*/
int hv_get_htxno();
int hv_get_htxno_eis();/*电子联行专用*/
/*得到撤销/退回申请号*/
int hv_get_cancno();
/*得到查询书号*/
int hv_get_queryno();
/*得到查复书号*/
int hv_get_callbackno();
/*把指定报文数据存放到一个文件中，并返回文件名*/
char* fcopy(char *pcFileName,THVPACKAGE *pHvPack,int flag);
char *DBC2SBC(char *DBCstr,int cplen);



#endif /*end HV_PUB_H*/

