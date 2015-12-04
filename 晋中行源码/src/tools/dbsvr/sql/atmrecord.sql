/* ATM分户帐atmrecord
table atmrecord
table index： atmrecord_idx  unique  atmid varchar2(20);*/

drop index  atmrecord_idx;
drop table atmrecord;      
create table atmrecord(    
	Atmno	       Varchar2(10),    /*  银联ATM机号 */
	Atmid	       Varchar2(20),    /*  ATM序列号*/
	Atmmode	       Varchar2(1),     /*  ATM型号*/
	Opnbr	       Varchar2(7),     /*  开户机构*/
	Depflag	       Varchar2(1),     /*  存款功能标志*/
	Chaflag	       Varchar2(1),     /*  转账功能标志*/
	Userflag       Varchar2(1),     /*  起用标 1起用2未起用*/         
	Atmstat	       Varchar2(1),     /*  ATM状态	 1正常2故障3未签到*/
	Paycnt	       Number  (3),     /*  每天取款次数*/
	Payamt	       Number(16,2),  /*  每笔最大金额*/
	Paymamt	       Number(16,2),  /*  每笔最大金额*/
	Feerate	       Number(7,5),   /*  手续费*/
	Opndate	       Varchar2(8),     /*  注册日期*/
	Startdte       Varchar2(8),     /*  起用日期*/
	Enddate	       Varchar2(8),     /*  停用日期*/
	Place	       Varchar2(40),     /*  安放地址*/
	tlrno	       Varchar2(6),     /*ATM  操作员号*/
	filler	       Varchar2(60)     /*  操作员号*/
)tablespace users;
create unique index atmrecord_idx on atmrecord (atmno) tablespace indx;
