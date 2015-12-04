#ifndef _HV_DEFINE_H_
#define _HV_DEFINE_H_

#include "public.h"
#include "hv_zf_c.h"
#include "hv_fzf_c.h"
#include "hv_uniontab_c.h"
#include "hv_fd123.h"

#ifndef TLRLENGTH
#define TLRLENGTH 6
#endif

#ifndef BRNO_LEN
#define BRNO_LEN 7
#endif

#ifndef HV_BR_NO
#define HV_BR_NO "00000"
#endif

#ifndef HV_CZ_QSBRNO
#define HV_CZ_QSBRNO    "10001"    /**清算中心机构号**/
#endif

#ifndef HV_CUNZHEN_BR_NO
#define HV_CUNZHEN_BR_NO "9999" /**村镇银行的头4位**/
#endif

#ifndef HV_QS_BR_NO
#define HV_QS_BR_NO "313175000011"
#endif

#ifndef HV_ORDERNO_IND
#define HV_ORDERNO_IND "99"         /****大额支付交易序号****/ 
#endif

#ifndef HV_REFNO_IND
#define HV_REFNO_IND "98"           /****大额支付交易报文参考号****/  
#endif

#ifndef HV_ADDID_IND
#define HV_ADDID_IND "97"           /****大额附加域序号****/
#endif

#ifndef MBFE_FZF_IND
#define MBFE_FZF_IND "92"           /****大额附加域序号****/
#endif

#ifndef MBFE_FZF_BR_NO
#define MBFE_FZF_BR_NO TOT_BR_NO
#endif

#ifndef HP_CENTER
#define HP_CENTER "1234"
#endif

#ifndef MAX_803_REC
#define MAX_803_REC 30
#endif
/** add by LiuHuafeng 汇票帐号参数序号 ***/
#ifndef PARM_HPQF          /** 2440401机构汇票签发帐号 */
#define PARM_HPQF 1
#endif

#ifndef PARM_HPYC          /** 2440402机构汇票移存帐号 */
#define PARM_HPYC 2
#endif

#ifndef PARM_HPTH          /** 2440403机构汇票逾期退回帐号 */
#define PARM_HPTH 3
#endif

#ifndef PARM_HPGZ          /** 26106机构汇票挂帐帐号 */
#define PARM_HPGZ 4
#endif

#ifndef PARM_HVQS          /** MBFE 110科目专用账号 */
#define PARM_HVQS 5
#endif

#ifndef PARM_HVGZ          /** MBFE 26104来帐挂帐科目专用账号 */
#define PARM_HVGZ 6 
#endif

#ifndef PARM_HVCJ          /** 大额 2618拆借科目专用账号 */
#define PARM_HVCJ 7 
#endif

/*** move to lv_define.h 
#ifndef PARM_LVGZ          * BEPS  小额挂账专用账号 * 
#define PARM_LVGZ 8 
#endif
******************************************************/

#ifndef PARM_CZYH_QS_NO     /* BEPS  清算挂账专用账号 */
#define PARM_CZYH_QS_NO 12 
#endif


#ifndef HVLZGD          /** 大额 来帐过渡科目 */
#define HVLZGD  "41201"
#endif

#ifndef HVWZGD          /** 大额  往帐过渡科目 */
#define HVWZGD  "41101"
#endif


