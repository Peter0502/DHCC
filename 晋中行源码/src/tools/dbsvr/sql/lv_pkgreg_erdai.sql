
-- 小额支付交易主表
DROP TABLE lv_pkgreg_erdai;
CREATE TABLE lv_pkgreg_erdai
(
  -- 报文类型
	msgtype   	CHAR    ( 35 ) not null ,
  -- 交易发起渠道
	channel   	CHAR    ( 10 )          ,
  -- 交易机构
	br_no     	CHAR    ( 10 )          ,
  -- 小额日期
	bepsdate  	CHAR    (  10 )                ,
  -- 交易时间yyyymmddhhmmss，保存第一次
	tx_time   	CHAR    ( 26 ) not null ,
  -- 委托日期
	wt_date   	CHAR    (  10 )       not null ,
  -- 支付交易序号
	orderno   	INT            not null ,
  -- 业务类型编号
	txtype    	CHAR    (  4 ) not null ,
  -- 业务种类
	txkind    	CHAR    (  5 )          ,
  -- 币种
	cur_no    	CHAR    (  3 ) not null ,
  -- 交易金额
	tx_amt    	DECIMAL (17,2) not null ,
  -- 支取方式
	drawtype  	CHAR    (  4 )          ,
  -- 密码/证件号码/支付密码
	drawval   	CHAR    ( 20 )          ,
  -- 手续费标志
	tx_chrg_ind	CHAR    (  1 )          ,
  -- 手续费金额
	chrg_amt  	DECIMAL (17,2)          ,
  -- 来往标识SR00-往SR01-来
	lw_ind    	CHAR    (  4 ) not null ,
  -- 借贷标志
	cdflag    	CHAR    (  4 ) not null ,
  -- 发起行行号
	or_br_no  	CHAR    ( 14 ) not null ,
  -- 发起清算行行号
	pay_qs_no 	CHAR    ( 14 ) not null ,
  -- 付款人开户行
	pay_opn_br_no	CHAR    ( 14 )          ,
  -- 付款人开户行名称
	pay_opn_br_nm	CHAR    ( 60 )          ,
  -- 付款人帐号
	pay_ac_no 	CHAR    ( 32 ) not null ,
  -- 付款人名称
	pay_name  	CHAR    ( 60 ) not null ,
  -- 付款人地址
	pay_addr  	CHAR    ( 60 )          ,
  -- 接收行行号
	ac_br_no  	CHAR    ( 14 ) not null ,
  -- 接收清算行行号
	cash_qs_no	CHAR    ( 14 ) not null ,
  -- 收款人开户行
	cash_opn_br_no	CHAR    ( 14 )          ,
  -- 收款人开户行名称
	cash_opn_br_nm	CHAR    ( 60 )          ,
  -- 收款人帐号
	cash_ac_no	CHAR    ( 32 ) not null ,
  -- 收款人名称
	cash_name 	CHAR    ( 60 ) not null ,
  -- 收款人地址
	cash_addr 	CHAR    ( 60 )          ,
  -- 凭证种类
	note_type 	CHAR    (  3 )          ,
  -- 凭证号码
	note_no   	CHAR    ( 16 )          ,
  -- 交易状态
	lv_sts    	CHAR    (  3 ) not null ,
  -- 包委托日期
	pack_date 	CHAR    (  10 )                ,
  -- 包序号
	packno    	INT                     ,
  -- 一二代标识
	cnapsflag 	CHAR    (  1 )          ,
  -- 业务拒绝码
	rejectcode	CHAR    ( 35 )          ,
  -- 业务拒绝原因
	rejectreason	CHAR    (105 )          ,
  -- 回执期限
	returndays	INT                     ,
  -- 回执日期
	resp_date 	CHAR    (  10 )                ,
  -- 回执支付序号/回执包序号
	respno    	INT                     ,
  -- 回执包日期
	rsp_pack_date	CHAR    (  10 )                ,
  -- 回执包序号
	rsp_packno	INT                     ,
  -- 业务回执状态
	rcpstat   	CHAR    (  4 )          ,
  -- NPC转发日期
	trandate  	CHAR    (  10 )                ,
  -- 记帐帐号
	accno     	CHAR    ( 32 )          ,
  -- 记账账户名称
	accnm     	CHAR    ( 60 )          ,
  -- 交易日期/平台日期
	tx_date   	CHAR    (  10 )       not null ,
  -- 录入平台流水/接收来帐平台流水
	rcv_trace_no	INT                     ,
  -- 核心记帐日期/录入(接收来帐)
	jz_date   	CHAR    (  10 )                ,
  -- 核心记账流水/录入(接收来帐)
	hostno    	INT                     ,
  -- 录入操作员号
	input_tel 	CHAR    ( 10 )          ,
  -- 录入柜员流水
	check_trace_no	INT                     ,
  -- 挂账标志
	susflag   	CHAR    (  1 )          ,
  -- 挂账机构
	gz_br_no  	CHAR    ( 10 )          ,
  -- 入账帐号
	enteracc  	CHAR    ( 32 )          ,
  -- 入账帐户名称
	enteraccnm	CHAR    ( 60 )          ,
  -- 入账帐号开户机构
	enteropenbrn	CHAR    ( 10 )          ,
  -- 手工入账平台日期
	enterptdate	CHAR    (  10 )                ,
  -- 手工入账平台流水/借记来帐处理平台流水
	enterswno 	INT                     ,
  -- 手工入账核心日期/借记来帐处理日期
	enterdate 	CHAR    (  10 )                ,
  -- 手工入账核心流水/借记来帐处理流水
	entertrace	INT                     ,
  -- 手工入账柜员/借记来帐处理柜员
	entertel  	CHAR    ( 10 )          ,
  -- 手工入账柜员流水
	entertel_no	INT                     ,
  -- 错帐处理平台日期
	deal_date 	CHAR    (  10 )                ,
  -- 错帐处理平台流水
	deal_swno 	INT                     ,
  -- 错账处理核心日期
	deal_mz_date	CHAR    (  10 )                ,
  -- 错帐处理核心流水
	deal_trace_no	INT                     ,
  -- 错帐处理操作员号
	deal_tel  	CHAR    ( 10 )          ,
  -- 错帐处理柜员流水
	deal_tel_no	INT                     ,
  -- 挂账处理平台流水
	gzdeal_trace_no	INT                     ,
  -- 挂账处理平台日期
	gzdeal_trace_date	CHAR    (  10 )                ,
  -- 挂账处理核心流水
	gzdeal_host_no	INT                     ,
  -- 挂账处理核心日期
	gzdeal_host_date	CHAR    (  10 )                ,
  -- 人行对帐检查状态
	checkflag 	CHAR    (  1 )          ,
  -- 行内对帐检查状态
	hostflag  	CHAR    (  1 )          ,
  -- 打印次数
	prt_times 	INT            not null ,
  -- 复核操作员号
	check_tel 	CHAR    ( 10 )          ,
  -- 授权操作员号
	auth_tel  	CHAR    ( 10 )          ,
  -- 发送操作员号
	send_tel  	CHAR    ( 10 )          ,
  -- 回执操作员号
	resp_tel  	CHAR    ( 10 )          ,
  -- 多方协议号
	protno    	CHAR    ( 60 )          ,
  -- 报文优先级
	mesgpriority	CHAR    (  1 )          ,
  -- 业务序号
	txno      	INT                     ,
  -- 帐户类型
	acc_type  	CHAR    (  1 )          ,
  -- 附言
	lv_brf    	CHAR    (270 )          ,
  -- 附言2
	lv_brf2   	CHAR    (150 )          ,
  -- 备注
	filler    	CHAR    (270 )          ,
  -- 备注2
	filler2   	CHAR    ( 10 )          ,
  -- 销帐编号
	chargeoff 	CHAR    ( 20 )          
) ;

CREATE UNIQUE INDEX lv_pkgreg_erdai_IDX ON lv_pkgreg_erdai ( or_br_no,wt_date,orderno,lw_ind );
