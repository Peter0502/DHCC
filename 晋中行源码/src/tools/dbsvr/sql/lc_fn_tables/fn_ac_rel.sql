drop table fn_ac_rel;
create table fn_ac_rel
(
	prdt_code	varchar2(20),	/*理财产品代码				*/
	qs_ac_no	varchar2(24),	/*理财资金清算账户	内部账，记借方  */
	wt_ac_no	varchar2(24),	/*委托投资理财账户	内部账，记贷方  */
    sy_ac_no    varchar2(24)    /*手续费、赎回费用账号*/
)tablespace users;
create unique index fn_ac_rel_1 on fn_ac_rel(prdt_code) tablespace indx;
