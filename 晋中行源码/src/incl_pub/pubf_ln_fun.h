/** ln目录下 函数声明部分 **/
/** pubf_ln_check.c **/
int pub_ln_lndis_file(char *,int ,struct mdm_ac_rel_c *,struct ln_mst_c *,
							struct ln_parm_c *);
int pub_ln_lnid_file(long ac_id ,int ac_seqn ,struct ln_mst_c *ln_mst);
int pub_ln_check(char *tx_type);
int pub_ln_check_sts( struct ln_mst_c ln_mst,int mode );

/** pubf_ln_intst.c **/
int pub_ln_cal_intst ( int ac_id ,int ac_seqn , double *intst ,	
		double *in_lo_intst , double *out_lo_intst , double *cmpd_lo_intst );
int pub_ln_cal_AmtIntst ( long ac_id ,int ac_seqn ,  double *intst );
int pub_ln_cal_ComIntst ( long ac_id , int ac_seqn , double *in_ComIntst , 
			double *out_ComIntst , double *com_intst );
int pub_ln_Udp_LnReg ( long ac_id , int ac_seqn , char *type , double pamt );
int pub_ln_ln_reg( struct ln_mst_c ln_mst,long lst_date , long tx_date,
		char *type, double amt, double acm , char *ind,int mode );
int pub_ln_Ins_ln_reg( struct ln_mst_c p_ln_mst , struct ln_parm_c p_ln_parm ,
	char *type , double pamt , double acm , char *ind);

/** pubf_ln_modify.c **/
int pub_ln_change( int ac_id , int ac_seqn , char *prdt_no );

/** pubf_ln_PayPlan.c **/
int pub_ln_cal_PayPlan ( double ttl_amt , int ttl_cnt , char *pay_typ , 
						double rate , int cnt , double *amt , double *intst );

/** pubf_ln_AcCheck.c **/
int pub_ln_AcCheck( char *ac_no,int ac_seqn,long *cif_no,long *ac_id );
						 
