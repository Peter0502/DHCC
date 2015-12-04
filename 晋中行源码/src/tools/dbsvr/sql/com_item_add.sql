/* ******************************************* */
/* ��Ŀ���ӱ�                                */
/* ��Ŀ��һЩ���ӵ���Ϣ��                      */
/* ******************************************* */
drop  index com_item_add_idx;
drop  table com_item_add;
create table com_item_add(
	acc_no       varchar2(7),  /* ��Ŀ��                   */
	acc_hrt      varchar2(5),  /* ��Ŀ������               */
	add_ind      varchar2(40), /* ���ӱ���Ʊ�־λ         */
        /* bit 2 - �ۻ����� 0-���ܿۻ� 1-�跽 2-���� 3 ˫��*/                             
        /* bit 1 - �Ƿ��ӡ���״�Ʊ 0- ����Ҫ��ӡ 1-��Ҫ��ӡ*/
        /* bit 0 - 0˵���������޶�,  ��0�����޶� 			*/
	dr_tx_amt    number(16,2), /* �跽��Ŀʹ�ö�           */
	cr_tx_amt    number(16,2), /* ������Ŀʹ�ö�           */
	dr_tx_cnt    number(6)   , /* �跽ʹ�ô���             */
	cr_tx_cnt    number(6)   , /* ����ʹ�ô���             */
	dr_limit_amt number(16,2), /* �跽�޶�                 */
	cr_limit_amt number(16,2), /* �����޶�                 */
	brf          varchar2(60)
)tablespace users;
create unique index com_item_add_idx on com_item_add(acc_no)tablespace indx;
