drop table fn_br_profit;
create table fn_br_profit
(
	prdt_code	varchar2(20),	/*��Ʋ�Ʒ����				*/
	br_no		varchar2(5),	/*��������		                */
	tx_amt		number(16,2),	/*ļ�����	������ļ�������ܶ�      */
	redempt_amt	number(16,2),	/*��ؽ��		                */
	profit_amt	number(16,2),	/*������		                */
	reg_date	number(8),	/*�Ǽ�����		                */
	reg_trace_no	number(9),	/*�Ǽ���ˮ		                */
	pay_date	number(8),	/*��������		                */
	pay_trace_no	number(9),	/*������ˮ		                */
	sts		varchar2(1),	/*״̬		0:�Ǽ�                  */
					/*		1������                 */
	brf		varchar2(20),	/*ժҪ		                        */
	filler		varchar2(60)	/*��ע		                        */
)tablespace users;
create unique index fn_br_profit_1 on fn_br_profit(prdt_code,br_no) tablespace indx;
