drop table out_in_data;
create table out_in_data
(
	out_tx_code     varchar2(8),/* 外部交易代码 */
	in_tx_codeind   varchar2(8),/* 内部交易代码 */
	tx_name         varchar2(30) /* 交易名称 */
)tablespace users;
create  unique index out_in_data_1 on out_in_data(out_tx_code) tablespace indx;

