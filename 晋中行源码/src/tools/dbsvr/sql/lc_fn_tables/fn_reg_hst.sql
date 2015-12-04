drop table fn_reg_hst;
create table fn_reg_hst
(
	tx_date		number(8),	/*��������				*/	
	trace_no	number(9),	/*������ˮ��		                */
	tx_br_no	varchar2(5),	/*���׻���		                */
	tel		varchar2(6),	/*���׹�Ա		                */
	type		varchar2(1),	/*�������		0���Ϲ�����     */
					/*			1���۱�����   */
					/*			2���������潻�� */
					/*			3��ļ��ʧ�ܽ��� */
					/*			4:�깺����      */
					/*			5:�깺ȷ�Ͻ���  */
					/*			6:��ؽ���      */
					/*			*����������     */
	tx_amt		number(16,2),	/*���׽��		                */
	agent_id_type	varchar2(1),	/*������֤����		                */
	agent_id	varchar2(20),	/*������֤����		                */
	agent_name	varchar2(60),	/*����������		                */
	buy_date	number(8),	/*�Ϲ�����		                */
	buy_trace_no	number(9),	/*�Ϲ���ˮ		                */
	brf		varchar2(20),	/*ժҪ		                        */
	filler		varchar2(60)	/*��ע		                        */
)tablespace users;
create index fn_reg_hst_1 on fn_reg_hst(tx_date,trace_no) tablespace indx;
create index fn_reg_hst_2 on fn_reg_hst(tx_date,type)tablespace indx;
