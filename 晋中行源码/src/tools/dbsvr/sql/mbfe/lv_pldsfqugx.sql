/**    ����ǩԼ��ϵ���֪ͨ�Ǽǲ� **/
drop index lv_pldsfqugx_idx;
drop table lv_pldsfqugx;
create table lv_pldsfqugx(
    pkgno         varchar2(3)   ,     /** ����š�           **/
    orderno       varchar2(8)   ,     /** ������š�         **/
    wt_date       number(9)     ,     /** ��ί�����ڡ�       **/
    br_no         varchar2(12)  ,     /** �����״����е��к� **/
    or_br_no      varchar2(12)  ,     /** ������             **/
    packid        varchar2(8)   ,     /** ��id               **/
    xyno          varchar2(60 ) ,     /** ��ͬ��Э�飩��     **/
    xy_ind        varchar2(1  ) ,     /** ��ͬ��Э�飩����   **/
    ch_ind        varchar2(1  ) ,     /** �����ʽ           **/
    pay_br_no     varchar2(12 ) ,     /** �������к�         **/
    pay_qs_no     varchar2(12 ) ,     /** �����������к�     **/
    pay_opn_br_no varchar2(12 ) ,     /** �����˿������к�   **/
    pay_ac_no     varchar2(32 ) ,     /** �������˺�         **/
    pay_name      varchar2(60 ) ,     /** ����������         **/
    pay_addr      varchar2(60 ) ,     /** �����˵�ַ         **/
    content       varchar2(60 ) ,     /** ����               **/
    sts           varchar2(2)     ,   /** ����״̬           **/
    beg_sts       varchar2(2)     ,   /** ��ʼ����״̬       **/
    operlevel     varchar2(1)     ,   /** ���ȼ�             **/
    /** 40506����ִ **/                                      
    re_dsf_date   number(9) ,         /** ���ո����Ĺ�����   **/
    repstat       varchar2(2 ) ,      /** ҵ���ִ״̬       **/
    re_content2   varchar2(60)        /** ����               **/
)tablespace users;
create unique index lv_pldsfqugx_idx on lv_pldsfqugx(orderno,wt_date,or_br_no) tablespace indx;    
