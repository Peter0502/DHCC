drop table out_in_data;
create table out_in_data
(
	out_tx_code     varchar2(8),/* �ⲿ���״��� */
	in_tx_codeind   varchar2(8),/* �ڲ����״��� */
	tx_name         varchar2(30) /* �������� */
)tablespace users;
create  unique index out_in_data_1 on out_in_data(out_tx_code) tablespace indx;

