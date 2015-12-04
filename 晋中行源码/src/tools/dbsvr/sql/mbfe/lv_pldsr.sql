/**    ��������ҵ����յǼǲ� **/
drop index lv_pldsr_idx;
drop table lv_pldsr;
create table lv_pldsr(
    packid           varchar2(8)  ,     /** ����š�                **/
    orderno          varchar2(8)  ,     /** ������š�              **/
    wt_date          number(9)    ,     /** ��ί�����ڡ�            **/
    br_no            varchar2(12) ,     /** �����״����е��кš�    **/
    or_br_no         varchar2(12) ,     /** ������                  **/
    dsf_date         number(9)    ,     /** ���ո����Ĺ�����        **/
    re_lim_date      number(2)    ,     /** ��ִ���ޣ�������        **/
    cash_br_no       varchar2(12 ) ,    /** �տ����к�              **/
    cash_opn_br_no   varchar2(12 ) ,    /** �տ��˿������к�        **/
    cash_ac_no       varchar2(32 ) ,    /** �տ����˻�              **/
    cash_name        varchar2(60 ) ,    /** �տ�������              **/
    cash_addr        varchar2(60 ) ,    /** �տ��˵�ַ              **/
    yw_type          varchar2(5  ) ,    /** ҵ������                **/
    totamt           number(13,2)  ,    /** �����տ��ܽ��          **/
    pay_qs_no        varchar2(12 ) ,    /** �������������к�        **/
    pay_num          number(8  )   ,    /** ��������Ŀ              **/
    tx_mx_no         varchar2(8  ) ,    /** ��ϸ���,ȫ0ʱΪ����Ϣ  **/
    pay_br_no        varchar2(12 ) ,    /** �������к�              **/
    pay_opn_br_no    varchar2(12 ) ,    /** �����˿������к�        **/
    pay_ac_no        varchar2(32 ) ,    /** �������˺�              **/
    pay_name         varchar2(60 ) ,    /** ����������              **/
    pay_addr         varchar2(60 ) ,    /** �����˵�ַ              **/
    tx_amt           number(13,2)  ,    /** ���                    **/
    xyno             varchar2(60 ) ,    /** ��ͬ��Э�飩��          **/
    content          varchar2(60 ) ,    /** ����                    **/ 
    filename         varchar2(256) ,    /** ���ɵı����ļ���        **/ 
    sts              varchar2(2)   ,    /** ����״̬                **/
    beg_sts          varchar2(2)   ,    /** ��ʼ����״̬            **/    
    operlevel        varchar2(1)   ,    /** ���ȼ�                  **/    
    /** 40503����ִ **/
    repstat          varchar(2  )  ,    /** ��ִ״̬                **/
    succamt          number(13,2)  ,    /** �ɹ��տ��ܽ��          **/
    succnum          number(8  )   ,    /** �ɹ�����                **/
    re_content       varchar(60 )  ,    /** ����                    **/
    /** 40506����ִ **/
    re_dsf_date      number(9)     ,    /** ���ո����Ĺ�����        **/
    repstat2         varchar2(2 )  ,    /** ҵ���ִ״̬            **/
    re_content2      varchar2(60)       /** ����                    **/
)tablespace users;
create unique index lv_pldsr_idx on lv_pldsr(packid,orderno,wt_date,tx_mx_no)  tablespace indx;
