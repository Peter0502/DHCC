/**************************************************/
/* TABLE NAME:POINFO 本票基本信息表                */
/* Description  : Postal Order Saving Master File  */
/* Table index  : pono,signday,                    */
/* create by shenlei: 2006-11-9                    */
/***************************************************/
drop index cashier_idx;
drop table cashier;
create table cashier(
	pono         varchar2(20),/* 本票号码*/
	brno         varchar2(7), /* 机构号码*/
	orbrno       varchar2(12),/* 发起行行号*/
	payqsactno   varchar2(12),/* 支付行清算帐号*/
	txday        number(8),   /* 交易日期 */
	lostcnt      number(6),   /* 挂失次数每挂失，借挂失增加一 */
	obrno        varchar2(12),/* 对方机构号 no user*/
	voctype      varchar2(3), /* 凭证种类  */
	vocnum       varchar2(16),/* 凭证号   */
	ovoctype     varchar2(3), /* 申请书种类*/
	ovocnum      varchar2(16),/* 申请书号*/
	avbal        number(15,2),/* 金额(出票金额／票面金额)*/
	useamt       number(15,2),/* 实际结算金额 */
	feeamt       number(15,2),/* 手续费 */
	leavebal     number(15,2),/* 多余金额 */
	cour         varchar2(3), /* 币种 */
	paybrno      varchar2(12),/* 付款人开户行号 */
	payactno     varchar2(32),/* 付款人帐号*/
	payname      varchar2(60),/* 付款人户名*/
	paybrname    varchar2(60),/* 签发行行名*/ 
	payaddress   varchar2(60),/* 付款人地址*/ 
	cashactno    varchar2(32),/* 收款人帐号*/ 
	cashbrno     varchar2(12),/* 兑付行行号*/
	cashbrname   varchar2(60),/* 兑付行行名*/
	cashaddress  varchar2(60),/* 兑付行地址*/
	cashname     varchar2(60),/* 收款人户名*/
	holdbrno     varchar2(12),/* 最后兑付行号*/
    	holdbrname   varchar2(60),/* 最后兑付行名*/
	holdactno    varchar2(32),/* 最后兑付帐号*/
	holdname     varchar2(60),/* 最后兑付名字*/
	lactcls      varchar2(1), /* 上次帐户状态*/
	stat         varchar2(1), /* 本票当前状态标志 0-登记 1-签发 
		2-本票兑付 3-本票解付 4-本票挂失 5-解挂失
		6-本票退票 7-签发取消 8-登记取消 */
	cashstat     varchar2(1), /*解付状态：0、系统内本行解付本行，1、系统内本行解付它行， 3、系统外解付本行,2.系统内解付系统外*/
	signday      number(8),   /* 签发日期*/
	regday       number(8),   /* 复核日期*/
	cashday      number(8),   /* 兑付日期*/
	jqday        number(8),   /* 结清日期*/
	endday       number(8),   /* 本票到期日期*/
	passwd       varchar2(20),/* 密押*/
        avbaltype    varchar2(1), /* 定额标志:0.定额，1.不定额*/
	bptype       varchar2(1), /* 本票类别0.转帐本票1.现金本票*/
	ccflag       varchar2(1), /* 转帐现金标志0-现金1 转账(帐号) 2科目对转*/
	cashflag     varchar2(1), /* 收款现金标志0-现金1 转账(帐号) 2科目对转*/
	drsubject    varchar2(7), /* 付款科目*/
	crsubject    varchar2(7), /* 资金去向*/
	reason       varchar2(60),/* 摘要*/
	feeflag      varchar2(1), /* 手续费标志  0.现金 1.现金加急 2.转帐 3.转帐加急 4.不收*/
	checkflag    varchar2(1), /* 对帐标志 */
	inflag       varchar2(1), /* 行内标志：0.行内，1.行外*/
	optlrno      varchar2(4) ,/* 开户操作员 */
	chktlrno     varchar2(4) ,/* 签发操作员 */
	passtlrno    varchar2(4) ,/* 密押操作员 */
	cashtlrno    varchar2(4) ,/* 兑付操作员 */
	jqtlrno      varchar2(4) ,/* 结清操作员 */
	addpassflg   varchar2(1) ,/* 是否加/核押处理 1-加押/核押 0-未处理 */
	filler       varchar2(60) /* 备注*/
)tablespace users;
create unique index cashier_idx on cashier(pono,signday) tablespace indx;
