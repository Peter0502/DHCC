/* create by lanzhisheng - 20050906 - HongDong  */
/* MISʹ�õ��ۺ�ҵ����ʲ���ծ�������������  */
drop index mis_iosreportdatatmp_idx;
drop index mis_iosreportdatatmp_idx2;
drop table mis_iosreportdatatmp;
create table mis_iosreportdatatmp
(
        brno         varchar2(7) NOT NULL,      /* �к�     */
        pigday       varchar2(8) NOT NULL,      /* �鵵���� */
        flag         number(2,0) NOT NULL,      /* ��������־ 1-�ʲ���ծ��
                                                   2-����� 3-�±���*/
        reprownum    number(4,0) NOT NULL,      /* �д�     */
        name         varchar2(60) NOT NULL,     /* ��Ŀ���� */
        tdbal        varchar(18),               /* ������� */
        ydbal        varchar(18),               /* ������� */
        tmbal        number(20,2),              /* ���ڷ����� */
        tdmbal       number(20,2),              /* ���ڽ跽������ */
        tcmbal       number(20,2),              /* ���ڴ��������� */
        cdcnt        number(8)   ,              /*{�跽�ֽ����}   */
        cdamt        number(16,2),              /*{�跽�ֽ���}   */
        cccnt        number(8)   ,              /*{�����ֽ����}   */
        ccamt        number(16,2),              /*{�����ֽ���}   */
        mdcnt        number(8)   ,              /*{�跽ת�ʱ���}   */
        mdamt        number(16,2),              /*{�跽ת�ʽ��}   */
        mccnt        number(8)   ,              /*{����ת�ʱ���}   */
        mcamt        number(16,2),              /*{����ת�ʽ��}   */
        fillter      varchar(10)                /* ��ע */
)tablespace users;
create unique index mis_iosreportdatatmp_idx on mis_iosreportdatatmp(brno,pigday,flag,reprownum) tablespace indx;
create index mis_iosreportdatatmp_idx2 on mis_iosreportdatatmp(brno,pigday,flag,name) tablespace indx;

