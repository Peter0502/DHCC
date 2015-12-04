/******************************************************/
/** oldnewbrno:新旧行号对照关系                      **/
/** 创建日期：2003.03.27  by ljx                     **/
/******************************************************/
drop index   oldnewbrno_idx;
drop table  oldnewbrno;
create table oldnewbrno 
(
	obrno   varchar(30),		/**旧所号**/
	nbrno   varchar(7)      	/**新所号**/
);
create unique index oldnewbrno_idx on oldnewbrno(obrno) tablespace indx;
