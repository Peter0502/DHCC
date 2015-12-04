#******************************************************************************#
#* 说明::本工程文件适应于如下配置:                                            *#
#*       操作系统  : AIX5                                                     *#
#*       数据库系统: ORACLE9i                                                 *#
#*       编译器    : gcc                                                      *#
#*----------------------------------------------------------------------------*#
#* 编者：Li JunXu    编写日期:20030112   修改者:           修改日期:          *#
#******************************************************************************#

#############################################
##              PUBLIC PART                ##
#############################################
PROCINCLUDE=include=$(ORACLE_HOME)/precomp/public include=../incl include=../src
APPINCLUDE= -I../incl -I../src -I$(ORACLE_HOME)/precomp/public 
SYS_INCLUDE='sys_include=(/usr/include)'
ORALIBDIR= -L$(ORACLE_HOME)/lib
APPLIBDIR= -L$(WORKDIR)/lib 
#-L/usr/lib -L/usr/local/lib/gcc-lib/powerpc-ibm-aix5.1.0.0/3.2.1
BINDIR= $(WORKDIR)/bin
LIBDIR= $(WORKDIR)/lib
USERID=scott/tiger
PROC=proc
CC=cc $(Q64)
CFLAGS= $(Q64)
ORALIB= -lclntsh -lpthread -lc `cat $(ORACLE_HOME)/lib/sysliblist`

