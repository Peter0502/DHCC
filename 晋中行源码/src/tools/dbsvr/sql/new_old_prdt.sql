drop index new_old_prdt_1;
drop  table new_old_prdt;
create table new_old_prdt(
	pact_no varchar2(20),--��ݺ�
	ac_no varchar2(19),--�˺�
	ac_name varchar2(60),--����
	old_prdt_no varchar2(3),--�ɲ�Ʒ��
	old_prdt_name varchar2(60),--�ɲ�Ʒ����
	new_prdt_no varchar2(3),--�²�Ʒ��
	new_prdt_name varchar2(60),--�²�Ʒ����
	id_no varchar2(20),--֤������
	bj_amt number(16,2),--�˻����
	in_lo_intst number(16,2),--����ǷϢ
	out_lo_intst number(16,2),--����ǷϢ
	sts varchar2(1) --״̬-0δ���� 1-������
)tablespace users;
create	 unique index new_old_prdt_1 on new_old_prdt(ac_no,pact_no );
