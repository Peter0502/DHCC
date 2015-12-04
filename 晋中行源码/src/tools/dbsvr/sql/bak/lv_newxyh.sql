/******************************************************************************/
/* Table Name:  LV_NEWXYH �¶෽Э��ű�                                         */
/* Location:    On region server                                              */
/* Index:       xyno,payactno                                                  */
/* Create for   XiaoE                        2005/12/08                          */
/******************************************************************************/
drop index lv_newxyh_idx;
drop table lv_newxyh;
CREATE TABLE lv_newxyh(
    xyno         varchar2(60),    /* �෽Э���,�������� */
    pay_ac_no    varchar2(32),    /* �����ʺ�<����>    */
    pay_name     varchar2(60),    /* �����<����> */
    pay_no       varchar2(12),    /* �����˿�����*/
    cashxyno     varchar2(60),    /* �տ���Э���,������� */
    cash_ac_no   varchar2(32),    /* �տ����ʺ� */
    cash_name    varchar2(60),    /* �տ��˻��� */
    cash_no      varchar2(12),    /* �տ����к� */
    yw_type      varchar2(5),     /* ҵ������ */
    ywlist       varchar2(200),   /* ��ϸҵ�������б�**/
    limitamt     number(17,2),    /* �����޶� */
    br_no        varchar2(7),     /* ¼����� */
    last_date    number(8),       /* ����޸����� */
    tel          varchar2(6),     /* ����޸Ĳ���Ա�� */
    flag         varchar2(1),     /* ��־*/
    filler       varchar2(60)     /* ��ע */
)tablespace users;
CREATE UNIQUE INDEX lv_newxyh_idx on lv_newxyh(xyno,pay_ac_no) tablespace indx;
