/* ���֧��ϵͳ������ */
drop index hv_syspara_idx;
drop table hv_syspara;
create table hv_syspara(
    sdate      varchar2(8),    /* ��Чʱ�� */
    updtype    varchar2(1),    /* �������� 1���� 2��� 3ע��*/
    startamt   number(15,2),   /* ����� */
    timedc     varchar2(8),    /* ʱ�����ݴ��� */
    tdcname    varchar2(20),   /* ʱ���������� */
    updtdc     varchar2(18),   /* ʱ�����ݸ���ֵ */
    updmethod  varchar2(1),    /* ���·�ʽ 1����������Ч2����ָ��������Ч3�·���
����Ч4�·�ָ��������Ч*/
    datatype   varchar2(1)     /* ��������1.202ϵͳ����ʱ����� 2.205���ҵ���
���� */
)tablespace users;
create unique index hv_syspara_idx on hv_syspara(sdate,updtype,timedc);

