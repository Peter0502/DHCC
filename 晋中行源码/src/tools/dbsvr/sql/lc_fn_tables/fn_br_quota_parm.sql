drop table fn_br_quota_parm;
create table fn_br_quota_parm
(
	prdt_code	varchar2(20),	/*��Ʒ����		��Fn_parm����ͬ					*/
	Br_no		varchar2(5),	/*������		                                                */
	Total_amt	Number(16,2),	/*�����Ϲ��ܶ���û�������ļ�����ܶ������ֵ���ܼ����Ϲ���      */
    available_amt   Number(16,2),   /*ʣ����*/
	reg_date	number(8),	/*�Ǽ�����		                                                */
	Reg_tel		Varchar2(6),	/*�Ǽǹ�Ա		                                                */
	Upd_date	Number(8),	/*�޸�����		                                                */
	Upd_tel		Varchar2(6)	/*�޸Ĺ�Ա                                                              */
)tablespace users;
create unique index fn_br_quota_parm_1 on fn_br_quota_parm(prdt_code,Br_no) tablespace indx;
