#include "struct_arbre.h"
#include <stdlib.h>
#include <stdio.h>

int OWS(char *str, Branch** branch)
{
	int i=0;
	while(str[i]==' ' || str[i]==9)
	{
		i++;
	}
	*branch = new_branch(str, i, "OWS");
	return i;
}

int RWS(char *str, Branch** branch)
{
	int i=0;
	while(str[i]==' ' || str[i]==9)
	{
		i++;
	}
	*branch = new_branch(str, i, "RWS");
	return i;
}

int obs_text(char *str, Branch** branch)
{
	if(*str>=0x80 && *str<=0xFF)
	{
		*branch = new_branch(str, 1, "obs_text");
		return 1;
	}
	return 0;
}


int ctext(char *car, Branch** branch)
{
	BranchList* sub_branches = new_branchlist();//
	int a=obs_text(car, &(sub_branches->branch));//
	if(*car==' ' || *car=='	' || (*car>=0x21 && *car<=0x27) || (*car >= 0x2A && *car<=0x5B)||(*car>=0x5D && *car<=0x7E)|| a){
		*branch = new_branch(car, 1, "ctext");//

		if(a) (*branch)->branches = sub_branches;//
		else freeall(sub_branches); //freeall fonction qui libère tout un arbre sous une branchlist

		return 1;
	}
	return 0;
}

int vchar(char *car, Branch** branch)
{
	if(*car>=32 && *car <=127)
	{
		*branch = new_branch(car, 1, "vchar");
		return 1;
	}
	return 0;
}

int ALPHA(char *car, Branch** branch)
{
	if((*car>='a' && *car<='z') || (*car>='A' && *car <='Z'))
	{
		*branch = new_branch(car, 1, "ALPHA");
  	return 1;
	}
  	return 0;
}

int DIGIT(char *car, Branch** branch)
{
	if(*car>='0' && *car<='9')
	{
		*branch = new_branch(car, 1, "DIGIT");
		return 1;
	}
	return 0;
}

int qvalue(char *str, Branch** branch)
{
	int i=0;
	int a;
	int res = 0;
	*branch = new_branch(str,0,"qvalue");
	if(str[i]=='0'){
		i+=1;
		if(str[i]=='.'){
			i++;
			BranchList* sub_branches = new_branchlist();
			BranchList* sb;
			a=DIGIT(str+i, &(sub_branches->branch));
			while(a){
				sb = sub_branches;
				while(sb->next) sb = sb->next;
				sb->next = new_branchlist();
				i++;
				a=DIGIT(str+i, &(sb->next->branch));
			}
			sb = sub_branches;
			if(sb->next){
				while(sb->next->next) sb = sb->next;
				freeall(sb->next);
				sb->next = NULL;
				(*branch)->branches = sub_branches;
			}
			else freeall(sb);
		}
		res = i;
	}
	else if(str[i]=='1'){
		i+=1;
		if(str[i]=='.'){
			i+=1;
			while(str[i]=='0' && i<5) i+=1;
		}
		res = i;
	}
	(*branch)->data_size = res;
	return res;
}

int main(void){
	char* test="0.15648";
	Branch* branch	;
	qvalue(test, &branch);
	printf("analyse terminée\n");
	displaytree(branch, 0);
}
