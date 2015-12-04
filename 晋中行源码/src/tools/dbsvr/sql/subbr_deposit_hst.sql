drop  table subbr_deposit_hst;
create table subbr_deposit_hst(
	up_br_no varchar2(5),
	opn_br_no varchar2(5),
	ex_year number(9),
	ex_month number(9),
	ac_id number(9),
	ac_seqn number(6),
	monthb_bal number(16,2),
	bal number(16,2),
	mon_increment number(16,2),
	month_addbal number(16,2),
	season_addbal number(16,2),
	year_addbal number(16,2),
	draw_deposit varchar2(4),
	client_mgr varchar2(4) 
)tablespace users;