#define HV_PAYIN_FIX_BLOCK \
	char    F_crdb          [ 1];/**借贷标志0借1贷**/ \
	char    F_opcd          [ 2];/**操作代码 以小额为主*/ \
	char    F_lw_ind        [ 1]; /* 来往标识 */ \
	char    F_tx_br_no      [ 7]; /* 交易机构 */ \
	char    F_tx_date       [ 8]; /*交易日期 */ \
	char    F_wt_date       [ 8] ;/**委托日期       **/ \
	char    F_orderno       [ 8];/**支付交易序号   **/  \
	char    F_cmtno         [ 3];/**cmt编号    **/ \
	char    F_cur_no        [ 3]; /* 货币种类 */ \
	char    F_note_type     [ 3];/**凭证种类 **/ \
	char    F_note_no       [16];/**凭证号码 **/ \
	char    F_tx_amt        [16];/**交易金额   **/ \
	char    F_tx_chrg_ind   [ 1]; /**手续费标志 **/ \
	char    F_tx_chrg_amt   [15];/**手续费金额     **/ \
	char    F_or_br_no      [12];/**发起行行号     **/  \
	char    F_ac_br_no      [12];/**接收行行号     **/  \
	char    F_pay_opn_br_no [12];/**付款人开户行   **/  \
	char    F_pay_ac_no     [32];/**付款人帐号     **/  \
	char    F_pay_name      [60];/**付款人名称     **/  \
	char    F_pay_addr      [60];/**付款人地址     **/  \
	char    F_userpswd      [ 8];/**储户密码       **/  \
	char    F_cash_opn_br_no[12];/**收款人开户行行号**/ \
	char    F_cash_ac_no    [32];/**收款人帐号     **/  \
	char    F_cash_name     [60];/**收款人名称     **/  \
	char    F_cash_addr     [60];/**收款人地址     **/  \
	char    F_yw_type       [ 2];/** 业务种类 **/ \
	char    F_brf           [60];/**附言           **/ \
	char    F_operlevel     [1 ];/**优先级         **/ \
	char    F_tel       [TLRLENGTH];/**操作员号   **/ \
	char    F_chk       [TLRLENGTH];/**复核操作员号   **/ \
	char    F_auth_tel  [TLRLENGTH];/**授权操作员号   **/ \
	char    F_auth_pwd  [8]        ;/**授权操作员密码 **/ \
	char    F_refno     [20]; /* 报文参考号 */            \
	/**以上字段构成标准输入部分，后面的内容随交易不同而选择性赋值**/ \
	char    F_o_tx_date [ 8];/* 原交易日期 */ \
	char    F_o_cmtno   [ 3];/**原CMT编号**/ \
	char    F_hv_brf    [60];/**附言***/     \
	char    F_o_wt_date [ 8];/**原委托日期    **/ \
	char    F_o_orderno [ 8];/**原支付交易序号**/ \
	char    F_beg_sts   [ 1];/* 原支付交易状态 */ \
	char    F_hv_osts   [1 ]; /* 退汇交易用到的原支付来帐交易状态 */ \
	char    F_resp1     [ 8];/**ccpc应答码    **/ 
typedef struct {
    HV_PAYIN_FIX_BLOCK
}HV_PAYIN_FIX_AREA;

#define HV_PAYIN_TABLE_BLOCK \
	char   T_tx_br_no     [BRNO_LEN]; /* 交易行行号*/  \
	char   T_tx_date         [8 ]  ; /* 交易日期*/    \
	char   T_wt_date         [8 ]  ; /* 委托日期*/    \
	char   T_tx_time         [6 ]  ; /* 交易时间*/    \
	char   T_orderno         [8 ]; /* 人行的交易序号*/ \
	char   T_cur_no          [3 ]; /* 货币符号*/  \
	char   T_note_type       [3 ]; /* 凭证类型 */ \
	char   T_note_no         [16]; /* 凭证号 */   \
	char   T_tx_amt          [16]; /* 交易金额 */ \
	char   T_tx_chrg_ind     [1 ]; /* 手续费标志 0.现金 1.现金加急 2.转帐  \
                                  3.转帐加急 4.不收*/ \
	char   T_chrg_amt        [16]; /* 手续费 */ \
	char   T_pay_qs_no       [12]; /* 发起清算行行号*/ \
	char   T_or_br_no        [12]; /* 发起行行号--退汇交易的原接收行 */ \
	char   T_pay_opn_br_no   [12]; /* 付款人开户行行号 */	\
	char   T_pay_ac_no       [32]; /* 付款人帐号 */	\
	char   T_pay_name        [60]; /* 付款人姓名-退汇交易的原收款人户名*/ \
	char   T_pay_addr        [60]; /* 付款人地址 */                     \
	char   T_cash_qs_no      [12]; /* 接收行清算行号 */                 \
	char   T_ac_br_no        [12]; /* 接收行行号--退汇交易的原发起行 */ \
	char   T_userpswd        [8 ]; \
	char   T_cash_opn_br_no  [12]; /* 收款人开户行行号 */ \
	char   T_cash_ac_no      [32]; /* 收款人帐号       */ \
	char   T_cash_name       [60]; /* 收款人姓名-退汇交易的原付款人户名*/ \
	char   T_cash_addr       [60]; /* 收款人地址   */  \
	char   T_yw_type         [2 ]; /* 业务种类     */  \
	char   T_sendco          [4 ]; /* 发报中心代码 */  \
	char   T_receco          [4 ]; /* 收报中心代码 */  \
	char   T_cmtno           [3 ]; /* CMT编号      */  \
	char   T_beg_sts         [1 ]; /* 存放stat原始状态用于时候监督用 */ \
	char   T_operlevel       [1 ]; /* 优先级别0-一般1-紧急       \
                                  2-特急[救灾战备款等支付]*/ \
	char   T_hv_sts          [1 ]; /* 状态 请参阅hv_define.h程序 */ \
	char   T_tel             [6 ]; /* 操作员 */ \
	char   T_trace_no        [6 ]  ; /* 流水号*/      \
	char   T_hv_osts         [1 ]; /* 退汇交易用到的原支付来帐交易状态 */ \
	char   T_checkflag       [1 ]; /* 对帐检查标志*/ \
	char   T_hv_prt_ind      [10]; /* 打印标志，0未打印，1已打印*/ \
	char   T_resend_ind      [1 ]; /* 拒绝重发标志0未重新发送，  \
                                  1表示已经重新发送旧数据;   \
                                  2，表示手工退回支行，      \
                                  M;表示重新发送的新数据  */ \
	char   T_resend_date     [8 ]; /* 拒绝重新发送日期 */ \
	char   T_o_orderno       [8 ]; /* 原支付交易序号--退汇交易用的 */ \
	char   T_o_tx_date       [8 ]; /* 原支付交易日期--退汇交易用的 */ \
	char   T_o_cmtno         [3 ]; /* 原cmtno--退汇交易用的 */ \
	char   T_lw_ind          [1 ]; /* 来往标识 1-往 2-来 */    \
	char   T_addid           [8 ]; /* 附加域 id --addit表*/    \
	char   T_refno           [20]; /* 报文参考号 */            \
	char   T_auth_tel        [6 ]; /* 授权操作员*/             \
	char   T_auth_trace_no   [8 ]; /* 授权操作员流水号*/       \
	char   T_chk             [6 ]; /* 复核员 */ \
	char   T_chk_trace_no    [6 ]; /* 复核操作员流水号*/       \
	char   T_dealerr_date    [8 ]; /* 错帐处理日期*/           \
	char   T_dealerr_tel     [6 ]; /* 错帐处理柜员*/           \
	char   T_resp1           [8 ]; /* 应答码*/                 \
	char   T_o_wt_date       [8 ]; /*原委托日期*/         \
	char   T_send_tel     [TLRLENGTH];/* 发送操作员 */ \
	char   T_send_tel_trace_no[6]; /* 发送操作员流水号 */ \
	char   T_hv_brf          [60];  /* 备注 */

