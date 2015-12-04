drop index hv_chkagemr_idx;
drop  table hv_chkagemr;
create table hv_chkagemr(
	orderno varchar2(8),
	pay_qs_no varchar2(12),
	br_no varchar2(7),
	tx_date number(9),
	cmtno varchar2(3),
	o_cmtno varchar2(3),
	tx_time number(9),
	cur_no varchar2(3),
	tx_amt number(15,2),
	other_amt number(15,2),
	jf_amt number(15,2),
	or_br_no varchar2(12),
	pay_no varchar2(12),
	pay_ac_no varchar2(32),
	payname varchar2(60),
	payaddress varchar2(60),
	cashqsactno varchar2(12),
	acbrno varchar2(12),
	cashno varchar2(12),
	cash_ac_no varchar2(32),
	cash_name varchar2(60),
	cash_addr varchar2(60),
	yw_type varchar2(2),
	sendco varchar2(4),
	receco varchar2(4),
	operlevel varchar2(1),
	obrno varchar2(12),
	voctype varchar2(2),
	vocnum varchar2(12),
	p_date number(9),
	p_num varchar2(8),
	hp_type varchar2(1),
	pass_wd varchar2(10),
	inst_rate number(8,5),
	term number(9),
	o_tx_date varchar2(8),
	ou_tx_date varchar2(8),
	o_orderno_hv varchar2(8),
	o_tx_type varchar2(1),
	p_orderno_hv varchar2(8),
	dc varchar2(1),
	pay_date number(9),
	intst number(15,2),
	reason varchar2(60),
	repson varchar2(1),
	chk_flg varchar2(1),
	brf1 varchar2(30),
	brf2 varchar2(30) 
)tablespace users;
create	 unique index hv_chkagemr_idx on hv_chkagemr( orderno,or_br_no)tablespace indx;
