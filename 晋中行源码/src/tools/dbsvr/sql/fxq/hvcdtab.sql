/***********************************************************/
/** hvcdtab:人民币大额现金支取业务表　                     */
/** 创建日期:2005-04-28                                    */
/***********************************************************/
drop index  hvcdtab_idx;
drop table  hvcdtab;
create table hvcdtab
(
	brno varchar2(12), 	      /*报送机构代码*/
	brname varchar2(60),     /*报送机构名称*/
	txday varchar2(8),			/*交易日期*/
	txtime varchar2(8),		   /*交易时间*/	 
	aptype varchar2(1),		   /*业务种类*/
	txcd varchar2(2),			   /*交易代码*/
	sendday varchar2(8),		   /*报送日期*/
	apflg varchar2(32),		   /*业务标识*/
	paybrno	varchar2(12),     /*付款行行号*/		
   paybrtype	varchar2(1),   /*付款行行号类型*/	
   payname	varchar2(60),     /*付款人名称*/			
   payactno	varchar2(40),     /*付款人账号*/		
   paycode	varchar2(20),     /*付款人机构代码	可选*/
   payertype	varchar2(2),   /*付款人类型 01：单位；02：个人*/
   paycertif	varchar2(22),  /*付款人证件类型及号码 可选 2位证件类型、20位号码*/
   cashname	varchar2(60),     /*收款人名称*/
   cashertype	varchar2(2),      /*收款人类型	01：单位；02：个人*/
   cashcertif	varchar2(22),  /*收款人证件类型及号码	可选 2位证件类型、20位号码*/
   txamt	varchar2(18),        /*货币符号及金额	3位币种、15位金额*/
   paydate	varchar2(8),      /*付款日期*/
   dscpt	varchar2(20),        /*用途 可选*/
	cashtype varchar2(2), 		   /*结算方式*/
	txtypeno varchar2(10), 	   /*交易类型代码*/
	expand varchar2(20)		   /*扩展*/
)tablespace users;
create unique index hvcdtab_idx on hvcdtab(apflg) tablespace indx;