#define HV_PAYIN_AUTO_BLOCK \
	char A_tx_date       [8 ];/* 交易日期  */                   \
	char A_trace_no      [6 ];/**当前操作员流水号**/            \
	char A_sts           [1 ];/**状态;这个字段对来账有效，      \
                          在做检查时赋值，初值为正常接收**/ \
	char A_sendco        [4 ];/**发报中心**/                    \
	char A_pay_qs_ac_no  [12];/**发起清算行行号**/              \
	char A_receco        [4 ];/**收报中心**/                    \
	char A_cash_qs_ac_no [12];/**接收清算行**/                  \
	char A_dr_br_no  [BRNO_LEN];/* 支出账户所在机构 */          \
	char A_dr_acc_no     [7 ];/* 支出账户对应科目**/            \
	char A_avbal1        [15];/**走账前余额**/                  \
	char A_avbal2        [15];/**走账后余额**/                  \
	char A_dr_ac_ind     [1 ];/**帐号类型;为了节省来帐检查时间*/ \
	char A_tx_br_no  [BRNO_LEN];/**保存来帐算出的机构号****/


typedef struct {
	HV_PAYIN_FIX_BLOCK
	HV_PAYIN_TABLE_BLOCK
	HV_PAYIN_AUTO_BLOCK
}HV_PAYIN_AREA;

/******委托收款*******/
typedef struct{
	char  pj_date [8];
	char  pj_num  [8];
	char  note_type[2]; /* 凭证种类 add by LiuHuafeng */
}HV_WTSK_ADD;

/***托收承付********/
typedef struct{
	char	pj_date    [8];
	char	pj_num     [8];
	char	ptxamt   [15];
	char	jtxamt   [15];
}HV_TSCF_ADD;

/****银行同业拆借*****/
typedef struct{
	char  rate          [7];
	char  ratelimit     [5];
}HV_TYCJ_ADD;
/****银行汇票**********/
typedef struct{
	char	last_ac_no      [32];/****最后持票人帐号***/
	char	last_name       [60];/****最后持票人姓名***/
	char	pj_date         [8 ];/****票据日期***/
	char	pj_num          [8 ];/****票据号码***/
	char	hpmy            [10];/****汇票密押***/	
	char	cp_amt          [15];/****出票金额***/	
	char	dy_amt          [15];/****多余金额**/	
}HV_HP_ADD;

