#ifndef ISO8583__FD123__H__
#define ISO8583__FD123__H__
		 

#define HV_FD123_AREA_BLOCK \
	char  cmtno           [ 3];/*3*cmt编号**/	\	
	char  o_cmtno         [ 3];/*6*原cmt编号**/\
	char  pay_qs_no       [12];/*18*(原)发起清算行行号**/		\
	char  or_br_no        [12];/*30*发起行号 原接受行号 		\
                           申请撤销行号 退回行号**/	\
	char  sendco          [ 4];/*34*发报中心代码**/\
	char  pay_opn_br_no   [12];/*46*付款人开户行**/\
	char  pay_ac_no       [32];/*78*付款人帐号(原收款人帐号)**/	\
	char  pay_name        [60];/*138*付款人名称(原收款人名称)**/	\
	char  pay_addr        [60];/*198*付款人地址(原收款人地址)**/	\
	char  cash_qs_no      [12];/*210*接受清算行号**/\
	char  ac_br_no        [12];/*222*接受行号(原发起行号)**/	\
	char  receco          [ 4];/*226*收报中心代码**/\
	char  cash_opn_br_no  [12];/*238*收款人开户行号**/      \
	char  cash_ac_no      [32];/*270*收款人帐号,原付款人帐号**/	\
	char  cash_name       [60];/*330*收款人姓名,原付款人姓名**/	\
	char  cash_addr       [60];/*390*收款人地址**/\
	char  yw_type         [ 2];/*392*业务种类**/	\
	char  orderno         [ 8];/*400交易序号 撤销支付交易序号*/	\
	char  brf             [60];/*460*附言**/            \
	char  rate            [ 7];/*467*拆借利率**/	\
	char  ratelimit       [ 5];/*472*拆借期限**/	\
	char  o_orderno       [ 8];/*480*原支付交易序号**/      \
	char  procode         [ 8];/*488*处理码**/	\
	char  cr_date         [ 8];/*496*查复日期**/	\
	char  cr_br_no        [12];/*508*查复行号**/	\
	char  cr_tx_num       [ 8];/*516*查复书号**/\
	char  qr_br_no        [12];/*528*查询行号  原查询行号**/	\
	char  qr_tx_num       [ 8] ;/*536*原查询书号 查询书号**/	\
	char  tx_type         [ 1];/*537*原交易种类**/\
	char  notpay_content [255];/*792内容 非支付时候用到**/		\
	char  notpay_orderno  [ 3];/*795撤销申请号   退回申请号**/	\
	char  o_notpay_orderno[ 3];/*798原撤销申请号  原退回申请号**/\	
	char  resp            [ 1];/*799撤销应答 退回应答**/		\
	char  operlevel       [ 1];/*800优先级别**/ \
	char  proc_sts        [ 1];/*801处理状态**/ \
	char  opcd            [ 2];/*803交易处理码以小额含义为准 */ \
	char  lw_ind          [1 ];/*804来往标识 1往,2来 */ \
	char  hp_add_pwd      [1 ];/* 汇票是否核押 */ \
	char  packid          [8 ];/* 包序号 */ \
	char  o_packid        [8 ];/* 原包序号 */ \
	char  pack_date       [8 ];/* 包日期 */ \
	char  o_pack_date     [8 ];/* 原包日期 */ \
	char  txnum           [5 ];/* 业务类型编号 */ \
	char  o_txnum         [5 ];/* 原业务类型编号 */ \
	char  lv_yw_ind       [12];/* 小额业务种类号,2位编号或者12位描述 */ \
	char  resp_date       [8 ];/* 应答日期 */ \
	char  rcpstat         [2 ];/* 业务回执状态 */ \
	char  resp2           [8 ];/* ccpc交易处理代码 */ \
	char  retcode         [2 ];/* 退汇原因 */ \
	char  ywdtlid         [8 ];/* 明细序号 */ \
	char  actype          [ 1];/* 实时业务付款人帐户类型 */ \
	char  reqtype         [ 1];/* 申请类型 0＝单笔	1＝整包 */ \
	char  df_chrg_amt     [16];/* 对方手续费 */ \
	char  prot_no         [60];/* 多方协议号 */ \
	char  po_no           [8 ];/* 汇票号码 */ \
	char  po_type         [1 ];/* 汇票类型0可转 1 不可转 2现金 */ \
	char  otype           [ 1];/* 大小额标志 0大额，1小额 */

typedef struct{
	HV_FD123_AREA_BLOCK
}HV_FD123_AREA;


#endif
