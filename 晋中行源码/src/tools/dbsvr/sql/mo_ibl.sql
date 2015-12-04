/* 同业拆借登记薄 */
drop index mo_ibl_1;
drop  table mo_ibl;
create table mo_ibl(
	pact_no varchar2(20),     /*  合同号                 */         
	br_no varchar2(5),        /*  本行机构编码           */         
	ac_id number(9),          /*  拆借交易生成的内部帐号 */         
	op_ac_no varchar2(20),    /*  外行账号               */         
	acc_no varchar2(7),       /*  科目号                 */         
	prdt_no varchar2(3),      /*  产品编号               */         
	sign varchar2(1),         /*  拆入拆出标志           */         
	op_name varchar2(50),        /*  外行帐户户名           */         
	term number(9),           /*  拆借期限               */         
	amt number(16,2),         /*  拆借金额               */         
	rate number(8,5),         /*  拆借利率               */         
	over_rate number(8,5),    /*  逾期利率               */         
	no_pay_bal number(16,2),  /*  尚欠金额               */         
	opn_date number(9),       /*  开户日期               */         
	matr_date number(9),      /*  到期日期               */         
	wrk_date number(9),       /*  交易日期               */         
	trance_no number(9),      /*  主机流水号             */         
	sts varchar2(1),          /*  标志                   */         
	tel varchar2(4),          /*  操作员                 */         
	chk varchar2(4),          /*  复合员                 */         
	auth varchar2(4),          /*  授权人                 */         
  op_br_no	varchar2(12),   /*  外行行号               */         
  kh_ac_no	varchar2(19),      /*  拆入/出的本行客户帐号  */
  kh_name	  varchar2(60)    /*  拆入/出的本行客户名    */
)tablespace users;
create	 unique index mo_ibl_1 on mo_ibl( pact_no)tablespace indx;
                                         
