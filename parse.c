#include <stdlib.h>
#include <stdio.h>
#include <struct_arbre.h>




int OWS(char *str, Branch* branch)
{
	int i=0;
	while(str[i]==' ' || str[i]==9)
	{
		i++;
	}
	*branch = new_branch(str, i, "OWS");
	return i;
}

int qdtext(char *str, Branch* branch)
{
	if(str[0]==0x9 || str[0]==0x20 || str[0] == '!' ||( str[0]>=0x23 || str[0]<=0x5B)|| (str[0]>=0x5D && str[0]<=0x7E))
	{
		return 1;
	}
	return 0;
}

int RWS(char *str, Branch* branch)
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
		*branch = new_branch(str, 1, "obs_text")
		return 1;
	}
	return 0;
}


int ctext(char *car, Branch** branch)
{
	BranchList* sub_branches = new_branchlist();
	int a=obs_text(car, &(sub_branches->branch));
	if(*car==' ' || *car=='	' || (*car>=0x21 && *car<=0x27) || (*car >= 0x2A && *car<=0x5B)||(*car>=0x5D && *car<=0x7E)|| a){
		*branch = new_branch(car, 1, "ctext")

		if(a) *branch->branches = sub_branches;
		else freeall(sub_branches); //freeall fonction qui libère tout un arbre sous une branchlist

		return 1;
	}
	return 0;
}

int vchar(char *car, Branch* branch)
{
	if(*car>=32 && *car <=127)
	{
		branch = new_branch(car, 1, "vchar");
		return 1;
	}
	return 0;
}

int ALPHA(char *car, Branch* branch)
{
	if((*car>='a' && *car<='z') || (*car>='A' && *car <='Z'))
	{
		branch = new_branch(car, 1, "ALPHA");
  	return 1;
	}
  	return 0;
}

int DIGIT(char *car, Branch* branch)
{
  if(*car>='0' && *car<='9')
  {
		branch = new_branch(car, 1, "DIGIT");
  	return 1;
  }
  return 0;
}

int scheme(char *str, Branch* branch)
{
	int i=0;
	int a;
	int b;
	if(ALPHA(str))
	{
		i+=1;
		a=ALPHA(str+i);
		b=DIGIT(str+i);
		while(a || b || str[i]=='+' || str[i]=='-' || str[i]=='.')
		{
			i+=1;
			a=ALPHA(str+i);
			b=DIGIT(str+i);
		}
		branch = new_branch(str, i, "scheme");
		return i;
	}
	else
	{
		return 0;
	}
}


int quoted_pair(char *str, Branch* branch)
{
	if(str[0]=='\\')
	{
		if(str[1]==0x09 || str[1]==' ' || vchar(str+1) || obs_text(str+1))
		{
			branch = new_branch(str, 2, "quoted_pair");
			return 2;
		}

	}
	return 0;
}


int comment(char *str, Branch* branch)
{
	int i=0;
	if(str[i]=='(')
	{
		i+=1;
		while(ctext(str+i) || quoted_pair(str+i) || comment(str+i))
		{
			if(ctext(str+i))
				i+=1;
			else if(quoted_pair(str+i))
				i+=2;
			else if(comment(str+i))
				i+=comment(str+i);


		}
		if(str[i]==')')
			return i+1;
	}
	return 0;
}


int tchar(char *car, Branch* branch)
{
	if(*car=='!' || *car=='#'||*car=='$'||*car=='%'||*car=='&'||*car=='\'' || *car== '*' ||*car== '+' || *car== '-' || *car=='.' || *car=='^' || *car=='_' || *car=='`' || *car== '|' || *car=='~' || DIGIT(car) || ALPHA(car) )
	{
		branch = new_branch(car, 1, "tchar");
		return 1;
	}
	return 0;
}

int token(char* str, Branch* branch) // 1* tchar
{
	int i=0;
	if(tchar(str)!=1)
		return 0;
	else
		while(tchar(str+i))
		{
			i+=1;
		}
		return i;
}

int method(char *str, Branch* branch)
{
	int i=token(str);
	return i;
}









