#include "find_debug.h" 
/*************************************************
* �� �� ��: pubf_base_mystery
* ������������Ѻ����ͼ��
* ��    ��:	jack
* ������ڣ�2005��04��04��
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
*************************************************/

#include "stdio.h"
#define EXTERN
#include "public.h"
#include "ln_lo_c.h"
static char tmpbuf[3000];
extern unsigned char *pub_com_des_pack(unsigned char * scode,unsigned char * pack,int packlen,unsigned char * mkey);
/**********************************************************************
 * �� �� ����   dd_mst_con
 * �������ܣ�   ���������ļ�����ת��һ���ַ�����
 * ����/ʱ�䣺	jack	2003��04��04��
 *
 * ������
 *     ����: 	һ���������ļ���¼
 *     ���: 	
 *     ����: 
 *
 * �޸���ʷ��
 *
********************************************************************/
int dd_mst_con ( struct dd_mst_c tDd_mst, char *instr )
{
	int	ret;
	char tmpstr[100];

	struct dd_mst_c sDd_mst;

return(0);
	MEMSET_DEBUG( &sDd_mst, 0x00, sizeof(sDd_mst) );

	MEMCPY_DEBUG( &sDd_mst, &tDd_mst, sizeof(struct dd_mst_c) );
vtcp_log("aaaaaaaaaaaaaaaaaaa");
	memset(tmpbuf,'\0',sizeof(tmpbuf));
	/*&tmpbuf=instr;*/
vtcp_log("bbbbbbbbbbbbbbbbaaa");

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%ld", sDd_mst.ac_id );
	pub_base_strpack( tmpstr );
	strcat( tmpbuf, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%d", sDd_mst.ac_seqn );
	pub_base_strpack( tmpstr );
	strcat( tmpbuf, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%s", sDd_mst.opn_br_no );
	pub_base_strpack( tmpstr );
	strcat( tmpbuf, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%s", sDd_mst.prdt_no );
	pub_base_strpack( tmpstr );
	strcat( tmpbuf, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%.2f", sDd_mst.bal );
	pub_base_strpack( tmpstr );
	strcat( tmpbuf, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%.2f", sDd_mst.acbo_bal );
	pub_base_strpack( tmpstr );
	strcat( tmpbuf, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%.2f", sDd_mst.ys_bal );
	pub_base_strpack( tmpstr );
	strcat( tmpbuf, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%.2f", sDd_mst.hst_bal );
	pub_base_strpack( tmpstr );
	strcat( tmpbuf, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%.5f", sDd_mst.rate );
	pub_base_strpack( tmpstr );
	strcat( tmpbuf, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%.5f", sDd_mst.flt_ratio );
	pub_base_strpack( tmpstr );
	strcat( tmpbuf, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%.2f", sDd_mst.intst_acm );
	pub_base_strpack( tmpstr );
	strcat( tmpbuf, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%s", sDd_mst.cash_ind );
	pub_base_strpack( tmpstr );
	strcat( tmpbuf, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%s", sDd_mst.sttl_type );
	pub_base_strpack( tmpstr );
	strcat( tmpbuf, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%s", sDd_mst.ac_type );
	pub_base_strpack( tmpstr );
	strcat( tmpbuf, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%s", sDd_mst.intst_type );
	pub_base_strpack( tmpstr );
	strcat( tmpbuf, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%ld", sDd_mst.hst_cnt );
	pub_base_strpack( tmpstr );
	strcat( tmpbuf, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%ld", sDd_mst.hst_pg );
	pub_base_strpack( tmpstr );
	strcat( tmpbuf, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%ld", sDd_mst.opn_date );
	pub_base_strpack( tmpstr );
	strcat( tmpbuf, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%ld", sDd_mst.ic_date );
	pub_base_strpack( tmpstr );
	strcat( tmpbuf, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%ld", sDd_mst.lst_date );
	pub_base_strpack( tmpstr );
	strcat( tmpbuf, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%ld", sDd_mst.val_date );
	pub_base_strpack( tmpstr );
	strcat( tmpbuf, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%s", sDd_mst.odtt_ind );
	pub_base_strpack( tmpstr );
	strcat( tmpbuf, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%s", sDd_mst.ac_sts );
	pub_base_strpack( tmpstr );
	strcat( tmpbuf, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%s", sDd_mst.hold_sts );
	pub_base_strpack( tmpstr );
	strcat( tmpbuf, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%.2f", sDd_mst.hold_amt );
	pub_base_strpack( tmpstr );
	strcat( tmpbuf, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%.2f", sDd_mst.ctl_amt );
	pub_base_strpack( tmpstr );
	strcat( tmpbuf, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%s", sDd_mst.od_ind );
	pub_base_strpack( tmpstr );
	strcat( tmpbuf, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%s", sDd_mst.draw_pwd );
	pub_base_strpack( tmpstr );
	strcat( tmpbuf, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%d", sDd_mst.cif_no );
	pub_base_strpack( tmpstr );
	strcat( tmpbuf, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%s", sDd_mst.name );
	pub_base_strpack( tmpstr );
	strcat( tmpbuf, tmpstr );

	/* sprintf( acErrMsg,"------����-----------ƴ���ַ���Ϊ[%s]", str);
	 WRITEMSG
	*/


 	return (0);
}

/**********************************************************************
 * �� �� ����   td_mst_con
 * �������ܣ�   ���������ļ�����ת��һ���ַ�����
 * ����/ʱ�䣺	jack	2003��04��04��
 *
 * ������
 *     ����: 	һ���������ļ���¼
 *     ���: 	
 *     ����: 
 *
 * �޸���ʷ��
 *
********************************************************************/
int td_mst_con ( struct td_mst_c tTd_mst, char *str )
{
	int	ret;
	char tmpstr[100];

	struct td_mst_c sTd_mst;

	MEMSET_DEBUG( &sTd_mst, 0x00, sizeof(sTd_mst) );

	MEMCPY_DEBUG( &sTd_mst, &tTd_mst, sizeof(struct td_mst_c) );

	str[0] = '\0';

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%ld", sTd_mst.ac_id );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%d", sTd_mst.ac_seqn );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%s", sTd_mst.opn_br_no );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%s", sTd_mst.prdt_no );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%.2f", sTd_mst.bal );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%.2f", sTd_mst.acbo_bal );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%.2f", sTd_mst.ys_bal );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%.2f", sTd_mst.hst_bal );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%.5f", sTd_mst.rate );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%.5f", sTd_mst.flt_ratio );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%.2f", sTd_mst.intst_acm );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%s", sTd_mst.intst_type );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%ld", sTd_mst.opn_date );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%ld", sTd_mst.ic_date );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%ld", sTd_mst.mtr_date );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%ld", sTd_mst.lst_date );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%.2f", sTd_mst.td_amt );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%d", sTd_mst.ttl_cnt );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%d", sTd_mst.tx_cnt );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%ld", sTd_mst.hst_cnt );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%s", sTd_mst.odtt_ind );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%s", sTd_mst.ac_sts );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%s", sTd_mst.hold_sts );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%.2f", sTd_mst.hold_amt );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%.2f", sTd_mst.ctl_amt );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%s", sTd_mst.tfr_ind );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%d", sTd_mst.cif_no );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%s", sTd_mst.name );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	/*sprintf( acErrMsg,"------����-----------ƴ���ַ���Ϊ[%s]", str);
	 WRITEMSG
	 */

 	return (0);
}

/**********************************************************************
 * �� �� ����   ln_mst_con
 * �������ܣ�   ���������ļ�����ת��һ���ַ�����
 * ����/ʱ�䣺	jack	2003��04��04��
 *
 * ������
 *     ����: 	һ���������ļ���¼
 *     ���: 	
 *     ����: 
 *
 * �޸���ʷ��
 *
********************************************************************/
int ln_mst_con ( struct ln_mst_c tLn_mst, char *str )
{
	int	ret;
	char tmpstr[100];

	struct ln_mst_c sLn_mst;

	MEMSET_DEBUG( &sLn_mst, 0x00, sizeof(sLn_mst) );

	MEMCPY_DEBUG( &sLn_mst, &tLn_mst, sizeof(struct ln_mst_c) );

	str[0] = '\0';

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%ld", sLn_mst.ac_id );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%d", sLn_mst.ac_seqn );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%s", sLn_mst.opn_br_no );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%s", sLn_mst.prdt_no );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%.2f", sLn_mst.bal );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%.2f", sLn_mst.ys_bal );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%.2f", sLn_mst.hst_bal );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%.2f", sLn_mst.amt_lmt );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%.2f", sLn_mst.ttl_prvd_amt );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%.5f", sLn_mst.rate );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%.5f", sLn_mst.over_rate );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%.2f", sLn_mst.intst_acm );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%.2f", sLn_mst.in_lo_intst );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%.2f", sLn_mst.in_lo_acm );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%.2f", sLn_mst.out_lo_intst );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%.2f", sLn_mst.out_lo_acm );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%.2f", sLn_mst.cmpd_lo_intst );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%.2f", sLn_mst.cmpd_lo_acm );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%s", sLn_mst.cal_code );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%ld", sLn_mst.ic_date );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%ld", sLn_mst.opn_date );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%ld", sLn_mst.mtr_date );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%ld", sLn_mst.lst_date );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%s", sLn_mst.intst_type );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%s", sLn_mst.repay_type );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%d", sLn_mst.hst_cnt );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%d", sLn_mst.hst_pg );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%d", sLn_mst.repay_ac_id );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%s", sLn_mst.exp_ind );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%s", sLn_mst.exp_ind );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%.5f", sLn_mst.exp_rate );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%ld", sLn_mst.exp_mtr_date );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%s", sLn_mst.ac_sts );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%s", sLn_mst.five_sts );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%ld", sLn_mst.cif_no );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%s", sLn_mst.name );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%s", sLn_mst.pact_no );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	/*sprintf( acErrMsg,"------����-----------ƴ���ַ���Ϊ[%s]", str);
	 WRITEMSG
	 */

 	return (0);
}

