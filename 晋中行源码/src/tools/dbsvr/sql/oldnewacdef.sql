/******************************************************/
/** oldnewacno:�¾ɿ�Ŀ���չ�ϵ                      **/
/** �������ڣ�2003.03.13  by ljx                     **/
/******************************************************/
drop index   oldnewacdef_idx;
drop table  oldnewacdef;
create table oldnewacdef
(
	oacno   varchar(7),   /**�ɿ�Ŀ**/
	nacno   varchar(7),   /**�¿�Ŀ**/
	acname	varchar(30),  /**��Ŀ����**/
	dscpt	varchar(30)  /**�ɿ�Ŀ˵��**/
);
create unique index oldnewacdef_idx on oldnewacdef(oacno) tablespace indx;