/******721报文附加域中需要用到***********/
typedef struct{
	char  pj_date         [8 ];/****票据日期****/
	char  pj_num          [8 ];/****票据号码****/
	char  hp_type         [1 ];/****汇票种类****/
	char  hp_miya         [10];/****汇票密押****/
	char  cp_amt          [15];/****出票金额****/
	char  real_js_amt  [15];/****实际结算金额*/
	char  dy_amt          [15];/****多余金额****/
	char  last_ac_no      [32];/****最后持票人帐号****/
	char  last_name       [60];/****最后持票人姓名****/
	char  return_date     [ 8];/****提示付款日期****/
	char  pay_opn_br_no  [12];/****付款人开户行,兑付行**/
	char  cash_opn_br_no  [12];/****收款人开户行,最后持票人开户行**/
}HV_721_ADD;

typedef struct{
	char  pj_date         [8 ];/****票据日期****/
	char  pj_num          [8 ];/****票据号码****/
	char  hp_miya         [10];/****汇票密押****/
	char  cp_amt          [15];/****出票金额****/
	char  real_js_amt  [15];/****实际结算金额*/
	char  cash_opn_br_no  [12];/****收款人开户行,最后持票人开户行**/
	char  last_ac_no      [32];/****最后持票人帐号****/
	char  last_name       [60];/****最后持票人姓名****/
	char  return_date     [ 8];/****提示付款日期****/
}HV_123_ADD;

typedef struct{
	char  spec_br_no		  [12];/****特许参与者,现金汇票汇兑行****/
	char  pj_date         [8 ];/****票据日期****/
	char  pj_num          [8 ];/****票据号码****/
	char  hp_type         [1 ];/****票据类型****/
	char  hp_miya         [10];/****汇票密押****/
}HV_121_ADD;


typedef struct{
	char  spec_br_no		  [12];/****特许参与者,现金汇票汇兑行****/
	char  pj_date         [8 ];/****票据日期****/
	char  pj_num          [8 ];/****票据号码****/
	char  hp_miya         [10];/****汇票密押****/
}HV_124_ADD;

/****CMT232报文涉及到的附加域**/
typedef struct{
	char tag_90A 		  [ 6];/*交换、轧差场次/交易批次*/
	char tag_CEB 		  [ 3];/*业务类型,即时转帐类型*/
	char tag_CF2		  [ 9];/*债券结算已匹配指令号*/
	char tag_CF1 		  [12];/*债券代码*/
	char tag_CNN 		  [15];/*净价金额*/
	char tag_CNP 		  [15];/*债券利息*/
 	char tag_CNM		  [15];/*债券面额*/
	char tag_CNQ		  [15];/*回购首期结算额*/
	char tag_CJ8		  [ 8];/*回购到期日*/
	char tag_CNR		  [15];/*回购利息*/
	char tag_90C          [ 1];/*借贷标识*/
	char tag_CCF          [ 4];/*被借记行所在CCPC代码*/
	char tag_CCG		  [ 4];/*被贷记行所在CCPC代码*/
}HV_232_ADD;

/******日间透支计息报文 803清单主表**BEG ADD BY ChenMing*****/
typedef struct{
		char	rate_time [ 6];   /*记息时点*/
		char	over_amt  [22];   /*透支金额*/
		char  rate	    [ 6];     /*利率*/
		char  over_intr [22];   /*透支利息*/ 
}CMT_803_RATE_INFO;

typedef struct{
    char tag_CD4		  [ 4];     /*计息时点数*/
    CMT_803_RATE_INFO tag_CLD[MAX_803_REC];
}HV_803_MAIN_ADD;		


typedef struct{
	char  pj_date         [8 ];/****票据日期****/
	char  pj_num          [8 ];/****票据号码****/
	char  hp_type         [1 ];/****汇票种类****/
	char  df_br_no       [12];/****原指定兑付行***/
	char  cp_amt          [15];/****出票金额****/
	char  hp_miya         [10];/****汇票密押****/
	char  pay_opn_br_no   [12];/***付款人开户行即签发行号***/
}HV_724_ADD;

typedef struct{
	char  pay_ac_no      [32];/****付款人帐号,汇票申请人帐号*****/
	char  pay_name        [60];/****付款人姓名,汇票申请人姓名*****/
	char  pay_addr        [60];/****付款人地址,汇票申请人地址*****/
	char  pj_date         [8 ];/****票据日期****/
	char  pj_num          [8 ];/****票据号码****/
	char  hp_miya         [10];/****汇票密押****/
	char  cp_amt          [15];/****出票金额****/
	char  real_js_amt  [15];/****实际结算金额*/
	char  cash_opn_br_no  [12];/****收款人开户行,最后持票人开户行**/
	char  last_ac_no      [32];/****最后持票人帐号****/
	char  last_name       [60];/****最后持票人姓名****/
	char  return_date     [ 8];/****提示付款日期****/
}HV_725_ADD;