/**********************************************************************
 * �� �� ����   in_mst_con
 * �������ܣ�   ���ڲ������ļ�����ת��һ���ַ�����
 * ����/ʱ�䣺	jack	2003��04��04��
 *
 * ������
 *     ����: 	һ���ڲ������ļ���¼
 *     ���: 	
 *     ����: 
 *
 * �޸���ʷ��
 *
********************************************************************/
int in_mst_con ( struct in_mst_c tIn_mst, char *str )
{
	int	ret;
	char tmpstr[100];

	struct in_mst_c sIn_mst;

	MEMSET_DEBUG( &sIn_mst, 0x00, sizeof(sIn_mst) );

	MEMCPY_DEBUG( &sIn_mst, &tIn_mst, sizeof(struct in_mst_c) );

	str[0] = '\0';

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%ld", sIn_mst.ac_id );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%d", sIn_mst.ac_seqn );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%s", sIn_mst.opn_br_no );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%s", sIn_mst.prdt_no );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%.2f", sIn_mst.bal );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%.2f", sIn_mst.ys_bal );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%.2f", sIn_mst.hst_bal );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%s", sIn_mst.sts );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%ld", sIn_mst.opn_date );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%ld", sIn_mst.ic_date );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%ld", sIn_mst.end_date );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%ld", sIn_mst.cls_date );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%ld", sIn_mst.lst_date );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%.5f", sIn_mst.rate );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%.5f", sIn_mst.over_rate );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%.2f", sIn_mst.intst_acm );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%d", sIn_mst.hst_cnt );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%d", sIn_mst.hst_pg );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%.2f", sIn_mst.hold_amt );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%s", sIn_mst.od_ind );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%s", sIn_mst.name );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	/*sprintf( acErrMsg,"------�ڲ���---------ƴ���ַ���Ϊ[%s]", str);
	 WRITEMSG
	 */

 	return (0);
}

