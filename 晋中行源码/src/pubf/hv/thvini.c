#include "hvini.h"
#include "find_debug.h"
int main(int argc, char *argv[])
{
	char Item[50];
	int j;
	j=hv_readIni("test2.ini","operating systems",
		"C:\\",Item);
	printf("%d,%s\n",j,Item);
	hv_writeIni("test2.ini","operating systems",
		"C:\\","\"Microsoft Windows1\"");
	hv_readIni("test2.ini","operating systems",
		"C:\\",Item);
	printf("%s\n",Item);
	return 0;
}
