drop index gl_note_1;
drop  table gl_note;
create table gl_note(
	br_no varchar2(5),
	note_type varchar2(3),
	"date" number(9),
	cr_bal number(16,2),
	lcd_bal number(16,2),
	rdd_cnt number(9),
	rcd_cnt number(9),
	rdd_amt number(16,2),
	rcd_amt number(16,2),
	tdcr_bal number(16,2),
	tddr_cnt number(9),
	tdcr_cnt number(9),
	tddr_amt number(16,2),
	tdcr_amt number(16,2),
	mcr_bal number(16,2),
	mdr_cnt number(9),
	mcr_cnt number(9),
	mdr_amt number(16,2),
	mcr_amt number(16,2),
	qcr_bal number(16,2),
	qdr_cnt number(9),
	qcr_cnt number(9),
	qdr_amt number(16,2),
	qcr_amt number(16,2),
	ycr_bal number(16,2),
	ydr_cnt number(9),
	ycr_cnt number(9),
	ydr_amt number(16,2),
	ycr_amt number(16,2) 
)tablespace users;
create	 unique index gl_note_1 on gl_note( br_no,note_type)tablespace indx;
