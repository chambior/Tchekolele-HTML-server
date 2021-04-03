#include <stdio.h>
#include <stdlib.h>

void main()
{
    
	char* test="Wget/1.16 (linux-gnu)";
	char* test2 ="\"\"";
	char* test3 ="é(";
	char* test4="$";
	char* test5="%F5";
	char* test6="%5";
	char* test7="f/test/t";
	char* test8="/slt/test/fe?test%%*ù$";
	char* test9="////?fresd";
	char* test10="";

	printf("%s : %d\n",test,quoted_string(test));
	printf("%s : %d\n",test2,quoted_string(test2));
	printf("%s : %d\n",test3,quoted_string(test3));
	printf("%s : %d\n",test4,quoted_string(test4));
	printf("%s : %d\n",test5,quoted_string(test5));
	printf("%s : %d\n",test6,quoted_string(test6));
	printf("%s : %d\n",test7,quoted_string(test7));
	printf("%s : %d\n",test8,quoted_string(test8));
	printf("%s : %d\n",test9,quoted_string(test9));
	printf("%s : %d\n",test10,quoted_string(test10));
}
