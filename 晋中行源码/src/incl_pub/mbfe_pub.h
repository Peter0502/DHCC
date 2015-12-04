/*********************   头文件   ************************/
#include "mbfe_sys_ctl_c.h"
#include "mbfeserialno_c.h"
#include "mbfe_cmt_fmt_c.h"
#include "mbfe_cmt_tag_c.h"
#include "mbfebrbkno_c.h"
#include "mbfesabk_c.h"
#include "mbfesabksnd_c.h"
#include "mbfenetbank_c.h"
#include "mbfenetbanksnd_c.h"
#include "mbfeeisbank_c.h"
#include "mbfeeisbanksnd_c.h"
#include "mbfeghvsendjrn_c.h"
#include "mbfeghvsendlist_c.h"
#include "mbfe_errcode_c.h"
#include "mbfeghvrecvlist_c.h"
#include "mbfebillopnreg_c.h"
#include "mbfebillrtureg_c.h"
#include "mbfebillaplset_c.h"
#include "mbferecvcmtmsg_c.h"
#include "mbfesendcmtmsg_c.h"
#include "mbfequerymsg_c.h"
#include "mbfereplymsg_c.h"
#include "mbfereturnapp_c.h"
#include "mbfereturnrply_c.h"
#include "mbfefreeinfo_c.h"
#include "mbfeoprtcancel_c.h"
#include "mbfehppaycnt_c.h"
#include "mbfebksttalt_c.h"
#include "mbfebillmsgreg_c.h"
#include "mbfeodoprtqryrpl_c.h"
#include "mbfeoprtfeecl_c.h"
#include "mbfeoprtfeelt_c.h"
#include "mbfeoverdraftcl_c.h"
#include "mbfeoverdraftlt_c.h"
#include "mbferltxdclt_c.h"
#include "mbfeinpawnmsg_c.h"
#include "mbfeinpawnfina_c.h"
#include "mbfe_hang_c.h"

/*********************   宏定义   ************************/
#define MBFE_CMTNO 			"0160"
#define MBFE_PAYPRILEVEL 	"0160"
#define MBFE_COMSNO 		"0160"
#define MBFE_CNAPS_DATE 	"0160"
#define MBFE_CMT_LEN 		"0160"
#define MBFE_OPER_TYPE 		"0160"
#define MBFE_OPER_STSCODE 	"0160"
#define MBFE_RESPNO 		"0160"
#define MBFE_SEND_TIME 		"0160"
#define MBFE_CMT_FILE 		"0160"
#define MBFE_CMT_NO 		"0160"
#define MBFE_O_CMT_NO 		"0160"
#define MBFE_O_OPER_STSCODE "0160"
#define MBFE_DEF_TIMEOUT 	120

/*******定义跟踪日志宏定义 MBFEERRDEAL和MBFEWRITEMSG*********/ 
/* 大额支付出错处理宏定义 */
#define MBFEERRDEAL \
		strcpy(g_pub_tx.reply,"MBFE"); \
		set_zd_data(DC_GET_MSG,acErrMsg); \
		WRITEMSG \
		return -1;

#define MBFEWRITEMSG \
		strcpy(g_pub_tx.reply,"MBFE"); \
		set_zd_data(DC_GET_MSG,acErrMsg); \
		WRITEMSG

/*********************  函数声明  ************************/
