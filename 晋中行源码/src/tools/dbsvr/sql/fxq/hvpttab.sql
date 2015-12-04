/***********************************************************/
/** hvpttab:人民币大额转帐表                               */
/** 创建日期:2005-04-28                                    */
/***********************************************************/
drop index  hvpttab_idx;
drop table  hvpttab;
create table hvpttab
(
	brno varchar2(12), 	   /*报送机构代码*/
	brname varchar2(60),  /*报送机构名称*/
	txday varchar2(8),		/*交易日期*/
	txtime varchar2(8),		/*交易时间*/	 
	aptype varchar2(1),		/*业务种类*/
	txcd varchar2(2),			/*交易代码*/
	sendday varchar2(8),		/*报送日期*/
	apflg varchar2(32),		/*业务标识*/
	paybrno varchar2(12),	/*付款行行号*/
	paybrtype varchar2(1),	/*付款行行号类型*/
	payname varchar2(60),	/*付款人名称*/
	payactno varchar2(40),	/*付款人账号*/
	paycode varchar2(20),	/*付款人机构代码*/
	cashbrno varchar2(12),	/*收款行行号*/
	cashbrtype varchar2(1),	/*收款行行号类型*/
	cashname varchar2(60),	/*收款人名称*/
	cashactno varchar2(40),	/*收款人账号*/
	cashbrcode varchar2(20),/*收款人机构代码*/
	txamt varchar2(18),		/*货币符号金额*/
	paydate varchar2(8),		/*付款日期*/
	cashdate varchar2(8),	/*收款日期*/
	dscpt varchar2(20),		/*用途*/
	cashtype varchar2(2),   /*结算方式*/	
	crdno varchar2(32), 		/*凭证代码*/
	txtypeno varchar2(10), 	/*交易类型代码*/
	expand varchar2(20)   	/*扩展*/
)tablespace users;
create unique index hvpttab_idx on hvpttab(apflg) tablespace indx;
