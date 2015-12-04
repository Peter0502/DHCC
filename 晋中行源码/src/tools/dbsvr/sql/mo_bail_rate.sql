drop index mo_bail_rate_idx;
drop  table mo_bail_rate;
create table mo_bail_rate(
	br_no varchar2(5),
	bail_type varchar2(2),
	pact_no varchar2(20),
	seq number(6),
	bail_ac_no varchar2(24),
	ac_no varchar2(24),
	amt number(16,2),
	ic_rate number(8,6),
	beg_date number(9),
	end_date number(9),
	intst_date number(9),
	in_trace_no number(9),
	end_trace_no number(9),
	intst_amt number(16,2),
	sts varchar2(1) 
)tablespace users;
create	 index mo_bail_rate_idx on mo_bail_rate( bail_type,pact_no,seq)tablespace indx;