/**********************************************************************
 * �� �� ����   mdm_ac_rel_con
 * �������ܣ�   ���ʻ����ʶ��ձ�����ת��һ���ַ�����
 * ����/ʱ�䣺	jack	2003��04��04��
 *
 * ������
 *     ����: 	һ���ʻ����ʶ��ձ��¼
 *     ���: 	
 *     ����: 
 *
 * �޸���ʷ��
 *
********************************************************************/
int mdm_ac_rel_con ( struct mdm_ac_rel_c tMdm_ac_rel, char *str )
{
	int	ret;
	char tmpstr[100];

	struct mdm_ac_rel_c sMdm_ac_rel;

	MEMSET_DEBUG( &sMdm_ac_rel, 0x00, sizeof(sMdm_ac_rel) );

	MEMCPY_DEBUG( &sMdm_ac_rel, &tMdm_ac_rel, sizeof(struct mdm_ac_rel_c) );

	str[0] = '\0';

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%s", sMdm_ac_rel.opn_br_no );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%s", sMdm_ac_rel.ac_no );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%ld", sMdm_ac_rel.ac_id );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%d", sMdm_ac_rel.ac_seqn );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%s", sMdm_ac_rel.mdm_code );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%s", sMdm_ac_rel.note_no );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%s", sMdm_ac_rel.note_sts );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%s", sMdm_ac_rel.coll_sts );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%ld", sMdm_ac_rel.beg_date );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%ld", sMdm_ac_rel.end_date );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%s", sMdm_ac_rel.draw_uncon_yn );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%s", sMdm_ac_rel.draw_pwd_chg );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%s", sMdm_ac_rel.draw_pwd_yn );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%s", sMdm_ac_rel.qry_pwd );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%s", sMdm_ac_rel.draw_pwd );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%s", sMdm_ac_rel.draw_id_yn );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%s", sMdm_ac_rel.id_type );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%s", sMdm_ac_rel.id_no );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%s", sMdm_ac_rel.draw_seal_yn );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%s", sMdm_ac_rel.pwd_mach_yn );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%.2f", sMdm_ac_rel.book_bal );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%.2f", sMdm_ac_rel.book_bal );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%ld", sMdm_ac_rel.prt_line );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%s", sMdm_ac_rel.main_ind );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	/*sprintf( acErrMsg,"------�����˺Ŷ��ձ�------ƴ���ַ���Ϊ[%s]", str);
	WRITEMSG
	*/


 	return (0);
}

