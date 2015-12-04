/*   �����н����кŶ��ձ�  */
drop index  changetab_idx;
drop table changetab;      
create table changetab(
    change_no        varchar2(4),    /***��������***/
    br_no            varchar2(5),    /***������****/
    br_name          varchar2(60),   /***��������***/
    address          varchar2(60),   /***��ַ***/
    phone            varchar2(20),   /***�绰***/
    Postcode         varchar2(6),    /***�ʱ�***/
    fax              varchar2(20),   /***����***/
    telegrap         varchar2(8),    /***�籨 ******/
    stat             varchar2(1)     /*** 1-���� 2-ɾ��***/
);
create unique index changetab_idx on changetab(change_no);

