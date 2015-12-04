drop index mo_loss_idx;
drop  table mo_loss;
create table mo_loss(
	ac_no varchar2(19),
	id_type varchar2(1),
	id_no varchar2(20),
	name varchar2(50),
	loss_ind varchar2(1),
	prop_book_loss varchar2(1),
	unloss_ind varchar2(1),
	opn_br_no varchar2(5),
	loss_date number(9),
	loss_br_no varchar2(5),
	loss_tel varchar2(4),
	loss_chk varchar2(4),
	loss_auth varchar2(4),
	unloss_date number(9),
	unloss_tel varchar2(4),
	unloss_chk varchar2(4),
	unloss_auth varchar2(4),
	new_ac_no varchar2(19),
	wrk_date number(9),
	trace_no number(9) 
)tablespace users;
create	 index mo_loss_idx on mo_loss( ac_no,unloss_ind)tablespace indx;
