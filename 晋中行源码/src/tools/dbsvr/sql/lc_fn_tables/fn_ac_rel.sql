drop table fn_ac_rel;
create table fn_ac_rel
(
	prdt_code	varchar2(20),	/*��Ʋ�Ʒ����				*/
	qs_ac_no	varchar2(24),	/*����ʽ������˻�	�ڲ��ˣ��ǽ跽  */
	wt_ac_no	varchar2(24),	/*ί��Ͷ������˻�	�ڲ��ˣ��Ǵ���  */
    sy_ac_no    varchar2(24)    /*�����ѡ���ط����˺�*/
)tablespace users;
create unique index fn_ac_rel_1 on fn_ac_rel(prdt_code) tablespace indx;
