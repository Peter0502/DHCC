drop  table zjgl_mst;
create table zjgl_mst(
    opn_br_no      varchar2(5),   /* ����������        */
    cur_no         varchar2(2),   /* ����              */
    acc_no         varchar2(7),   /* ��Ŀ�� */
    acc_hrt        varchar(5),    /* ��Ŀ������        */
    dr_bal         number(16,2),  /* �跽���          */
    cr_bal         number(16,2),  /* �������          */
    ys_dr_bal      number(16,2),  /* ���ս跽���      */
    ys_cr_bal      number(16,2),  /* ���մ������      */
    sts            varchar2(1),   /* �ʻ�״̬          */
    opn_date       number(8),     /* ��������          */
    ic_date        number(8),     /* ��Ϣ����          */
    end_date       number(8),     /* ��������          */
    cls_date       number(8),     /* ��������          */
    lst_date       number(8),     /* �ϱʷ�������      */
    intst_knd      varchar2(10),  /* ��Ϣ����          */
    over_rate_type varchar2(3),   /* �������ʱ��      */
    rate_type      varchar2(3),   /* ���ʱ��          */
    rate           number(8,5),   /* ����              */
    over_rate      number(8,5),   /* ��������          */
    dr_intst_acm   number(19,2),  /* ��Ϣ����          */
    cr_intst_acm   number(19,2),  /* ��Ϣ����          */
    hst_cnt        number(8),     /* ��ϸ���ܱ���      */
    hst_pg         number(5),     /* ��ϸ����ҳҳ��    */
    od_ind         varchar2(1),   /* ͸֧��־          */
    name           varchar2(50),  /* ����              */
    lim_amt        number(16,2),  /* �����޶�          */
    lim_bal        number(16,2),  /* �ۼƽ��׶�        */
    lim_term       varchar2(2),   /* �޶�����          */
    lim_beg_date   number(8),     /* �޶ʼ����      */
    lim_end_date   number(8),     /* �޶��������      */
    tmp1           varchar2(50),  /* ��ע1             */
    tmp2           varchar2(50),  /* ��ע2             */
    tmp_amt1       number(19,2),  /* ��ע���1         */
    tmp_amt2       number(19,2)   /* ��ע���2         */
)tablespace users;
create   unique index zjgl_mst_1 on zjgl_mst( opn_br_no,cur_no,acc_no)tablespace indx;
