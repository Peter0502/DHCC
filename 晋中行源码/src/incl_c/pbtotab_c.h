#ifndef __pbtotab_CH__
#define __pbtotab_CH__
struct pbtotab_c{
	char      rowid[20];
	char      brno[13];
	char      brname[61];
	char      txday[9];
	char      txtime[9];
	char      aptype[2];
	char      txcd[3];
	char      sendday[9];
	char      apflg[33];
	char      paybrno[13];
	char      paybrtype[2];
	char      payname[61];
	char      payactno[41];
	char      paycode[21];
	char      cashbro[13];
	char      cashbrtype[2];
	char      cashname[61];
	char      cashactno[41];
	char      cashcode[21];
	char      cashertype[3];
	char      cashcertif[23];
	char      txamt[49];
	char      paydate[9];
	char      cashdate[9];
	char      dscpt[21];
	char      paytype[3];
	char      crdno[33];
	char      txtypeno[11];
	char      expand[21];
};
#endif 
