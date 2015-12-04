drop index dc_acc_rel_idx;
drop  table dc_acc_rel;
create table dc_acc_rel(
	dc_code varchar2(4),
	mo varchar2(20),
	acc_hrt varchar2(7),
	data_code varchar2(4) 
)tablespace users;
create	 index dc_acc_rel_idx on dc_acc_rel( dc_code)tablespace indx;
