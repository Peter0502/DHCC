drop index mo_righ_invet_idx;
drop  table mo_righ_invest;
create table mo_righ_invest(
	ac_id number(9),
	br_no varchar2(5),
	bkrl_code varchar2(3),
	pact_no varchar2(20),
	invst_type varchar2(1),
	num number(9),
	amt number(16,2),
	incm_bal number(16,2),
	cif_code varchar2(20),
	name varchar2(50),
	wrk_date number(9),
	trace_no number(9),
	sts varchar2(1) 
)tablespace users;
create	 unique index mo_righ_invet_idx on mo_righ_invest( pact_no)tablespace indx;
