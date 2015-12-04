drop index acno_hst_log_idx;
 drop index acno_hst_log_idx1;
drop  table acno_hst_log;
create table acno_hst_log(
	ac_type		varchar2(1),	/*�ʻ����� 1����  2 ����  3 ����***/
	ac_id 		number(9),	/*�ʺű�־��**/
	ac_seqn 	number(6),	/*�ʺ����**/
	hst_cnt 	number(9),	/*�ʺ���ϸ����**/
	tx_date		number(9),	/*��������**/
	br_no		varchar2(5),	/*��������**/
	note_type 	varchar2(3),	/*ƾ֤����**/
	note_no 	varchar2(16),	/*ƾ֤����**/
	tel 		varchar2(4),	/*������Ա**/
	tx_amt 		number(16,2),	/*���׽��**/
	bal 		number(16,2),	/*�ʻ����**/
	dc_ind		varchar2(1),	/*����������**/
	print_flag	varchar2(1),	/*��ӡ��־ 0 δ��ӡ 1 ��ӡ 2 ��ӡ�������Ѿ���ӡ�����һ����¼***/
	pagecnt		number(6),	/*������¼��ӡ�ڵڼ�ҳ**/
	brf 		varchar2(20),	/*ժҪ****/
	filler		varchar2(20),	/*��ע1**/
	filler2		varchar2(40)	/*��ע2**/
)tablespace users;
create	 index acno_hst_log_idx on acno_hst_log( ac_id,ac_seqn,hst_cnt) tablespace indx;
create   index acno_hst_log_idx1 on acno_hst_log(ac_id, print_flag) tablespace indx;
