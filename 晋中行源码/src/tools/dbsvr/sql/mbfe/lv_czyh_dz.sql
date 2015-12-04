/***С��������ж���***/
drop index lv_czyh_dz_idx;
drop  table lv_czyh_dz;
create table lv_czyh_dz(
	lw_ind               varchar2(1),   /*������־ 1-���ʣ�2-����*/
	pack_date            number(8),     /*������*/
	pack_id              varchar2(8),   /*�����*/ 
	pack_type            varchar2(3),   /*������*/ 
	check_cnt            number(6),     /*����*/ 
	check_amt            number(16,2),  /*���*/
	check_flag           varchar2(1),   /*���˱�־ 1-��ƽ��2-δƽ*/ 
	pay_qs_no            varchar2(12),  /*�����������к�*/
	tx_date              number(8),     /*ϵͳ����*/
	br_no                varchar2(5)    /*�Է�����*/
)tablespace users;
create	 unique index lv_czyh_dz_idx on lv_czyh_dz( lw_ind,pack_date,pack_id)tablespace indx;
