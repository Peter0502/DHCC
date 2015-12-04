/****票交机构代理关系表****/
drop index lv_chgbank_idx;
drop table lv_chgbank;
create table lv_chgbank(
	bakcode		varchar2(12),	/**票交机构***/
	agecode		varchar2(12),	/**代理行号***/
	oagecode	varchar2(12),	/**原代理行号***/
	stat		varchar2(1),	/**变更状态**/
	effday		varchar2(8),	/**生效日期***/
	filler   	varchar2(60) 	/**备注***/
)tablespace users;
create unique index lv_chgbank_idx on lv_chgbank(bakcode);