/******收费802清单主表**BEG ADD BY LiuHuafeng*****/
typedef struct{
	char beg_date [8]; /* 计费开始日期 */
	char end_date [8]; /* 计费终止日期 */
	char zjcy_rate [6];/* 直接参与者收费比率 */
	char hb_rate[6];   /* 行别收费比率 */
	char ywl_rate[6];  /* 月累计业务量收费比率 */
	char zf_fee[15];   /* 支付类业务收费金额 */
	char zf_cnt [8];   /* 支付类业务收费记录个数 */ 
	char db_fee[15];   /* 单边业务收费金额 */
	char db_cnt [8];   /* 单边业务收费记录个数 */
	char xx_fee [15];  /* 信息类业务收费金额 */
	char xx_cnt [8];   /* 信息类业务收费记录个数 */
}HV_802_MAIN_ADD;
/******收费802清单主表**END ADD BY LiuHuafeng*****/
/******收费81x清单主表**BEG ADD BY Dongxy*****/
typedef struct{
	char beg_date [8]; /* 计费开始日期 */
	char end_date [8]; /* 计费终止日期 */
	char hv_amt	  [15];   /* 大额计费金额 */
	char lv_amt	  [15];   /* 小额计费金额 */
	char hv_cnt [5];   /* 参与者大额计费数目 */ 
	char lv_cnt [5];   /* 参与者小额计费数目 */
}HV_81X_MAIN_ADD;
/******收费81x清单主表**END ADD BY Dongxy*****/

#define HV_NOTPAY_FIX_BLOCK \
	char  F_opcd [ 2];/**操作代码 按照小额统一 */ \
	char  F_lw_ind    [ 1];/**来往标志 1 往 2 来**/ \
	char  F_tx_br_no  [ 7];/**交易机构**/ \
	char  F_tx_date  [ 8];/**非支付交易日期**/ \
	char  F_wt_date  [ 8];/**委托日期**/ \
	char  F_cmtno     [ 3];/**CMT 编号**/ \
	char  F_or_br_no  [12];/**发起行行号,查询行**/ \
	char  F_ac_br_no  [12];/**接受行行号,查复行,原查询行**/ \
	char  F_orderno	  [ 8];/**非支付交易序号**/ \
	char  F_actno     [32];/**非支付交易中涉及的帐号**/\
	char  F_name      [60];/**帐号户名**/ \
	char  F_content   [255];/**附言, 查询，查复，自由格式使用**/ \
	char  F_hv_brf    [60];/**附言***/ \
	char  F_tel       [TLRLENGTH];/**操作员**/ \
	char  F_respcode  [ 1];/**退回/撤销应答,0, 可以 1，不可以***/ \
	char  F_o_payin_orderno [ 8];/*原支付交易序号*/ \
	char  F_o_notpay_orderno[ 8];/*原非支付交易序号, 查复之原查询书号等应答类对应的申请交易的序号*/\
	char  F_o_cur_no        [ 3];/**原货币符号,查复等交易使用**/ \
	char  F_o_txamt         [15];/**支付交易金额,查复等使用**/ \
	char  F_o_tx_type       [ 1];/**原交易种类**/ \
	char  F_o_wt_date       [ 8];/**原委托日期,提供给对应的支付交易**/ \
	char  F_o_fzf_date      [ 8];/**原非支付交易日期,查复中用到的查询日期**/\
	char  F_o_ac_br_no      [12];/**原支付交易接受行号**/ \
	char  F_o_or_br_no      [12];/**原支付交易发起行号**/ \
	char  F_o_cmtno         [ 3];/**原CMT编号**/ \
	char  F_beg_sts         [1 ];/**原支付交易状态**/ \
	char  F_rcpstat         [2 ];/**业务回执状态**/ \
	char  F_refno           [20]; /* 报文参考号 */ \
	char  F_resp1           [8 ];/**ccpc应答码 */ \
	char  F_send_tel  	[6]; /*send_tel*/ \
	char  F_otype		[1];/**1.大额 2.小额**/ \
	char  F_hp_add_pwd    	[1];/* 汇票是否核押 */

