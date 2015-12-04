drop  table mas_phfe;
create table mas_phfe(
	seqn varchar2(32),
	report_br_no varchar2(20),
	report_br varchar2(60),
	id_card_no varchar2(20),
	name varchar2(60),
	nationality varchar2(5),
	tx_date varchar2(8),
	rpt_std_code varchar2(4),
	tx_code varchar2(32),
	add_ind varchar2(1),
	card_no varchar2(32),
	tx_ind varchar2(8),
	tw_bank_no varchar2(20),
	tw_bank_name varchar2(60),
	tw_name varchar2(60),
	tw_acc_no varchar2(32),
	tw_br_no varchar2(20),
	tw_tx_date varchar2(8),
	cur_no varchar2(3),
	tx_amt_orig varchar2(15),
	tx_amt_dollar varchar2(15),
	code varchar2(32),
	remark varchar2(20),
	extend varchar2(20) 
)tablespace users;
