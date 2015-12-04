/**    �������ճ����Ǽǲ� **/
drop index lv_pldscx_idx;
drop table lv_pldscx;
create table lv_pldscx(
    packid           varchar2(8)  ,     /** ����š�          **/
    orderno          varchar2(8)  ,     /** ������š�        **/
    wt_date          number(9)    ,     /** ��ί�����ڡ�      **/
    br_no            varchar2(12) ,     /** �����״����е��к� */
    or_br_no         varchar2(12) ,     /** ������            **/
    o_wt_date        number(9 )   ,     /** ԭ����ί������    **/
    o_orderno        varchar2(8 ) ,     /** ԭ������Ϣҵ�����**/
    o_yw_type        varchar2(5 ) ,     /** ԭ��ҵ������      **/
    dsf_date         number(9 )   ,     /** ���ո���������    **/
    content          varchar2(60) ,     /** ����              **/
    sts              varchar2(2)  ,     /** ����״̬          **/
    beg_sts          varchar2(2)  ,     /** ��ʼ����״̬      **/
    operlevel        varchar2(1)  ,     /** ���ȼ�            **/
    /** 40506����ִ **/
    re_dsf_date      number(9 )   ,     /** ���ո����Ĺ�����  **/
    repstat2         varchar2(2 )   ,     /** ҵ���ִ״̬      **/
    re_content2      varchar2(60)       /** ����              **/
)tablespace users;
create unique index lv_pldscx_idx on lv_pldscx(packid,orderno,wt_date)tablespace indx;