#define	HV_NOTPAY_AUTO_BLOCK \
	char  A_tx_date  [ 8];/**交易日期**/ \
	char  A_trace_no [ 6];/**当前操作员流水号**/ \
	char  A_sts      [ 1];/**状态，这个字段对来帐有效, 在做检查的时候赋值，初始值为正常接受**/ \
	char  A_sendco        [ 4];/**发报中心**/ \
	char  A_receco        [ 4];/**收报中心**/ \
	char  A_pay_qs_ac_no  [12];/**发起清算行行号**/ \
	char  A_cash_qs_ac_no [12];/**接受清算行行号**/


#define	HV_NOTPAY_TABLE_BLOCK \
	char  T_or_br_no    [12];/**发起行行号**/ \
	char  T_orderno     [ 8];/**交易序号,查询书号，查复书号**/ \
	char  T_tx_date     [ 8];/**交易日期**/ \
	char  T_wt_date     [ 8];/**委托日期,查询查复日期**/ \
	char  T_br_no       [BRNO_LEN];/**交易行行号**/ \
	char  T_hv_fzf_sts  [ 1];/**非支付交易状态,录入、发送**/ \
	char  T_cmtno       [ 3];/**CMT编号**/  \
	char  T_tx_time     [ 6];/**交易时间**/ \
	char  T_trace_no    [ 6];/**交易流水**/ \
	char  T_pay_qs_no   [12];/**发起清算行号**/ \
	char  T_ac_br_no    [12];/**接受行行号**/ \
	char  T_cash_qs_no  [12];/**接受清算行号**/ \
	char  T_o_wt_date    [ 8];/**原委托日期(支付交易的委托日期)**/ \
	char  T_o_or_br_no  [12];/**原支付交易发起行**/ \
	char  T_o_ac_br_no  [12];/**原支付交易接受行**/ \
	char  T_o_payin_orderno[ 8];/**原支付交易交易序号**/ \
	char  T_o_cur_no    [ 3];/**对应支付交易的货币类型**/ \
	char  T_o_tx_amt    [16];/**对应支付交易的交易金额**/ \
	char  T_o_tx_type   [ 1];/**原交易种类 汇兑1 托收2 待定**/ \
	char  T_o_cmtno     [ 3];/**原CMT编号**/ \
	char  T_sendco      [ 4];/**发报中心代码**/ \
	char  T_receco      [ 4];/**收报中心代码**/ \
	char  T_req_date    [ 8];/**申请日期**/ \
	char  T_req_br_no   [12];/**申请行号**/ \
	char  T_req_content [255];/**附言,255位长度,查询,查复,自由格式**/ \
	char  T_req_orderno [ 8];/**申请序号,note原非支付交易序号填入该字段**/ \
	char  T_res_date    [ 8];/**应答日期**/ \
	char  T_res_br_no   [12];/**应答行号**/ \
	char  T_res_content [255];/**附言,255位长度,查询,查复,自由格式**/ \
	char  T_res_orderno [ 8];/**应答序号**/ \
	char  T_respcode    [ 1];/**退回/撤销应答,0, 可以 1，不可以***/ \
	char  T_addid       [ 8];/**附加域id,addit--数据表**/ \
	char  T_refno       [20];/**报文参考号**/ \
	char  T_hv_prt_ind  [10];/**打印标记,0未打印1已打印**/ \
	char  T_tel	    [6];/*操作员号*/ \
	char  T_hv_brf	    [60];/*备注,note这不是附言**/ \
	char  T_lw_ind      [1];/*来往标识*/ \
	char  T_o_trace_no  [6];/*old wssrno*/ \
	char  T_send_tel    [6];/*send_tel*/ \
	char  T_resp1       [8];/* ccpc返回的处理码 */ \
	char  T_otype	    [1];/**0.大额 1.小额**/
		
typedef struct{
	HV_NOTPAY_FIX_BLOCK
	HV_NOTPAY_AUTO_BLOCK
	HV_NOTPAY_TABLE_BLOCK
}HV_NOTPAY_AREA;

	
#ifndef HV_HD
#define HV_HD "100"
#endif

#ifndef HV_WT
#define HV_WT "101"
#endif

#ifndef HV_TSCF
#define HV_TSCF "102"
#endif

#ifndef HV_GKZJHH
#define HV_GKZJHH "103"
#endif

#ifndef HV_CJ
#define HV_CJ "105"
#endif

#ifndef HV_TH
#define HV_TH "108"
#endif

#ifndef HV_HPYC
#define HV_HPYC "121"
#endif

#ifndef HV_HP_QS_RECV
#define HV_HP_QS_RECV "122"
#endif

