/******************************************************/
/** oldnewbrno:�¾��кŶ��չ�ϵ                      **/
/** �������ڣ�2003.03.27  by ljx                     **/
/******************************************************/
drop index   oldnewbrno_idx;
drop table  oldnewbrno;
create table oldnewbrno 
(
	obrno   varchar(30),		/**������**/
	nbrno   varchar(7)      	/**������**/
);
create unique index oldnewbrno_idx on oldnewbrno(obrno) tablespace indx;
