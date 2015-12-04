drop index bzj_js_ac_rel_idx;
drop  table bzj_js_ac_rel;
create table bzj_js_ac_rel(
	tx_br_no varchar2(5),
	bzj_ac_id number(9),
	bzj_ac_seqn number(6),
	js_ac_id number(9),
	js_ac_seqn number(6),
	tx_date number(9),
	trace_no number(9),
	tel varchar2(4),
	sts varchar2(1),
        filler varchar2(60) 
)tablespace users;
create	 index bzj_js_ac_rel_idx on bzj_js_ac_rel( bzj_ac_id,bzj_ac_seqn,sts)tablespace indx;
