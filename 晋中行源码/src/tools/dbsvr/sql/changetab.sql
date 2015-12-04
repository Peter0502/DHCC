/*   长治市交换行号对照表  */
drop index  changetab_idx;
drop table changetab;      
create table changetab(
    change_no        varchar2(4),    /***交换代码***/
    br_no            varchar2(5),    /***机构号****/
    br_name          varchar2(60),   /***机构名称***/
    address          varchar2(60),   /***地址***/
    phone            varchar2(20),   /***电话***/
    Postcode         varchar2(6),    /***邮编***/
    fax              varchar2(20),   /***传真***/
    telegrap         varchar2(8),    /***电报 ******/
    stat             varchar2(1)     /*** 1-正常 2-删除***/
);
create unique index changetab_idx on changetab(change_no);

