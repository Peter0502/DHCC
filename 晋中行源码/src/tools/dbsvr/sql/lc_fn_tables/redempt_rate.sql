drop table redempt_rate;
create table redempt_rate
(
	prdt_code	varchar2(20),	/*��Ʒ����				*/
    rate_code   varchar2(3),  /*���ʱ��001�������002������*/
	beg_date	number(8),	/*������ʼ����		                */
	end_date	number(8),	/*��������		Ĭ��99999999    */
	min_days	number(8),	/*��ʼ����		����            */
	max_days	number(8),	/*��ֹ����		������          */
	redempt_rate	number(9,6),	/*����		   */
	profit_rate	number(9,6)	/* �������� */
)tablespace users;
create unique index redempt_rate_1 on redempt_rate(prdt_code,rate_code,beg_date,end_date,min_days,max_days) tablespace indx;
