/**	定期借（贷）记业务种类编码表**/
drop index lv_category_idx;
drop table lv_category;
create table lv_category(
	definition	varchar2(1),
	category_no varchar2(2),
	name		varchar2(60),
	orderno		varchar2(2),
	code		varchar2(5),
	state		varchar2(1),
	remark		varchar2(60)
)tablespace users;
create unique index lv_category_idx on lv_category(code) tablespace indx;

