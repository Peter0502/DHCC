/* *********************************
* ��ҵ��������������ʽ������
*
*
**********************************/
drop  table syyh_czyh_qs_reg;
create table syyh_czyh_qs_reg(
	tx_date number(8),  /* ��������*/
	flag    varchar2(1), /* 1-��� ��2-С�� */
	trace_no number(9),  /* ������ˮ�� */
	tx_code varchar2(4),
	wz_amt  number(16,2), /* �����������ʽ�� */
	lz_amt  number(16,2), /* �����������ʽ�� */
	zc_amt  number(16,2), /* ������,������*/
	tel     varchar2(7) , /* �������Ա */
	qs_trace_no number(9),/* ������ˮ�� */
	qs_time     number(9),/* ����ʱ�� */
	qs_flag     varchar2(1), /* �����־ 1-δ���㣬�����°࣬2-������ */
	br_no     varchar2(5) /* �Է�����*/
)tablespace users;
create unique index syyh_czyh_qs_reg_1 on syyh_czyh_qs_reg( tx_date,flag)tablespace indx;