int product_version(char* str, Branch* branch) // token
{
	int i= token(str);
	return i;
}

int type(char* str, Branch* branch)
{
	int i =token(str);
	return i;
}
int subtype(char* str, Branch* branch)
{
	int i =token(str);
	return i;
}

int product(char* str, Branch* branch) // token [ "/" product-version ]
{
	int i=token(str);

	if(str[i]=='/')
	{
		i+=1;
		if(product_version(str))
		{
			i+=product_version(str);
			return i;

		}
		else
		{
			return 0;
		}

	}
	else
	{
		return i;
	}


}



int User_Agent(char* str, Branch* branch)
{
	int i=product(str);
	int a,b,c;
	if(i==0)
		return 0;
	else
	{
		a=RWS(str+i);
		b=product(str+i+a+1);
		c=comment(str+i+a+1);
		while(a && (b || c))
		{
			i+=a+b+c;
			a=RWS(str+i);
			b=product(str+i+a);
			c=comment(str+i+a);
		}
		return i;
	}

}

int HEXDIG(char *car, Branch* branch)
{
	if((*car>='0' && *car<='9') || (*car>='A' && *car<='F'))
	{
		return 1;
	}
	return 0;
}

int quoted_string(char* str, Branch* branch)
{
	int i=1,a,b;
	if(str[0]== 0x22)
	{
		a=qdtext(str+i);
		b=quoted_pair(str+i);
		while(a || b)
		{
			if(a)
			{
				i+=a;
			}
			else
			{
				i+=b;
			}
		}
		if(str[i]==0x22)
			return i+1;
	}
	return 0;
}

int parameter(char *str, Branch* branch)
{
	int i=0;
	int a=token(str);
	int b;
	int c;
	if(a)
	{
		i+=a;
		if(str[i]=='=')
		{
			i+=1;
			b=token(str+i);
			c=quoted_string(str+i);
			if(b)
			{
				return i+b;

			}
			else if(c)
			{
				return i+c;
			}
		}
	}
	return 0;
}


int media_range(char* str, Branch* branch)
{
	int i=0;
	int a = type(str);
	int b= subtype(str+2);
	int c;
	if((str[0]=='*' && str[1]=='/' && str[2]=='*') || (a && str[1]=='/' && b) || (a && str[1]== '/' && str[2]=='*'))
	{
		i+=3;
		a=OWS(str+i);
		b=OWS(str+i+a+1);
		c=parameter(str+i+a+b+i);
		while(a & b &c)
		{
			i+=a+b+c+1;
		}
		return i;

	}
	return 0;
}

int sub_delims(char *car, Branch* branch)
{
	if(*car=='!'||*car=='$'||*car=='&'||*car=='\''||*car=='('||*car==')'||*car=='*'||*car=='+'||*car== ',' ||*car== ';' || *car=='=')
	{
		return 1;
	}
	return 0;
}

int pct_encoded(char *str, Branch* branch)
{
	if (str[0]=='%' && HEXDIG(str+1) && HEXDIG(str+2))
	{
		return 3;
	}
	return 0;
}

int unreserved(char *car, Branch* branch)
{
	if (ALPHA(car) || DIGIT(car) || *car=='-' || *car=='.' || *car=='_' || *car=='~')
	{
		return 1;
	}
	return 0;
}



int pchar(char* str, Branch* branch)
{
	if(unreserved(str)  || sub_delims(str) || str[0]==':' || str[0]=='@'){
		return(1);
	}
	else if(str[0]=='%')
	{

		return pct_encoded(str);
	}
	return 0;
}

int segment(char *str, Branch* branch)
{
	int i=0;
	while(pchar(str+i))
	{
		i+=pchar(str+i);
	}
	return i;
}

int absolute_path(char* str, Branch* branch) //1* ("/" segment)
{
	int i=0;
	while(str[i]=='/')
	{
		i+=1;
		i+=segment(str+i);
	}
	return i;

}

int query(char* str, Branch* branch) //  * ( pchar / "/" / "?" )
{
	int i=0;
	while(pchar(str+i)||str[i]=='/'||str[i]=='?')
	{
		if(str[i]=='%')
			i+=3;
		else
			i+=1;
	}
	return i;
}


