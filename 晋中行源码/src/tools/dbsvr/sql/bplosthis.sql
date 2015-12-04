/**************************************************/
/* TABLE NAME:bplosthis ��Ʊ��ʧ��Ϣ��            */
/* Description  : Postal Order Saving Master File */
/* Table index  : pono,txday,                     */
/* create by Wei XiaoJian: 2006-11-9              */
/***************************************************/
drop index  bplosthis_idx;
drop table bplosthis;
create table bplosthis(
    lostday           number(8),    /***��ʧ����***/
    pono              varchar2(20), /***��Ʊ����****/
    lostcnt           number(6),    /***��ʧ����****/
    signday           number(8),    /***��Ʊ����***/
    unlostday         number(8),    /***��ʧ/�������***/
    lostbrno	      Varchar2(7),  /***��ʧ���׻���***/
    unlostbrno	      Varchar2(7),  /***���ʧ���׻���***/
    inflag            varchar2(1),  /*��Ʊ�б�0.���б�Ʊ1.���б�Ʊ */
    bptype            varchar2(1),  /*��Ʊ���0.ת�ʱ�Ʊ1.�ֽ�Ʊ */
    paybrno           Varchar2(12), /**ǩ�����к�***/
    paybrname         varchar2(60), /** ǩ��������**/
    cashbrno          varchar2(12), /** �Ҹ����к�**/
    cashbrname        varchar2(60), /** �Ҹ�������**/
    avbal             Number(16,2), /***Ʊ����***/
    feeflag           varchar2(1),  /** �����ѱ�־ 0-�� 1-���� */
    feeamt            number(16,2), /** �����ѽ�� */
    stat              Varchar2(1),  /***�ʻ�״̬0--��ʧ1--���ʧ***/
    losttlrno         Varchar2(6),  /***��ʧ����Ա***/
    unlosttlrno       Varchar2(6),  /***��Ҳ���Ա***/
    filler            Varchar2(60)  /***��ע---���ԷŹ�ʧ����ϵ��ʽ****/
)tablespace users;
create unique index bplosthis_idx on bplosthis(pono,lostday,lostcnt) tablespace indx;