/**********************************************************************
 * �� �� ����   od_mst_con
 * �������ܣ�   ��͸֧���ļ�����ת��һ���ַ�����
 * ����/ʱ�䣺	jack	2003��04��05��
 *
 * ������
 *     ����: 	һ��͸֧���ļ���¼
 *     ���: 	
 *     ����: 
 *
 * �޸���ʷ��
 *
********************************************************************/
int od_mst_con ( struct od_mst_c tOd_mst, char *str )
{
	int	ret;
	char tmpstr[100];

	struct od_mst_c sOd_mst;

	MEMSET_DEBUG( &sOd_mst, 0x00, sizeof(sOd_mst) );

	MEMCPY_DEBUG( &sOd_mst, &tOd_mst, sizeof(struct od_mst_c) );

	str[0] = '\0';

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%ld", sOd_mst.ac_id );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%d", sOd_mst.ac_seqn );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%s", sOd_mst.opn_br_no );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%s", sOd_mst.prdt_no );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%.2f", sOd_mst.bal );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%.2f", sOd_mst.drt_bal );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%.2f", sOd_mst.cur_bal );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%.2f", sOd_mst.intst_amt );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%.2f", sOd_mst.intst_bal );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%.2f", sOd_mst.drt_intst_bal );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%.2f", sOd_mst.cash_bal );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%.2f", sOd_mst.ys_bal );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%.2f", sOd_mst.hst_bal );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%.2f", sOd_mst.intst_acm );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%.2f", sOd_mst.drt_intst_acm );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%.2f", sOd_mst.max_bal );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%.2f", sOd_mst.cash_max_bal );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%.2f", sOd_mst.min_pay_amt );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%.5f", sOd_mst.min_pay_ratio );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%ld", sOd_mst.lst_date );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%ld", sOd_mst.hst_cnt );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%ld", sOd_mst.hst_pg );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%s", sOd_mst.ac_sts );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%ld", sOd_mst.pay_ac_id );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%d", sOd_mst.pay_ac_seqn );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	/*sprintf( acErrMsg,"------͸֧----------------ƴ���ַ���Ϊ[%s]", str);
	 WRITEMSG
	 */

	return 0;
}


/**********************************************************************
 * �� �� ����   ln_lo_con
 * �������ܣ�   ������Ƿ�������ת��һ���ַ�����
 * ����/ʱ�䣺	jack	2003��04��04��
 *
 * ������
 *     ����: 	һ������Ƿ����¼
 *     ���: 	
 *     ����: 
 *
 * �޸���ʷ��
 *
********************************************************************/
int ln_lo_con ( struct ln_lo_c tLn_lo, char *str )
{
	int	ret;
	char tmpstr[100];

	struct ln_lo_c sLn_lo;

	MEMSET_DEBUG( &sLn_lo, 0x00, sizeof(sLn_lo) );

	MEMCPY_DEBUG( &sLn_lo, &tLn_lo, sizeof(struct ln_lo_c) );

	str[0] = '\0';

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%ld", sLn_lo.ac_id );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%d", sLn_lo.ac_seqn );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%d", sLn_lo.lo_term );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%.2f", sLn_lo.lo_amt );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%.2f", sLn_lo.lo_intst );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%.2f", sLn_lo.pun_intst );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%s", sLn_lo.pay_type );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%ld", sLn_lo.over_date );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%ld", sLn_lo.shld_pay_date );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%.2f", sLn_lo.pay_lo_amt );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%.2f", sLn_lo.pay_lo_intst );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );

	MEMSET_DEBUG( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr, "%.2f", sLn_lo.pay_pun_intst );
	pub_base_strpack( tmpstr );
	strcat( str, tmpstr );


	/*sprintf( acErrMsg,"------����Ƿ���-------ƴ���ַ���Ϊ[%s]", str);
	 WRITEMSG
	 */


 	return (0);
}


/**********************************************************************
 * �� �� ����   put_mystery
 * �������ܣ�   ����������Ѻ
 * ����/ʱ�䣺	jack	2003��04��04��
 *
 * ������
 *     ����: 	
 *     ���: 	
 *     ����: 
 *
 * �޸���ʷ��
 *
********************************************************************/
int put_mystery( char *str, char *mystery )
{

	int packlen = 0;

	packlen = strlen( str );

	strcpy( mystery, pub_com_des_pack("00000000", str, packlen, MYKEY) );

/** sprintf( acErrMsg,"��Ѻ[%s]", str);
	 WRITEMSG
	sprintf( acErrMsg,"-----------------������ѺΪ[%s]", mystery);
	 WRITEMSG
**/
	return 0;
}