#ifndef HV_HP_PART_RETURN
#define HV_HP_PART_RETURN "123"
#endif

#ifndef HV_HP_ALL_RETURN
#define HV_HP_ALL_RETURN "124"
#endif

/************************************************
* 此文件包含有大额支付业务所需的一些常量定义    *
* Write by SSH,2004.9.8                         *
************************************************/
#ifndef HV_DEFINE_H
#define HV_DEFINE_H

#define HP_SERVERNAME1 "172.168.99.194"  /* 汇票密押服务器1地址 mod by zyl 20110701 for test last value is 66 */
#define HP_SERVERNAME2 "172.168.99.196"  /* 汇票密押服务器2地址 */
#define TIME_OUT 60  /* 等待时间 */
typedef struct STR_BaseSend {
	char cmtno   [3];
	char separator1;
	char txday   [8];
	char separator2;
	char brno    [BRNO_LEN];
	char separator3;
	char wssrno[6];
	char separator4;
	char orderno[8];
	char separator5;
    char resp1[8];
	char separator6;
	char nextline;
};
typedef struct STR_ChkSend {
	char cmtno  [3];
	char separator1;
	char orderno  [8];
	char separator2;
	char orbrno   [12];
	char separator3;
	char txday   [8];
	char separator4;
	char errmsg  [20];
	char separator5;
	char nextline;
};
#define MAXLINE 38
#define SEND_ACCT_CHECK 	'0'	/*往账记账(复合)*/
#define SEND_ACCT_WAIT 		'1'	/*往账发送(发送)*/
#define SEND_ACCT_QS		'2'	/*往账记账清算/本行发出查询已查复;*/
#define SEND_BACK_ACCT		'3'	/*往账退回记账;*/
#define SEND_BACK_WAIT		'4'	/*往账退回等待*/
#define SEND_REJECT		'5'	/*往账拒绝*/
#define SEND_CANCEL_ACCT	'6'	/*往账撤销记账*/	
#define SEND_CANCEL_WAIT	'7'	/*往账撤销等待*/
#define RECEIVE_REJECT		'8'	/*来账拒绝 -注意:日终核对用该状态表示来帐冲回*/
#define RECEIVE_ACCT_QS		'9'	/*来账记账清算*/
#define SEND_REVERSE_REG	'A'	/*往账登记取消*/
#define RECEIVE_BACK_ACCT	'B'	/*来账退回/它行发出查询已查复*/
#define SEND_REGIST		'C'	/*往帐登记(不记账)*/
#define SEND_REVERSE_ACCT	'D'	/*往账复核取消 不用了*/
#define RECEIVE_ACCT_REG	'E'	/*来账记账登记-来帐记大额临时科目,暂时不用*/
#define RECEIVE_LMAC_ERR	'F'	/*来账核地方押错,临时科目仍记账 --挂支行*/
#define RECEIVE_NMAC_ERR	'G'	/*来账核全国押错,临时科目仍记账 --挂支行*/
#define RECEIVE_DATA_ERR	'H'	/*来账内容不符，清算中心挂帐 -接收机构不存在*/
#define RECEIVE_CUSTNAME_ERR 'I' /*来账户名不符，但帐号不存在或户名不符或状态不不正常,挂帐*/
#define SEND_INQUEUE 		'J' /*往账排队中*/
#define CHECK_QS 			'K' /* 核对清算记帐 */
/**********以下为日终数据核对专用状态 *****************/
#define REJECT_EACTCLS 'L' /* 核对来往帐需要冲帐帐户状态错误或余额不足 清算中心挂账 */ 
/*********以下为数据核对比较结果状态 *******************/
#define NOTFOUND_CCPC_1      '1'  /* ccpc没有但银行成功走帐的成功冲帐状态情况 */
#define NOTFOUND_CCPC_2      '2'  /* ccpc没有但银行成功走帐的支行挂账冲帐状态情况 */
#define NOTFOUND_CCPC_3      '3'  /* ccpc没有但银行成功走帐的清算中心挂账冲帐状态情况 */
#define AMTDIFF_CCPC_BANK_1  '4'  /* 双方都走账但交易金额不同且成功冲帐的情况 */
#define AMTDIFF_CCPC_BANK_2  '5'  /* 双方都走账但交易金额不同且支行挂账冲帐的情况 */
#define AMTDIFF_CCPC_BANK_3  '6'  /* 双方都走账但交易金额不同且清算中心挂账冲账的情况 */
#define NOTFOUND_BANK_1      '7'  /* ccpc有但银行没有的情况--成功补计 */
#define NOTFOUND_BANK_2      '8'  /* ccpc有但银行没有的情况--支行挂账 */
#define NOTFOUND_BANK_3      '9'  /* ccpc有但银行没有的情况--清算中心挂账 */
#define CCPC_SAME_BANK       'A'  /* 银行和CCPC相同 */
#define NOTFOUND_CCPC        'B'  /* ccpc没有但银行成功走帐的情况 */
#define AMTDIFF_CCPC_BANK    'C'  /* 双方都走账但交易金额不同的情况 */
#define NOTFOUND_BANK        'D'  /* ccpc有但银行没有的情况 */
/**********************操作类型****************/
#define OP_QUERY	  '0'
#define OP_SEND		  '1'
#define HV_CZ_QSBRNO  "10001" /*清算中心行号*/
#define HV_HPCENTER   "904290099992" /*汇票处理中心*/
/*********************营业状态***************************/
#define HV_STAT_PREPARE		'0' 	/*营业准备*/
#define HV_STAT_WORK		'1'		/*日间处理*/
#define HV_STAT_WARN		'2'		/*截至准备*/
#define HV_STAT_STOP		'3'		/*业务截至*/
#define HV_STAT_QSWIN		'4'		/*清算窗口*/
#define HV_STAT_WORKEND		'5'		/*日终处理开始*/
#define HV_STAT_CHECKERR	'6'		/*日终对账不平*/
#define HV_STAT_CHECKRIGHT	'7'		/*日终对账已经平*/
#define HV_STAT_UNKNOWN		'F'		/*未知状态*/
#define HV_CHECK_MONEY     10000000 /* 电子联行最小授权金额 */
#define HV_HD_MIN_MONEY_INIT 10000000
extern  int HV_HD_MIN_MONEY;
#define HV_SYSPARA_POS  16
/*********************************************************/
#define   HV_EISBRNO    "040142"
#define   HV_QSBRNO HV_QS_BR_NO
#define   HV_SENDCO "1610"	/****TODO:影像测试****/
/**********************************************************/
/***********应答类型***************************************/
#define   HV_RP_AGR         '0'    /*同意应答*/
#define   HV_RP_DIS         '1'    /*不同意应答*/
/**********汇票处理状态************************************/
#define   HV_HP_REG         '0'    /* 汇票登记 */
#define   HV_HP_SIGN        '1'    /* 汇票签发(复合) */
#define   HV_HP_MV_CENTER   '2'    /* 资金移存记帐 */
#define   HV_HP_ASKCASH     '3'    /* 申请兑付 */
#define   HV_HP_CASH        '4'    /* 汇票兑付 */
#define   HV_HP_JQ          '5'    /* 全额结清 */
#define   HV_HP_PARTBACK    '6'    /* 部分退回 */
#define   HV_HP_FULLBACK    '7'    /* 未用退回 */
#define   HV_HP_TOCUST      '8'    /* 汇票正常解付 */
#define   HV_HP_UNSIGN      '9'    /* 汇票签发取消 */
#define   HV_HP_UNTOCUST    'A'    /* 解付取消 */
#define   HV_HP_SEND        'B'    /* 发送 */
#define   HV_HP_UNCASH      'C'    /* 申请兑付取消 --不需要这个状态 */
#define   HV_HP_UNREG       'D'    /* 汇票登记取消 */
#define   HV_HP_LOSS        'E'    /* 挂失 */
#define   HV_HP_UNLOSS      'F'    /* 解挂 -- 也不需要，一旦解挂就设置为上次状态*/
#define   HV_HP_OVERDATE_UNTREAD    'G'    /* 逾期未用退回 */
#define   HV_HP_UNUSED_TOCUST       'H'    /* 未用解付 */
#define   HV_HP_UNUSED_FULLBACK     'I'    /* 未用退回冲帐 */
#define   HV_HP_CANCEL              'J'    /* 汇票申请注销 */
#define   HV_HP_ASKRETURN           'K'    /* 申请退回 */
#define HV_POLEN     8    /* 汇票号长度 */
/************** 汇票类型 ********************/
#define ATTORNED_PO   '0'  /* 可转让的汇票 */
#define UNATTORNED_PO '1'  /* 不可转让的汇票 */
#define CASH_PO       '2'  /* 现金汇票 */
#endif



/***********     以下为原 mbfe_pub.h 中内容      *********/
/*********************   宏定义   ************************/
#ifndef MBFE_PUB
#define MBFE_PUB 

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

#endif

#endif


char * cTmpfp;
char * cTmpfp1; 
