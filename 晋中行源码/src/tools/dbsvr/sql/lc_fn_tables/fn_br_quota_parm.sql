drop table fn_br_quota_parm;
create table fn_br_quota_parm
(
	prdt_code	varchar2(20),	/*产品代码		与Fn_parm中相同					*/
	Br_no		varchar2(5),	/*机构号		                                                */
	Total_amt	Number(16,2),	/*机构认购总额（配额）该机构可以募集的总额，超过该值不能继续认购。      */
    available_amt   Number(16,2),   /*剩余金额*/
	reg_date	number(8),	/*登记日期		                                                */
	Reg_tel		Varchar2(6),	/*登记柜员		                                                */
	Upd_date	Number(8),	/*修改日期		                                                */
	Upd_tel		Varchar2(6)	/*修改柜员                                                              */
)tablespace users;
create unique index fn_br_quota_parm_1 on fn_br_quota_parm(prdt_code,Br_no) tablespace indx;
