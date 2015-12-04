drop index mdm_unprt_hst_1;
drop index mdm_unprt_hst_2;
drop  table mdm_unprt_hst;
create table mdm_unprt_hst(
	trace_no number(9),
	trace_cnt number(9),
	tx_br_no varchar2(5),
	ac_id number(9),
	ac_seqn number(6),
	add_ind varchar2(1),
	tx_amt number(16,2),
	bal number(16,2),
	tx_code varchar2(4),
	tx_date number(9),
	hst_cnt number(9),
	tel varchar2(4),
	brf varchar2(10) 
)tablespace users;
create	 unique index mdm_unprt_hst_1 on mdm_unprt_hst( ac_id,tx_date,trace_no,trace_cnt)tablespace indx;
create	 index mdm_unprt_hst_2 on mdm_unprt_hst( ac_id,ac_seqn)tablespace indx;
