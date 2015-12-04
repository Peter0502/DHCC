/****打印满页帐附表***/
drop index print_full_page_idx;
drop table print_full_page;      
create table print_full_page(    
	ac_id		number(9),		/*客户ID*/
	hst_cnt		number(9),		/*当前最小的未打印明细的标志数*/
	pagecnt		number(9),		/*已经打印的满页帐的页数*/
	lst_bal		number(16,2),	/*上页最后一笔的余额*/
	trace_no	number(9),		/*当前最小未打印的记录的流水号*/
	tx_date		number(9),		/*当前最小未打印的记录的日期*/
	tx_time		number(9),		/*当前最小未打印的记录的时间*/
	filler1		varchar(10),	/*备用字段1*/
	filler2		varchar(20),	/*备用字段2*/
	filler3		varchar(30)		/*备用字段3*/
)tablespace users;
create unique index print_full_page_idx on print_full_page(ac_id) tablespace indx;
