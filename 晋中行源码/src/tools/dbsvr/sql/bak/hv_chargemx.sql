/**    CMT552С���շ�֪ͨ���ĵǼǲ� **/
drop index hv_chargemx_idx;
drop table hv_chargemx;
create table hv_chargemx(
    br_no           Varchar2(7)    ,/**  �����ţ������кţ�                  **/
    in_date         Number(8)      ,/**  ���ף�����ϵͳ������                **/
    dtlid           Number(3)      ,/**  ��ϸ���                            **/
    chgtype         Varchar(1)     ,/**  �շ����� 1֧���� 2���� 3��Ϣ��      **/
    cmtno           Varchar2(3)    ,/**  Cmt���                             **/
    srflag          Varchar2(1)    ,/**  �շ���ϸ֮�շ���־0 ���� 1����      **/
    /** timespan        Varchar2(40)   , �Ʒ�ʱ���                          **/
    lowlimit        Varchar2(6)    ,/**  �շ���ϸ֮ʱ������                  **/
    upplimit        Varchar2(6)    ,/**  �շ���ϸ֮ʱ������                  **/
    mnum            Varchar2(10)   ,/**  �շ���ϸ֮��¼����                  **/
    price           Varchar2(6)    ,/**  �շ���ϸ֮�Ʒѵ��۵�λ              **/
    brate           Varchar2(6)    ,/**  �շ���ϸ֮�������ʵ�λ��            **/
    pecrate         Varchar2(6)    ,/**  �շ���ϸ֮ʱ���շѰٷֱ�            **/
    txamt           Varchar2(15)   ,/**  �շ���ϸ֮�Ʒѽ��                  **/
    prtflag         Varchar2(1)     /**  �շ���ϸ֮��ӡ��־0 δ��ӡ 1�Ѵ�ӡ  **/
)tablespace users;
create unique index hv_chargemx_idx on hv_chargemx(in_date,dtlid)tablespace indx;
