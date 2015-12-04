/****小额批量服务控制表****/
drop index lv_oths_idx;
drop table lv_oths;
create table lv_oths(
   	wtday  		Varchar2(8) , /** 委托日期	 **/
	orbrno		varchar2(12),
	cmtno		varchar2(3),
   	orderno  	Varchar2(8) , /** 交易序号	 **/
	stat		varchar2(1),
	lwflg		varchar2(1),	/**0-往,1-来***/
   	brno  		Varchar2(7) , /** 交易机构	 **/
	content		varchar2(2000),	/****内容****/
	filler		varchar2(60)
)tablespace users;
create unique index lv_oths_idx on lv_oths(wtday,orderno);

