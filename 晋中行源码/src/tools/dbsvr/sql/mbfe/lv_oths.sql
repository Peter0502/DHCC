/****С������������Ʊ�****/
drop index lv_oths_idx;
drop table lv_oths;
create table lv_oths(
   	wtday  		Varchar2(8) , /** ί������	 **/
	orbrno		varchar2(12),
	cmtno		varchar2(3),
   	orderno  	Varchar2(8) , /** �������	 **/
	stat		varchar2(1),
	lwflg		varchar2(1),	/**0-��,1-��***/
   	brno  		Varchar2(7) , /** ���׻���	 **/
	content		varchar2(2000),	/****����****/
	filler		varchar2(60)
)tablespace users;
create unique index lv_oths_idx on lv_oths(wtday,orderno);