#############################################
##              USER DEFINE PART           ##
#############################################
SLIB=	$(WORKDIR)/lib/libpubdb.so 
OBJS =  comm.o \
		ac_seqn_id.o \
		acc_ac_id.o \
		accounthtreg.o \
		accountreg.o \
		ag_itemlist.o \
		ag_paytlcs.o \
		ag_peracctinfo.o \
		ag_peritemmap.o \
		ag_unitacctmap.o \
		ag_unitinfo.o \
		ass_br_def.o \
		ass_cp_stat.o \
		ass_gl.o \
		ass_mger_ach_stat.o \
		ass_mst.o \
		ass_mst_hst.o \
		ass_mst_prdt_rel.o \
		ass_prdt_sale_sta.o \
		ass_rate_def.o \
		ass_subs_intst.o \
		ass_tel_pflio_stat.o \
		auth_ac_rec.o \
		auth_auth_auth.o \
		auth_auth_lmt.o \
		auth_chk_def.o \
		auth_code.o \
		auth_cond.o \
		auth_cond_def.o \
		auth_def.o \
		auth_flow.o \
		auth_inf.o \
		auth_item.o \
		auth_lmt_def.o \
		auth_oper_brno.o \
		auth_seqn.o \
		auth_tel_auth.o \
		auth_tel_tx_def.o \
		auth_tx_list.o \
                auth_remote_inf.o \
		bitmap.o \
		bitmaptmp.o \
		card_book.o \
		card_consum_lmt.o \
		cash_book.o \
		cash_mst.o \
		cash_mst_hst.o \
		changetab.o \
		chnl_self_trade.o \
		chnl_tradeadd.o \
		chnl_tradelimit.o \
		cif_addr_inf.o \
		cif_alias_inf.o \
		cif_basic_inf.o \
		cif_blklst_rel.o \
		cif_cif_rel.o \
		cif_cop_inf.o \
		cif_crdt_code.o \
		cif_crdt_inf.o \
		cif_crdt_use_det.o \
		cif_email_inf.o \
		cif_flok_code.o \
		cif_id_code_rel.o \
		cif_inf_chg_log.o \
		cif_lang_code.o \
		cif_lifsty_inf.o \
		cif_magn_inf.o \
		cif_mger_inf.o \
		cif_mger_rel.o \
		cif_nat_code.o \
		cif_no_com.o \
		cif_no_in.o \
		cif_no_per.o \
		cif_oth_prdt_inf.o \
		cif_per_inf.o \
		cif_post_code.o \
		cif_prdt_rel.o \
		cif_rel_chg_hst.o \
		cif_rel_des.o \
		cif_req_log.o \
		cif_tranq_stat.o \
		cif_wrk_hst_inf.o \
                cif_photo_inf.o \
                cif_cop_photo.o \
                idcode_chk_inf.o \
		city_flag.o \
		clt_id_seq.o \
		com_acc_br.o \
		com_branch.o \
		com_chrg_hst.o \
		com_chrg_rate.o \
		com_cur_no_code.o \
		com_def_chrg.o \
		com_eod_exec.o \
		com_exch_rate.o \
		com_holiday.o \
		com_item.o \
		com_item_add.o \
		com_parm.o \
		com_rate.o \
		com_rollback_ctl.o \
		com_sect_acm.o \
		com_sys_parm.o \
		com_tel.o \
		com_tel_change.o \
		com_tel_change_hst.o \
		com_tel_connect.o \
		com_tx_br_rel.o \
		correl_brf.o \
		dadary.o \
		data_dic.o \
		data_dictmp.o \
		dc_acc_rel.o \
		dc_branch.o \
		dc_entry.o \
		dc_log.o \
		dc_log_bk.o \
		dc_log_hao.o \
		dc_log_rz.o \
		dd_mst.o \
		dd_mst_a.o \
		dd_mst_bk.o \
		dd_mst_hao.o \
		dd_mst_hst.o \
		dd_parm.o \
		dda_zyywzk.o \
		dic_data.o \
		dic_name.o \
		doc_dic.o \
		fin_acct.o \
		four_tradelog.o \
		gl_amt.o \
		gl_amt_nj.o \
		gl_amt_sum.o \
		gl_bal.o \
		gl_bal_nj.o \
		gl_bal_sum.o \
		gl_mst.o \
		gl_mst_hst.o \
		gl_mst_nj.o \
		gl_note.o \
		gl_note_sum.o \
		gl_oprt_dyn.o \
		gl_prdt_dyn.o \
		gl_prdt_dyn_hst.o \
		gl_rpt.o \
		gl_sub.o \
		gl_sub_826.o \
		gl_sub_nj.o \
		gl_tsub.o \
		impa_dep_reg.o \
		in_ac_seqn.o \
		in_id_seq.o \
		in_mst.o \
		in_mst_bk.o \
		in_mst_hst.o \
		in_parm.o \
		intst_hst.o \
		ln_acm_reg.o \
		ln_auth.o \
		ln_eod_pri.o \
		ln_expand.o \
		ln_gage_reg.o \
		ln_lo.o \
		ln_lo_rol.o \
		ln_mst.o \
		ln_mst_bk.o \
		ln_mst_hst.o \
		ln_parm.o \
		ln_pay_pln.o \
		ln_pay_pln_rol.o \
		ln_pay_pln_tmp.o \
		ln_pay_seq.o \
		ln_paystatus.o \
		ln_pln_inf.o \
		ln_reg.o \
		ln_repair.o \
		march_info.o \
		march_mst.o \
		march_mst_hst.o \
		march_mst_mx.o \
		march_mst_mx_hst.o \
		mas_ehfe.o \
		mas_hvcd.o \
		mas_hvcs.o \
		mas_hvpt.o \
		mas_ohvc.o \
		mas_pato.o \
		mas_pbto.o \
		mas_phfe.o \
		mas_safe.o \
		mas_seqn.o \
		mbfe_cmt_fmt.o \
		mbfe_cmt_tag.o \
		mbfe_errcode.o \
		mbfe_hang.o \
		mbfe_sys_ctl.o \
		mbfebillaplset.o \
		mbfebillmsgreg.o \
		mbfebillopnreg.o \
		mbfebillrtureg.o \
		mbfebksttalt.o \
		mbfebrbkno.o \
		mbfeeisbank.o \
		mbfeeisbanksnd.o \
		mbfefreeinfo.o \
		mbfeghvrecvlist.o \
		mbfeghvsendjrn.o \
		mbfeghvsendlist.o \
		mbfehppaycnt.o \
		mbfeinpawnfina.o \
		mbfeinpawnmsg.o \
		mbfenetbank.o \
		mbfenetbanksnd.o \
		mbfeodoprtqryrpl.o \
		mbfeoprtcancel.o \
		mbfeoprtfeecl.o \
		mbfeoprtfeelt.o \
		mbfeoverdraftcl.o \
		mbfeoverdraftlt.o \
		mbfequerymsg.o \
		mbferecvcmtmsg.o \
		mbfereplymsg.o \
		mbfereturnapp.o \
		mbfereturnrply.o \
		mbferltxdclt.o \
		mbfesabk.o \
		mbfesabksnd.o \
		mbfesendcmtmsg.o \
		mbfeserialno.o \
		mdm_ac_rel.o \
		mdm_attr.o \
		mdm_prdt_rel.o \
		mdm_unprt_hst.o \
		menu.o \
		mo_ac_chg_hst.o \
		mo_again_ln.o \
		mo_bail_rate.o \
		mo_bank_acc_po.o \
		mo_bank_po.o \
		mo_bank_po_query.o \
		mo_bank_split_po.o \
		mo_cashier.o \
		mo_cheq_od.o \
		mo_chg_bal.o \
		mo_city.o \
		mo_city_query.o \
		mo_cli_log.o \
		mo_coll.o \
		mo_dc_inf.o \
		mo_def_agio.o \
		mo_def_rate.o \
		mo_discnt.o \
		mo_fax.o \
		mo_fax_code.o \
		mo_fet_cash.o \
		mo_finance_info.o \
		mo_finance_quit.o \
		mo_hold.o \
		mo_ibl.o \
		mo_ibl_ret.o \
		mo_infrm.o \
		mo_invest.o \
		mo_jl_bal.o \
		mo_loss.o \
		mo_merge_ac.o \
		mo_opn_cls.o \
		mo_pay_again_ln.o \
		mo_pay_discnt.o \
		mo_po_co.o \
		mo_po_qry.o \
		mo_prdt_prot.o \
		mo_prot_ac_rel.o \
		mo_pun_acc.o \
		mo_put_intst.o \
		mo_righ_invest.o \
		mo_rmny_smny.o \
		mo_rpl_note.o \
		mo_sg_cash_po.o \
		mo_sg_cash_po_bill.o \
		mo_spec_intst.o \
		mo_stok_rebuy_inpb.o \
		mo_stok_rebuy_prot.o \
		mo_trust_list.o \
		mo_trust_pay.o \
		mo_trust_prot.o \
		mo_unchange.o \
		mo_xjszybb.o \
		mob_acct_type.o \
		mob_acct_type_dele.o \
		mob_acct_type_hst.o \
		mob_custinfo.o \
		mob_group.o \
		mob_recvmail.o \
		mob_sendmail.o \
		mob_server_type.o \
		mort_bill.o \
		note_cheq_hst.o \
		note_cheq_mst.o \
		note_cheq_mst_tmp.o \
		note_min_no.o \
		note_mst.o \
		note_mst_hst.o \
		note_mst_tmp.o \
		note_parm.o \
		note_tel_auth.o \
		note_type_tmp.o \
		od_bill.o \
		od_dd_rel.o \
		od_drt_def.o \
		od_mst.o \
		od_mst_hst.o \
		od_parm.o \
		od_pay_pln.o \
		pay_agent.o \
		pay_agent_bk.o \
		paymach_mana_inf.o \
		point_mst_hst.o \
		point_point_mst.o \
		point_prdt_def.o \
		point_tx_def.o \
		pos_fee_deal.o \
		pos_fee_mst.o \
		pos_fee_mst_hst.o \
		pos_mobile_pay.o \
		prdt_point_hst.o \
		prdt_point_mst.o \
		prdt_prot_parm.o \
		protect_note.o \
		remark_ctl.o \
		rpt_note_ctl.o \
		rpt_note_inf.o \
		rsp_code.o \
		seqctl.o \
		six_dz_log.o \
		subbr_deposit.o \
		subbr_deposit_hst.o \
		subbr_ln.o \
		subbr_ln_hst.o \
		sum_tax.o \
		sysmenuitems.o \
		sysmenus.o \
		td_jycx_tmp.o \
		td_mst.o \
		td_mst_bk.o \
		td_mst_hst.o \
		td_parm.o \
		td_list.o \
		tel_control.o \
		tel_save_inf.o \
		tmp_7861080.o \
		tmp_acct.o \
		tmp_ck.o \
		tmp_gy.o \
		tmp_hold.o \
		tmp_table.o \
		tmp_tx_def.o \
		tmp_tx_flow_def.o \
		tmp_tx_sub_def.o \
		trace_log.o \
		trace_parm.o \
		trace_log_bk.o \
		trace_log_rz.o \
		tx_dc_br_rel.o \
		tx_dc_rel.o \
		tx_def.o \
		tx_def_tmp.o \
		tx_flow_def.o \
		tx_flow_def_tmp.o \
		tx_log.o \
		tx_log_bk.o \
		tx_sub_def.o \
		tx_sub_def_tmp.o \
		tx_sub_rel.o \
		tx_sub_rel_tmp.o \
		up_dd_mst.o \
		up_dd_mst_hst.o \
		prdt_ac_id.o \
		wjdeview_td.o \
		wjdeview_dd.o \
		wjdeview.o \
		bal_ciftype.o \
		prot_hst.o \
		cash_lmt_def.o \
		mq_acct_no.o \
		mq_batch_head.o \
		mq_batch_mx.o \
		mq_ctl.o \
		mq_retmsg.o \
		mq_rush.o \
		mq_tax_head.o \
		mq_tax_info.o \
		mq_tax_payment.o \
		mq_tax_payment_mx.o \
		mq_taxcode.o \
		ln_verify_reg.o \
		ac_bal_view.o \
		auth_tx_code.o \
		chnl_marchinfo.o \
		mo_dep_reg.o \
        ac_ac_rel.o \
		com_intst_tax_rate.o \
		mo_int_tax_hst.o \
		hv_brno_orno.o   \
		hv_chargemx.o    \
		hv_chkagehtr.o   \
		hv_chkagemr.o     \
		hv_chkerror.o     \
		hv_chkmr.o        \
		hv_chkmrdtl.o     \
		hv_chkmrhis.o     \
		hv_errormr.o      \
		hv_fzf.o          \
		hv_fzfhtr.o       \
		hv_orno_brno.o    \
		hv_poinfo.o       \
		hv_sendlist.o     \
		hv_sendlisthtr.o  \
		hv_syspara.o      \
		hv_uniontab.o     \
		hv_zf.o           \
		hv_zfhtr.o        \
		hvstat.o          \
		lv_addit.o        \
		lv_czyh_dz.o      \
		lv_c254r.o        \
		lv_c552r.o        \
		lv_c554r.o        \
		lv_chk842_d.o     \
		lv_chk842_m.o     \
		lv_chk850_d.o     \
		lv_chk850_m.o     \
		lv_chk851_d.o     \
		lv_chk851_m.o     \
		lv_chk852_d.o     \
		lv_chk852_m.o     \
		lv_lbctl.o        \
		lv_newxyh.o       \
		lv_parad.o        \
		lv_param.o        \
		lv_pkghtr.o       \
		lv_pkgreg.o       \
		lv_recvlist.o     \
		lv_sendlist.o     \
		lv_sysctl.o       \
		lv_wbctl.o        \
		lv_xmh.o          \
		seqctl.o	  \
		cassreg.o         \
		ctl_accountreg.o  \
		zjgl_mst.o  \
		zjgl_mst_hst.o \
		in_stok.o \
		front_back_rel.o \
        	kajiaojie.o \
		whwtsk_reg.o \
		ac_yht_rel.o \
		mis_iosreportdatatmp.o \
		print_full_page.o \
		hvcdtab.o \
		hvcstab.o \
		hvpttab.o \
		ohvctab.o \
		patotab.o \
		pbtotab.o \
		card_reg.o \
		keys.o \
		acc_hrt_log.o \
		czcust.o \
		card_check_sheet.o \
		tware.o \
		acno_hst_log.o \
		lv_dsfsendlist.o \
		lv_pldfr.o \
		lv_pldscx.o \
		lv_pldsfqugx.o \
		lv_pldsr.o \
		lv_oths.o \
		hv_81x.o \
		hv_addit.o \
		lv_c503r.o \
		lv_chgbank.o \
		sh_com_tel.o \
		sh_com_branch.o \
		sh_trace_log_bak.o \
		tranpara.o \
		shjdcode.o \
		sherrmsg.o \
		shjd_tlrpoot.o \
		shjd_tlrvsbrn.o \
		flag_tranpara.o \
		intrtmpreg.o \
		sum_tax_new.o \
        tctd_ktzh.o \
        cif_no_yzh.o \
        bplosthis.o\
        cashier.o\
        com_parm_holiday.o\
	chnl_premise.o \
	atmrecord.o\
	cupscardhtr.o\
	cupserr.o\
	cupsinsctl.o\
	cupsppfw.o\
	syyh_czyh_qs_reg.o\
	auth_modify_ln_rate.o ecds_recv_reg.o\
	lv_category.o\
	cms_rollback.o\
	mo_bank_po_fee.o\
	draw_loan_hst.o\
	draw_loan_limit.o\
	pact_gaga_rel.o\
	seltelreg.o\
	mob_sendmail.o\
	kh_reg.o\
	sub_dd_mst.o\
	sub_dd_mst_hst.o\
	xqdz.o\
	sub_intst_hst.o\
	sub_com_sect_acm.o\
	qury_reg.o\
	chnl_single_hold.o\
        cif_inf.o\
	cif_sfhc_inf.o\
  bzj_js_ac_rel.o\
   ass_score_lvl_rel.o\
   cif_ass_answer_score.o\
   fn_ac_rel.o\
   fn_br_profit.o\
   fn_br_quota_parm.o\
   fn_flt_radio_parm.o\
   fn_parm.o\
   fn_redempt_hst.o\
   fn_reg.o\
   fn_reg_hst.o\
   redempt_rate.o \
   netbank_yqdz.o \
   mob_fee_mx.o \
   netbank_cif_ukey_rel.o \
   mob_sendmail_pl.o \
   ag_qs_gd_rel.o\
   chnl_compare_acct.o \
   chnl_batch_acct.o \
   chnl_tx_detl.o \
   chnl_single_cnaps.o \
   mobile_time.o\
   cif_ass_reg.o\
   ag_ac_rel.o\
   cif_merge_log.o\
   vtm_note_mst.o\
   tel_save_rel.o \
   mob_sys_parm.o \
   chnl_single_acct.o\
   fn_parm.o\
   cif_copa_inf.o\
   cif_shareholder_inf.o\
   cif_manage_inf.o \
   join_cop_inf.o\
   cif_family_inf.o\
   hv_zf_erdai.o\
   lv_pkgreg_erdai.o \
   agent_info.o \
   mob_fee_mx.o \
   counter_tlog.o\
	mob_chg_rel.o\
   weixin_acct_reg.o \
   weixin_acct_hst.o \
   mo_sxc.o \
   mo_sxc_hst.o \
   mo_sxc_hst_intst.o \
   mo_qzgl_hst.o \
   mo_qzgl.o \
   new_old_accno.o\
   new_old_prdt.o \
   prdt_jz_dz.o\
   new_jz_prdt.o\
   mo_qzgl.o\
   mo_sxc_hst_intst.o
#############################################
##              PUBLIC PART                ##
#############################################
$(SLIB): $(OBJS)
	$(CC) -G -o   $@ $(OBJS) -L$(HOME)/lib -lbase -lenvinfo
	ar $(X64) -rv libpubdb.a $(OBJ)  
	@echo "$@ is OK !"

.SUFFIXES: .pc .c .o
.pc.o:
	$(PROC) $(PROCINCLUDE) dbms=v7 unsafe_null=yes lines=yes iname=$*.pc
	$(CC) -mminimal-toc -w $(CFLAGS) $(APPINCLUDE) -c $*.c
	@rm -f $*.c
	@rm -f $*.lis
.c.o:
	$(CC) -w $(CFLAGS)  $(APPINCLUDE) -c $*.c

fall:
	@touch *c
	@make