int userinfo(char* str,Branch* branch)
{
	int i=0;
	int a=unreserved(str);
	int b=pct_encoded(str);
	int c=sub_delims(str);
	while(str[i]==':' || a || b || c)
	{
		i+=1;
		a=unreserved(str);
		b=pct_encoded(str);
		c=sub_delims(str);
	}
	return i;
}

int host(char* str, Branch* branch)
{

}

int authority(char* str, Branch* branch)
{
	int i=userinfo(str);
	if(str[i]=='@')
	{
		i+=1;

	}
}

int hier_part(char* str, Branch* branch)
{
	int i=0;
	if(str[0]=='/')
	{
		if(str[1]=='/')
		{

		}
	}
}

int absolute_URI(char* str,Branch* branch)
{
	int i=scheme(str);
	if(i && str[i]==':')
	{

	}

}

int origin_form(char* str, Branch* branch)
{
	int i=absolute_path(str);
	if(i==0)
	{
		return 0;
	}
	else
	{
		printf("coucou\n");
		if(str[i]=='?')
		{
			i+=1+query(str+i);
			return i;
		}
		else
		{
			return i;
		}
	}
}

int request_target(char *str, Branch* branch)
{
	int i = origin_form(str);
	return i;

}

int HTTP_name(char *str, Branch* branch)
{
	if(*str=='H')
	{
		if(str[1]=='T')
		{
			if(str[2]=='T')
			{
				if(str[3]=='P')
				{
					return 4;
				}
			}
		}
	}
	return 0;
}
int HTTP_version(char* str, Branch* branch)
{
	int i=HTTP_name(str);
	int a;

	if(i && str[i]=='/')
	{
		i+=1;
		a=DIGIT(str+i);
		if(a && str[i+a]=='.')
		{
			i+=a+1;
			a=DIGIT(str+i);
			if(a)
			{
				return a+i;
			}
		}
	}
	return 0;
}
int request_line(char *str, Branch* branch)
{
    int i=0;
    int a=method(str);
    int b;
    if(a)
    {
    	i+=a;
    	if(str[i]==' ')
    	{
    		i+=1;
    		b=request_target(str+i);
    		if(b)
    		{
    			i+=b;
    			if(str[i]==' ')
    			{
    				i+=1;
						a=HTTP_version(str+i);
						if(str[i+a]==13)
						{
							if(str[i+a+1]==10)
							{
								return i+a+2;
							}
						}

    			}
    		}
    	}
    }
		return 0;
}

int status_code(char* str, Branch* branch)
{
	if(DIGIT(str))
	{
		if(DIGIT(str+1))
		{
			if(DIGIT(str+2))
			{
				return 3;
			}
		}
	}
	return 0;
}

int reason_phrase(char *str, Branch branch)
{
	int i=0;
	int a=obs_text(str);
	int b=vchar(str);
	while(str[i]== ' ' || str[i]==0x9 || a || b)
	{
		a=obs_text(str);
		b=vchar(str);
		i+=1;
	}
	return i;

}

int status_line(char *str, Branch* branch)
{
	int i=HTTP_version(str);
	int a;
	if(i && str[i]==' ')
	{
		i+=1;
		a=status_code(str+i);
		if(a && str[i+a]==' ')
		{
			i+=a+1;
			a=reason_phrase(str);
			if(str[i+a]==13)
			{
				if(str[i+a+1]==10)
				{
					return i+a+2;
				}
			}
		}
	}
	return 0
}

int start_line(char *str, Branch* branch)
{
    int a=request_line(str);
		int b= status_line(str);
		if(a)
		{
			return a;
		}
		else if(b)
		{
			return b;
		}
		return 0;
}

int HTTP_message(char *str, Branch* branch)
{
	int i=start_line(str);
	if(i)
	{

	}
}


int Accept(char* str, Branch* branch)
{
	int i=0;
	if(str[0]=='\0')
	{
		return 1;
	}
	else
	{
		if(str[i]== ',' || media_range(str+i))
		{

		}
	}
}

int main()
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
