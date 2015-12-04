/***********************************************************/
/** hvcstab:人民币大额现金存入业务表                               */
/** 创建日期:2005-04-28                                    */
/***********************************************************/
drop index  hvcstab_idx;
drop table  hvcstab;
create table hvcstab
(
	brno varchar2(12), 	      /*报送机构代码*/
	brname varchar2(60),     /*报送机构名称*/
	txday varchar2(8),		   /*交易日期*/
	txtime varchar2(8),		   /*交易时间*/	 
	aptype varchar2(1),		   /*业务种类*/
	txcd varchar2(2),			   /*交易代码*/
	sendday varchar2(8),		   /*报送日期*/
	apflg varchar2(32),		   /*业务标识*/
	Payname	varchar2(60),     /*缴款人名称*/	
   paycertif	varchar2(22),  /*缴款人证件类型及号码 2位证件类型、20位号码*/
   cashbrno	varchar2(12),     /*收款行行号*/	
   cashbrtype	varchar2(1),   /*收款行行号类型*/
   cashname	varchar2(60),     /*收款人名称*/	
   cashactno	varchar2(40),  /*收款人账号*/	
   cashertype	varchar2(2),      /*收款人类型 01：单位；02：个人*/
   cashcode	varchar2(20),     /*收款人机构代码	可选*/
   cashcertif	varchar2(22),  /*收款人证件类型及号码	2位证件类型、20位号码可选*/
   txamt	varchar2(18),        /*货币符号及金额	3位币种、15位金额*/
   cashdate	varchar2(8),      /*收款日期*/	
   cfrom	varchar2(20),        /*现金来源	可选*/
	cashtype varchar2(2), 		   /*结算方式*/
	txtypeno varchar2(10), 	   /*交易类型代码*/
	expand varchar2(20)		   /*扩展*/
)tablespace users;
create unique index hvcstab_idx on hvcstab(apflg) tablespace indx;
