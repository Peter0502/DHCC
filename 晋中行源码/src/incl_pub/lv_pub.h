#ifndef _LV_PUB_H_
#define _LV_PUB_H_

#include "lvpack.h"
#include "errlog.h"
#include "lv_wbctl_c.h"
#include "lv_lbctl_c.h"
#include "lv_sendlist_c.h"
#include "lv_recvlist_c.h"
#include "lv_pkgreg_c.h"
#include "hv_fzf_c.h"
#include "lv_param_c.h"

#define FLAG_SENDER 1           /*Õ˘’À*/
#define FLAG_RECEIVER 0         /*¿¥’À*/

#ifndef DB_OK
	#define DB_OK 0
#endif

#define DB_NOTFOUND 100




int  lv_pack911(TLVPACKAGE *pReqPack,char *pResCode,char *pAdd,TLVPACKAGE *pResPack);
int  lv_pack920(TLVPACKAGE *pReqPack,char *pResCode,char *pAdd,TLVPACKAGE *pResPack);
int  findElement(int arr[],int element);
void rtrim(char *str);
char* fcopy(char *pcFileName,TLVPACKAGE *pLvPack,int flag);
int regerr(char *pcFileName);
#endif


