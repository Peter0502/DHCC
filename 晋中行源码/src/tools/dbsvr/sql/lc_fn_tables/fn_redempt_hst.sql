drop table fn_redempt_hst;
create table fn_redempt_hst
(
	tx_date			number(8),	/*��������							*/
	trace_no		number(9),	/*������ˮ��		                                        */
	tx_br_no		varchar2(5),	/*���׻���		                                        */
	pact_no			varchar2(20),	/*Э����		                                        */
	prdt_code		varchar2(20),	/*��Ʒ����		                                        */
	ac_no			varchar2(24),	/*�˺�		                                                */
	ac_id			number(9),	/*�˻�ID		                                        */
    ac_seqn         number(6),  /*�˻����                                              */
	chnl_type		varchar2(4),	/*��������		                                        */
	redempt_amt		number(16,2),	/*��ؽ��		                                        */
	redempt_chrg_amt	number(16,2),	/*��ط���		                                        */
	return_rate		number(9,6),	/*������������		                                        */
	ic_date			number(8),	/*��Ϣ����	�����������ʼ���ڣ���ֹ����Ϊ�������ڡ�        */
	fct_intst		number(16,2),	/*����		����*����*��ؽ�δ�۳����ַ���              */
	return_capital		number(16,2),	/*���ر���		                                        */
	profit_amt		number(16,2),	/*������		                                        */
	profit_flag		varchar2(1),	/*�����־		�Ƿ�ͳ�ƽ���fn_br_profit                */
						/*			Y:��                                    */
						/*			N:��                                    */
	tel			varchar2(6),	/*��Ա		                                                */
	return_date		number(8),	/*�黹��������		�����������治��ͬʱ�黹����Ҫ���ֶ�  */
	return_trace_no		number(9),	/*�黹�������ˮ		                                */
	sts			varchar2(1),	/*״̬		0���Ǽ�                                         */
						/*		1���ѷ�������                                   */
	buy_date		number(8),	/*�Ϲ�����		                                        */
	buy_trace_no		number(9),	/*�Ϲ���ˮ��		                                        */
	filler			varchar2(60)	/*��ע		                                                */
)tablespace users;
create unique index fn_redempt_hst_1 on fn_redempt_hst(tx_date,trace_no) tablespace indx;
create index fn_redempt_hst_2 on fn_redempt_hst(tx_date,prdt_code)tablespace indx;
