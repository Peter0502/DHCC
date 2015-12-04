
DROP TABLE hv_zf_erdai;
CREATE TABLE hv_zf_erdai
(
  -- CMT编号/报文类型
	cmtno     	CHAR    ( 20 ) not null ,
  -- 通信级标识号
	commid    	CHAR    ( 20 ) not null ,
  -- 通信级参考号
	refcommid 	CHAR    ( 20 )          ,
  -- 报文标识号
	msgno     	CHAR    ( 35 ) not null ,
  -- 发送网点
	snd_br_no 	CHAR    ( 10 )          ,
  -- 接收网点
	rcv_br_no 	CHAR    ( 10 )          ,
  -- 委托日期
	wt_date   	CHAR    (  10 )       not null ,
  -- 人行的交易序号
	orderno   	INT            not null ,
  -- 交易时间
	tx_time   	CHAR    ( 26 ) not null ,
  -- 业务类型
	yw_kind   	CHAR    (  4 ) not null ,
  -- 业务种类
	yw_type   	CHAR    (  5 ) not null ,
  -- 货币符号
	cur_no    	CHAR    (  3 ) not null ,
  -- 交易金额
	tx_amt    	DECIMAL (15,2) not null ,
  -- 支取方式
	drawtype  	CHAR    (  4 )          ,
  -- 密码/证件号码/支付密码
	drawval   	CHAR    ( 20 )          ,
  -- 手续费标志1现金2转账3不收
	tx_chrg_ind	CHAR    (  1 )          ,
  -- 手续费
	chrg_amt  	DECIMAL (15,2)          ,
  -- 来往标识SR00-往SR01-来
	lw_ind    	CHAR    (  4 ) not null ,
  -- 借贷标志
	cd_flag   	CHAR    (  4 )          ,
  -- 发起清算行行号(帐号)accountno
	pay_qs_no 	CHAR    ( 14 ) not null ,
  -- 发起行行号--退汇交易的接收行
	or_br_no  	CHAR    ( 14 ) not null ,
  -- 发起行行名
	or_br_nm  	CHAR    ( 60 )          ,
  -- 记帐帐号
	accno     	CHAR    ( 40 )          ,
  -- 记账帐号名称
	accnm     	CHAR    ( 60 )          ,
  -- 付款人开户行行号
	pay_opn_br_no	CHAR    ( 14 )          ,
  -- 付款人开户行行名
	pay_opn_br_nm	CHAR    ( 60 )          ,
  -- 付款人帐号
	pay_ac_no 	CHAR    ( 32 ) not null ,
  -- 付款人姓名--退汇交易的收款人户名
	pay_name  	CHAR    ( 60 ) not null ,
  -- 付款人地址
	pay_addr  	CHAR    ( 60 )          ,
  -- 接收行清算行号
	cash_qs_no	CHAR    ( 14 ) not null ,
  -- 接收行行号--退汇交易的发起行
	ac_br_no  	CHAR    ( 14 ) not null ,
  -- 接收行行名
	ac_br_nm  	CHAR    ( 60 )          ,
  -- 收款人开户行行号
	cash_opn_br_no	CHAR    ( 14 )          ,
  -- 收款人开户行行名
	cash_opn_br_nm	CHAR    ( 60 )          ,
  -- 收款人帐号
	cash_ac_no	CHAR    ( 32 ) not null ,
  -- 收款人姓名--退汇交易的付款人户名
	cash_name 	CHAR    ( 60 ) not null ,
  -- 收款人地址
	cash_addr 	CHAR    ( 60 )          ,
  -- 中介机构1
	mediary1  	CHAR    ( 35 )          ,
  -- 中介机构1名称
	mediaryname1	CHAR    (140 )          ,
  -- 中介机构2
	mediary2  	CHAR    ( 35 )          ,
  -- 中介机构2名称
	mediaryname2	CHAR    (140 )          ,
  -- 存放stat原始状态用于事后监督用
	beg_sts   	CHAR    (  4 )          ,
  -- 优先级别
	operlevel 	CHAR    (  4 )          ,
  -- 凭证类型
	note_type 	CHAR    (  3 )          ,
  -- 冠子号码
	note_preno	CHAR    (  4 )          ,
  -- 凭证号
	note_no   	CHAR    ( 16 )          ,
  -- 端到端标识号
	endtoendid	CHAR    ( 35 )          ,
  -- 清算日期
	clsdate   	CHAR    (  10 )                ,
  -- 业务状态
	hv_sts    	CHAR    (  4 ) not null ,
  -- 退汇交易用到的原支付来帐交易状态
	hv_osts   	CHAR    (  4 )          ,
  -- 处理状态
	procstat  	CHAR    (  4 )          ,
  -- 处理码
	respcode  	CHAR    (  8 )          ,
  -- 应答信息
	rspinfo   	CHAR    (200 )          ,
  -- 业务处理参与机构
	procarty  	CHAR    ( 12 )          ,
  -- 拒绝重发标志
	resend_ind	CHAR    (  1 )          ,
  -- 拒绝重新发送日期
	resend_date	CHAR    (  10 )                ,
  -- 打印次数
	hv_prt_ind	INT                     ,
  -- 人行对帐检查状态
	checkflag 	CHAR    (  1 )          ,
  -- 行内对帐检查状态
	hostflag  	CHAR    (  1 )          ,
  -- 授权操作员
	auth_tel  	CHAR    ( 10 )          ,
  -- 授权操作员流水号
	auth_trace_no	INT                     ,
  -- 复核操作员
	chk       	CHAR    ( 10 )          ,
  -- 复核操作员流水号
	chk_trace_no	INT                     ,
  -- 发送操作员
	send_tel  	CHAR    ( 10 )          ,
  -- 发送操作员流水号
	send_trace_no	INT                     ,
  -- 发送时核心记账日期
	host_date_snd	CHAR    (  10 )                ,
  -- 发送时核心记账流水号
	trace_no_snd	INT                     ,
  -- 交易发起渠道
	channel   	CHAR    ( 10 )          ,
  -- 一二代标识
	cnapsflag 	CHAR    (  1 )          ,
  -- 交易日期
	tx_date   	CHAR    (  10 )       not null ,
  -- 接收平台流水号
	rcv_swno  	INT                     ,
  -- 核心记账日期
	host_date 	CHAR    (  10 )                ,
  -- 核心记账流水号
	trace_no  	INT                     ,
  -- 操作员员
	tel       	CHAR    ( 10 ) not null ,
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
  -- 手工入账日期
	enterdate 	CHAR    (  10 )                ,
  -- 手工入账流水
	entertrace	INT                     ,
  -- 手工入账平台流水
	enterswno 	INT                     ,
  -- 手工入账平台日期
	enterswdate	CHAR    (  10 )                ,
  -- 手工入账柜员
	entertel  	CHAR    ( 10 )          ,
  -- 手工入账柜员流水
	entertel_no	INT                     ,
  -- 错帐处理日期
	dealerr_date	CHAR    (  10 )                ,
  -- 错帐处理柜员
	dealerr_tel	CHAR    ( 10 )          ,
  -- 错帐处理主机流水
	dealerr_hostno	INT                     ,
  -- 错帐处理平台流水
	dealerr_swno	INT                     ,
  -- 错账处理平台日期
	dealerr_swdate	CHAR    (  10 )                ,
  -- 挂账处理平台流水
	gzdeal_trace_no	INT                     ,
  -- 挂账处理平台日期
	gzdeal_trace_date	CHAR    (  10 )                ,
  -- 挂账处理核心流水
	gzdeal_host_no	INT                     ,
  -- 挂账处理核心日期
	gzdeal_host_date	CHAR    (  10 )                ,
  -- 备注
	hv_brf    	CHAR    (270 )          ,
  -- 备注2
	hv_brf2   	CHAR    ( 10 )          ,
  -- 附言
	remark    	CHAR    (270 )          ,
  -- 附言2
	remark2   	CHAR    (150 )          ,
  -- 销帐编号
	chargeoff 	CHAR    ( 20 )          
) ;

CREATE UNIQUE INDEX hv_zf_erdai_IDX ON hv_zf_erdai ( or_br_no,msgno,lw_ind );
CREATE UNIQUE INDEX hv_zf_erdai_IDX2 ON hv_zf_erdai ( or_br_no,wt_date,orderno,lw_ind );
