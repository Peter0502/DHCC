/* ͬҵ���ǼǱ� */
drop index mo_ibl_1;
drop  table mo_ibl;
create table mo_ibl(
	pact_no varchar2(20),     /*  ��ͬ��                 */         
	br_no varchar2(5),        /*  ���л�������           */         
	ac_id number(9),          /*  ��轻�����ɵ��ڲ��ʺ� */         
	op_ac_no varchar2(20),    /*  �����˺�               */         
	acc_no varchar2(7),       /*  ��Ŀ��                 */         
	prdt_no varchar2(3),      /*  ��Ʒ���               */         
	sign varchar2(1),         /*  ��������־           */         
	op_name varchar2(50),        /*  �����ʻ�����           */         
	term number(9),           /*  �������               */         
	amt number(16,2),         /*  �����               */         
	rate number(8,5),         /*  �������               */         
	over_rate number(8,5),    /*  ��������               */         
	no_pay_bal number(16,2),  /*  ��Ƿ���               */         
	opn_date number(9),       /*  ��������               */         
	matr_date number(9),      /*  ��������               */         
	wrk_date number(9),       /*  ��������               */         
	trance_no number(9),      /*  ������ˮ��             */         
	sts varchar2(1),          /*  ��־                   */         
	tel varchar2(4),          /*  ����Ա                 */         
	chk varchar2(4),          /*  ����Ա                 */         
	auth varchar2(4),          /*  ��Ȩ��                 */         
  op_br_no	varchar2(12),   /*  �����к�               */         
  kh_ac_no	varchar2(19),      /*  ����/���ı��пͻ��ʺ�  */
  kh_name	  varchar2(60)    /*  ����/���ı��пͻ���    */
)tablespace users;
create	 unique index mo_ibl_1 on mo_ibl( pact_no)tablespace indx;
                                         
