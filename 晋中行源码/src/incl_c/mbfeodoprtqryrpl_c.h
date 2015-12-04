#ifndef __mbfeodoprtqryrpl_CH__
#define __mbfeodoprtqryrpl_CH__
struct mbfeodoprtqryrpl_c{
	char      rowid[20];
	long      consigndate;
	char      querybkcode[13];
	long      origconsigndate;
	char      origodficode[13];
	char      origtxtype[2];
	char      origtxssno[9];
	char      sndcmtcentrecvtime[15];
	char      sndcmtcentmvtime[15];
	char      sndcmtcenttranstat[3];
	char      npcrecvtime[15];
	char      statetime[15];
	char      npcmvtime[15];
	char      npctranstat[3];
	char      recvcmtcentrecvtime[15];
	char      recvcmtcentmvtime[15];
	char      recvcmtcenttranstat[3];
	char      errcode[9];
	char      errdesc[61];
};
#endif 
