drop index mo_prdt_prot_idx;
drop  table mo_prdt_prot;
create table mo_prdt_prot(
	tx_br_no varchar2(5),
	prot_type varchar2(3),
	main_ac_id number(9),
	main_ac_seqn number(6),
	leave_amt number(16,2),
	turn_amt number(16,2),
	m3 number(6),
	m6 number(6),
	y1 number(6),
	y2 number(6),
	tx_date number(9),
	trace_no number(9),
	tel varchar2(4),
	sts varchar2(1) 
)tablespace users;
create	 index mo_prdt_prot_idx on mo_prdt_prot( tx_br_no,prot_type)tablespace indx;
