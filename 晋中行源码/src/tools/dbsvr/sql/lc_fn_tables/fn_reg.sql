drop table fn_reg;
create table fn_reg
(
	tx_date		number(8),	/*��������								*/
	trace_no	number(9),	/*������ˮ��		                                                */
	tx_br_no	varchar2(5),	/*���׻���		                                                */
	pact_no		varchar2(20),	/*Э����		                                                */
	prdt_code	varchar2(20),	/*��Ʒ����		                                                */
	ac_no		varchar2(24),	/*�˺�		                                                        */
	ac_id		number(9),	/*�˻�ID		                                                */
	ac_seqn		number(6),	/*�˻����		                                                */
	cif_type	varchar2(1),	/*�ͻ�����		1.���ˣ�2.�Թ�                                  */
	ct_ind		varchar2(1),	/*��ת��־		                                                */
	chnl_type	Varchar2(4),	/*��������		                                                */
	count		number(9),	/*�Ϲ�����		                                                */
	tx_amt		number(16,2),	/*�Ϲ����		                                                */
	redempt_amt	number(16,2),	/*��ؽ��		��Ͷ���ڼ�ͻ���صĽ���������ʱ��          */
					/*			����=�Ϲ����-��ؽ��                          */
	return_capital	number(16,2),	/*���ر���		                                                */
	flt_radio	Number(9,6),	/*���ʸ�������		                                                */
	return_rate	number(9,6),	/*������������		ʵ��������*��1+���ʸ���������                   */
	intst		number(16,2),	/*Ԥ������		                                                */
	ic_date		number(8),	/*��Ϣ����		                                                */
	end_date	number(8),	/*��������		                                                */
	fct_intst	number(16,2),	/*����			δ�۳�����                                      */
	profit_amt	number(16,2),	/*Ԥ�ڷ�����		�Ϲ������ķ�����                              */
	fct_profit_amt	number(16,2),	/*ʵ�ʷ�����		�Ϲ������ķ�����                              */
	profit_flag	varchar2(1),	/*�����־		�Ƿ�ͳ�ƽ���fn_br_profit                        */
					/*			Y:��                                            */
					/*			N:��                                            */
	brf		varchar2(20),	/*ժҪ		                                                        */
	sts		varchar2(1),	/*״̬		0���Ϲ�                                                 */
					/*		1���ѿۿ�                                               */
					/*		2���ѷ�������                                           */
					/*		3��ļ��ʧ��                                             */
					/*		4���깺                                                 */
					/*		5���깺δͨ��                                           */
					/*		*������                                                 */
    amt1    number(16,2),/*���ý��1*/
    amt2    number(16,2),/*���ý��2*/
    date1    number(8),/*��������1*/
    date2    number(8),/*��������1*/
	pay_date	number(8),	/*�ۿ�����		                                                */
	pay_trace_no	number(9),	/*�ۿ���ˮ��		                                                */
	return_date	number(8),	/*������������		                                                */
	return_trace_no	number(9),	/*����������ˮ��		                                        */
	tel		varchar2(6),	/*ǩԼ��Ա		                                                */
	mang		varchar2(6),	/*�ͻ������		                                                */
	agent_id_type	varchar2(1),	/*������֤������	                                            */
	agent_id	varchar2(20),	/*������֤��		                                                */
	agent_name	varchar2(60),	/*����������		                                                */
	filler		varchar2(60)	/*��ע		                                                        */
)tablespace users;
create unique index fn_reg_1 on fn_reg(tx_date,trace_no) tablespace indx;
create index fn_reg_2 on fn_reg(prdt_code,ac_id,ac_seqn,sts) tablespace indx;
create index fn_reg_3 on fn_reg(tx_br_no,prdt_code,sts) tablespace indx;
