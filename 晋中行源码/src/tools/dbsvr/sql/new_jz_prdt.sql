drop index new_jz_prdt_1;
drop  table new_jz_prdt;
create table new_jz_prdt(
	pact_no varchar2(20),--��ݺ�
	ac_no varchar2(19),--�˺�
	ac_name varchar2(60),--����
	old_prdt_no varchar2(3),--�²�Ʒ��
	old_prdt_name varchar2(60),--�²�Ʒ����
	new_prdt_no varchar2(3),--��ֵ��Ʒ��
	new_prdt_name varchar2(60),--��ֵ��Ʒ����
	id_no varchar2(20),--֤������
	bj_amt number(16,2),--�˻����
	in_lo_intst number(16,2),--����ǷϢ
	out_lo_intst number(16,2),--����ǷϢ
	sts varchar2(1), --״̬-0δ��ֵ 1-�Ѽ�ֵ
	ac_id number(9),---�˺�id
	tx_date number(8),--��������
	tx_time number(9),--����ʱ��
	trace_no number(9),--������ˮ
	tel varchar2(4),---���׹�Ա
	tx_br_no varchar2(5),---���׻���
	amt1 number(16,2),--���ý��1
	amt2 number(16,2),---���ý��2
	code1 varchar2(60),--�����ֶ�1
	code2 varchar2(60) ---�����ֶ�2
)tablespace users;
create	 unique index new_jz_prdt_1 on new_jz_prdt(ac_no,pact_no);
