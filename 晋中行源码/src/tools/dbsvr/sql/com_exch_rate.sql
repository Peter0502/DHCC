drop  table com_exch_rate;
create table com_exch_rate(
	cur_no1 varchar2(2),
	cur_no2 varchar2(2),
	unit number(9),
	od_buy number(10,4),
	od_sale number(10,4),
	dt_buy number(10,4),
	dt_sale number(10,4),
	mdl number(10,4),
	"date" number(9) 
)tablespace users;
