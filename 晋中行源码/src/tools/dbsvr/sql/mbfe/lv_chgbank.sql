/****Ʊ�����������ϵ��****/
drop index lv_chgbank_idx;
drop table lv_chgbank;
create table lv_chgbank(
	bakcode		varchar2(12),	/**Ʊ������***/
	agecode		varchar2(12),	/**�����к�***/
	oagecode	varchar2(12),	/**ԭ�����к�***/
	stat		varchar2(1),	/**���״̬**/
	effday		varchar2(8),	/**��Ч����***/
	filler   	varchar2(60) 	/**��ע***/
)tablespace users;
create unique index lv_chgbank_idx on lv_chgbank(bakcode);
