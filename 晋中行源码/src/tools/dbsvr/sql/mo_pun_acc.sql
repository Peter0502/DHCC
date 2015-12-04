drop index mo_pun_acc_idx;
drop  table mo_pun_acc;
create table mo_pun_acc(
	ac_id number(9),
	ac_seqn number(6),
	sts number(9),
	beg_date number(9),
	end_date number(9),
	beg_tel varchar2(4),
	beg_br_no varchar2(5),
	end_tel varchar2(4),
	end_br_no varchar2(5),
	brf varchar2(50) 
)tablespace users;
create	 unique index mo_pun_acc_idx on mo_pun_acc( ac_id,ac_seqn,sts)tablespace indx;
