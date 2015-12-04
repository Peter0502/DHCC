/******************************************************/
/** oldnewacno:新旧科目对照关系                      **/
/** 创建日期：2003.03.13  by ljx                     **/
/******************************************************/
drop index   oldnewacdef_idx;
drop table  oldnewacdef;
create table oldnewacdef
(
	oacno   varchar(7),   /**旧科目**/
	nacno   varchar(7),   /**新科目**/
	acname	varchar(30),  /**科目名称**/
	dscpt	varchar(30)  /**旧科目说明**/
);
create unique index oldnewacdef_idx on oldnewacdef(oacno) tablespace indx;
