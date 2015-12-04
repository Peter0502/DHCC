#ifndef __prot_hst_CH__
#define __prot_hst_CH__
struct prot_hst_c
{
	long		tx_date;
	long		ac_id;
	int		ac_seqn;
	char		prot_type[4];
	double		amt;
	double		amt2;
	double		amt3;
	char		reply[5];
};
#endif
