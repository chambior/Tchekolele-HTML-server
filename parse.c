#include "parse.h"

int strcomp(char* str1,char* str2)
{
	int i=0;
	while(str1[i]==str2[i]){
		i+=1;
	}
	if(str2[i]=='\0')
	{
		return 1;
	}
	else
	{
		return 0;
	}
}



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

int qdtext(char *str, Branch** branch)
{
	if(str[0]==0x9 || str[0]==0x20 || str[0] == '!' ||( str[0]>=0x23 || str[0]<=0x5B)|| (str[0]>=0x5D && str[0]<=0x7E))
	{
		*branch = new_branch(str, 1, "RWS");
		return 1;
	}
	*branch = new_branch(str, 0, "RWS");
	return 0;
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
	*branch = new_branch(str, 0, "obs_text");
	return 0;
}


int ctext(char *car, Branch** branch)
{
	int res;
	*branch = new_branch(car, 0, "ctext");
	BranchList* sub_branches = new_branchlist();

	int a=obs_text(car, addBranch(&sub_branches));
	if(!a) freeLast(&sub_branches);

	if(*car==' ' || *car=='	' || (*car>=0x21 && *car<=0x27) || (*car >= 0x2A && *car<=0x5B)||(*car>=0x5D && *car<=0x7E)|| a){

		res = 1;
	}
	else res = 0;

	(*branch)->branches = sub_branches;
	(*branch)->data_size = res;
	return res;
}

int vchar(char *car, Branch** branch)
{
	if(*car>=32 && *car <=127)
	{
		*branch = new_branch(car, 1, "vchar");
		return 1;
	}
	*branch = new_branch(car, 0, "vchar");
	return 0;
}

int ALPHA(char *car, Branch** branch)
{
	if((*car>='a' && *car<='z') || (*car>='A' && *car <='Z'))
	{
		*branch = new_branch(car, 1, "ALPHA");
  		return 1;
	}
	*branch = new_branch(car, 0, "ALPHA");
  	return 0;
}

int DIGIT(char *car, Branch** branch)
{
  if(*car>='0' && *car<='9')
  {
	*branch = new_branch(car, 1, "DIGIT");
	#ifdef DEBUG
	printf("DIGIT returns %d\n", 1);
	#endif /* DEBUG */
  	return 1;
  }
	*branch = new_branch(car, 0, "DIGIT");
	#ifdef DEBUG
	printf("DIGIT returns %d\n", 0);
	#endif /* DEBUG */
  return 0;
}

int qvalue(char *str, Branch** branch)//revoir
{
	int i=0;
	int a;
	int res = 0;
	*branch = new_branch(str,0,"qvalue");
	if(str[i]=='0'){
		i+=1;
		if(str[i]=='.'){
			i++;
			BranchList* sub_branches = NULL;

			a=DIGIT(str+i, addBranch(&sub_branches));
			if(!a)freeLast(&sub_branches);
			while(a){
				i++;
				a=DIGIT(str+i, addBranch(&sub_branches));
				if(!a)freeLast(&sub_branches);
			}

			(*branch)->branches = sub_branches;
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

int weight(char *str,Branch** branch)
{
	*branch = new_branch(str,0,"weight");
	BranchList* sub_branches = NULL;

	int res = 0;
	int i=OWS(str, addBranch(&sub_branches));
	//if(!i) freeLast(&sub_branches);

	if(str[i]==';')
	{
		i+=1;
		int pa = OWS(str+i, addBranch(&sub_branches));
		//if(!pa) freeLast(&sub_branches);

		i+=pa;
		if(str[i]=='q'){
			i+=1;
			if(str[i]=='='){
				i+=1;

				int a=qvalue(str+i, addBranch(&sub_branches));
				if(!a) freeLast(&sub_branches);

				if(a){
					i+=a;
					res = i;
				}
			}
		}
	}
	(*branch)->branches = sub_branches;
	(*branch)->data_size = res;
	#ifdef DEBUG
	printf("weight returns %d\n", res);
	#endif /* DEBUG */
	return res;
}




int scheme(char *str, Branch** branch)
{
	*branch = new_branch(str, 0, "scheme");
	BranchList* sub_branches = new_branchlist();

	int i=0;
	int a;
	int b;
	int res;
	int var = ALPHA(str, addBranch(&sub_branches));
	if(!var) freeLast(&sub_branches);

	if(var)
	{
		i+=1;
		a=ALPHA(str+i, addBranch(&sub_branches));
		if(!a) freeLast(&sub_branches);

		b=DIGIT(str+i, addBranch(&sub_branches));
		if(!b) freeLast(&sub_branches);

		while(a || b || str[i]=='+' || str[i]=='-' || str[i]=='.')
		{
			i+=1;
			a=ALPHA(str+i, addBranch(&sub_branches));
			if(!a) freeLast(&sub_branches);

			b=DIGIT(str+i, addBranch(&sub_branches));
			if(!b) freeLast(&sub_branches);
		}
		res = i;
	}
	else
	{
		res = 0;
	}
	(*branch)->branches = sub_branches;
	(*branch)->data_size = res;
	return res;
}


int quoted_pair(char *str, Branch** branch)
{
	*branch = new_branch(str, 0, "quoted_pair");
	BranchList* sub_branches = new_branchlist();

	if(str[0]=='\\')
	{
		int var = vchar(str+1, addBranch(&sub_branches));
		if(!var) freeLast(&sub_branches);

		int var2 = obs_text(str+1, addBranch(&sub_branches));
		if(!var2) freeLast(&sub_branches);

		if(str[1]==0x09 || str[1]==' ' || var || var2)
		{
			*branch = new_branch(str, 2, "quoted_pair");
			return 2;
		}
	}
	return 0;
}


int comment(char *str, Branch** branch)
{
	*branch = new_branch(str, 0, "comment");
	BranchList* sub_branches = new_branchlist();
	int res=0;
	int i=0;

	if(str[i]=='(')
	{
		i+=1;

		int a1 = ctext(str+i, addBranch(&sub_branches));
		if(!a1) freeLast(&sub_branches);
		int a2 = quoted_pair(str+i, addBranch(&sub_branches));
		if(!a2) freeLast(&sub_branches);


		int a3 = comment(str+i, addBranch(&sub_branches));

		if(!a3) freeLast(&sub_branches);
		if(a1)
		i+=a1;
		else if(a2)
		i+=a2;
		else i+=a3;
		while( a1 || a2 || a3 )
		{
 			a1= ctext(str+i, addBranch(&sub_branches));
			if(!a1) freeLast(&sub_branches);

			if(a1) i+=1;
			else{
				a2 = quoted_pair(str+i, addBranch(&sub_branches));
				if(!a2) freeLast(&sub_branches);

				if(a2) i+=2;
				else{
					a3 = comment(str+i, addBranch(&sub_branches));
					if(!a3) freeLast(&sub_branches);

					if(a3) i+=comment(str+i, addBranch(&sub_branches)); //wip
				}
			}
		}
		if(str[i]==')') res = i+1;
	}
	else res = 0;

	(*branch)->branches = sub_branches;
	(*branch)->data_size = res;
	return res;
}


int tchar(char *str, Branch** branch)
{
	*branch = new_branch(str, 0, "tchar");
	BranchList* sub_branches = new_branchlist();

	int res = 0;

	int var1 = DIGIT(str, addBranch(&sub_branches));
	if(!var1) freeLast(&sub_branches);

	int var2 = ALPHA(str, addBranch(&sub_branches));
	if(!var2) freeLast(&sub_branches);

	if(*str=='!' || *str=='#'||*str=='$'||*str=='%'||*str=='&'||*str=='\'' || *str== '*' ||*str== '+' || *str== '-' || *str=='.' || *str=='^' || *str=='_' || *str=='`' || *str== '|' || *str=='~' || var1 || var2 )
	{
		res = 1;
	}
	else res = 0;

	(*branch)->branches = sub_branches;
	(*branch)->data_size = res;
	return res;
}

int token(char* str, Branch** branch) // 1* tchar
{
	*branch = new_branch(str, 0, "token");
	BranchList* sub_branches = new_branchlist();
	int res;
	int i=0;

	int a1 = tchar(str, addBranch(&sub_branches));
	if(!a1) freeLast(&sub_branches);

	if(a1!=1)
		res = 0;
	else{
		i++;
		int a2 = tchar(str+i, addBranch(&sub_branches));
		if(!a2) freeLast(&sub_branches);

		while(a2)
		{
			i+=1;

			a2 = tchar(str+i, addBranch(&sub_branches));
			if(!a2) freeLast(&sub_branches);
		}
		res = i;
	}
	#ifdef DEBUG
	printf("token returns %d\n", res);
	#endif /* DEBUG */
	(*branch)->branches = sub_branches;
	(*branch)->data_size = res;

	return res;
}

int content_coding(char *str, Branch** branch)
{
	*branch = new_branch(str, 0, "content_coding");
	BranchList* sub_branches = new_branchlist();

	int i = token(str, addBranch(&sub_branches));
	if(!i) freeLast(&sub_branches);

	(*branch)->branches = sub_branches;
	(*branch)->data_size = i;
	return i;
}

int codings(char *str, Branch** branch)
{
	*branch = new_branch(str, 0, "codings");
	BranchList* sub_branches = new_branchlist();

	int i=content_coding(str, addBranch(&sub_branches));
	if(!i) freeLast(&sub_branches);

	if(i);
	else if(str[i]=='i'){
		if(str[i+1]=='d'){
			if(str[i+2]=='e'){
				if(str[i+3]=='n'){
					if(str[i+4]=='t'){
						if(str[i+5]=='i'){
							if(str[i+6]=='t'){
								if(str[i+7]=='y'){
									i=8;
								}
							}
						}
					}
				}
			}
		}
	}
	else if(str[i]=='*')
	{
		i=1;
	}

	(*branch)->branches = sub_branches;
	(*branch)->data_size = i;
	return i;
}


int Accept_Encoding(char* str, Branch** branch) //  [ ( "," / ( codings [ weight ] ) ) * ( OWS "," [ OWS ( codings [ weight ] ) ] ) ]
{
	*branch = new_branch(str, 0, "Accept_Encoding");
	BranchList* sub_branches = new_branchlist();

	int i=codings(str, addBranch(&sub_branches));
	if(!i) freeLast(&sub_branches);

	int a=0,b=0,c=0,d=0;
	if(i||str[i]==','){
		if(i){
			int var = weight(str+i, addBranch(&sub_branches));
			if(!var) freeLast(&sub_branches);
			i+=var;
		}
		a=OWS(str+i, addBranch(&sub_branches));
		//if(!a) freeLast(&sub_branches);

		while(str[i+a]==',')
		{
			i+=a+1;
			b=OWS(str+i, addBranch(&sub_branches));
			//if(!b) freeLast(&sub_branches);

			c=codings(str+i+b, addBranch(&sub_branches));
			if(!c) freeLast(&sub_branches);

			if(c)
			{
				d=weight(str+i+c+b, addBranch(&sub_branches));
				if(!d) freeLast(&sub_branches);

				i+=d+c+b;
			}
		}
		}
		#ifdef DEBUG
		printf("Accept_Encoding returns %d\n", i);
		#endif /* DEBUG */
		(*branch)->branches = sub_branches;
		(*branch)->data_size = i;
		return i;
}

int connection_option(char *str, Branch** branch)
{
	*branch = new_branch(str, 0, "connection_option");
	BranchList* sub_branches = new_branchlist();
	int i=token(str, addBranch(&sub_branches));
	if(!i) freeLast(&sub_branches);
	(*branch)->branches = sub_branches;
	(*branch)->data_size = i;
	#ifdef DEBUG
	printf("connection_option returns %d\n", i);
	#endif /* DEBUG */
	return i;
}

int Connection(char *str, Branch** branch) //* ( "," OWS ) connection-option * ( OWS "," [ OWS connection-option ] )
{
	*branch = new_branch(str, 0, "Connection");
	BranchList* sub_branches = new_branchlist();

	int i=0;
	int a;
	int b;
	while(str[i]==',')
	{
		int var = OWS(str+i+1, addBranch(&sub_branches));
		if(!var) freeLast(&sub_branches);
		i+=1+var;
	}
	a=connection_option(str+i, addBranch(&sub_branches));
	if(!a) freeLast(&sub_branches);

	if(a)
	{
		i+=a;
		a=OWS(str+i, addBranch(&sub_branches));
		//if(!a) freeLast(&sub_branches);

		while(str[i+a]==',')
		{
			i+=a;

			a=OWS(str+i, addBranch(&sub_branches));
			//if(!a) freeLast(&sub_branches);

			b=connection_option(str+i+a, addBranch(&sub_branches));
			if(!b) freeLast(&sub_branches);

			if(b)
			{
				i+=a+b;
			}
		}
	}
	else{
		i=0;
	}

	(*branch)->branches = sub_branches;
	(*branch)->data_size = i;
	#ifdef DEBUG
	printf("Connection returns %d\n", i);
	#endif /* DEBUG */
	return i;
}


int method(char *str, Branch** branch)
{
	*branch = new_branch(str, 0, "method");
	BranchList* sub_branches = new_branchlist();
	int i=token(str, addBranch(&sub_branches));
	if(!i) freeLast(&sub_branches);
	(*branch)->branches = sub_branches;
	(*branch)->data_size = i;
	#ifdef DEBUG
	printf("method returns %d\n", i);
	#endif /* DEBUG */
	return i;
}


int product_version(char* str, Branch** branch) // token
{
	*branch = new_branch(str, 0, "product_version");
	BranchList* sub_branches = new_branchlist();
	int i=token(str, addBranch(&sub_branches));
	if(!i) freeLast(&sub_branches);
	(*branch)->branches = sub_branches;
	(*branch)->data_size = i;
	return i;
}

int type(char* str, Branch** branch)
{
	*branch = new_branch(str, 0, "type");
	BranchList* sub_branches = new_branchlist();
	int i = token(str, addBranch(&sub_branches));
	if(!i) freeLast(&sub_branches);
	(*branch)->branches = sub_branches;
	(*branch)->data_size = i;
	#ifdef DEBUG
	printf("type returns %d\n", i);
	#endif /* DEBUG */
	return i;
}
int subtype(char* str, Branch** branch)
{
	*branch = new_branch(str, 0, "subtype");
	BranchList* sub_branches = new_branchlist();
	int i = token(str, addBranch(&sub_branches));
	if(!i) freeLast(&sub_branches);
	(*branch)->branches = sub_branches;
	(*branch)->data_size = i;
	#ifdef DEBUG
	printf("subtype returns %d\n", i);
	#endif /* DEBUG */
	return i;
}

int product(char* str, Branch** branch) // token [ "/" product-version ]
{
	*branch = new_branch(str, 0, "product");
	BranchList* sub_branches = new_branchlist();

	int res;

	int i=token(str, addBranch(&sub_branches));
	if(!i) freeLast(&sub_branches);

	if(str[i]=='/')
	{
		int a = product_version(str+i+1, addBranch(&sub_branches));
		if(!a) freeLast(&sub_branches);

		if(a){
			i+=a+1;
			res = i;
		}
	}
	else res = i;

	(*branch)->branches = sub_branches;
	(*branch)->data_size = res;
	return res;
}



int User_Agent(char* str, Branch** branch)
{
	*branch = new_branch(str, 0, "User_Agent");
	BranchList* sub_branches = new_branchlist();
	int res;
	int i=product(str, addBranch(&sub_branches));

	if(!i) freeLast(&sub_branches);
	int a,b,c;
	if(i==0)
		res = 0;
	else
	{

		a=RWS(str+i, addBranch(&sub_branches));
		if(!a) freeLast(&sub_branches);
		b=product(str+i+a, addBranch(&sub_branches));
		if(!b) freeLast(&sub_branches);
		c=comment(str+i+a, addBranch(&sub_branches));
		if(!c) freeLast(&sub_branches);
		while(a && (b || c))
		{
			i+=a+b+c;
			a=RWS(str+i, addBranch(&sub_branches));
			if(!a) freeLast(&sub_branches);
			b=product(str+i+a, addBranch(&sub_branches));
			if(!b) freeLast(&sub_branches);
			c=comment(str+i+a, addBranch(&sub_branches));
			if(!c) freeLast(&sub_branches);
		}
		res = i;
	}
	(*branch)->branches = sub_branches;
	(*branch)->data_size = res;
	return res;
}

int HEXDIG(char *car, Branch** branch)
{
	int a = 0;

	if((*car>='0' && *car<='9') || (*car>='A' && *car<='F'))
	{
		a = 1;
	}

	*branch = new_branch(car, a, "HEXDIG");

	#ifdef DEBUG
	printf("HEXDIG returns %d\n",a);
	#endif /* DEBUG */
	return a;
}

int quoted_string(char* str, Branch** branch)
{
	*branch = new_branch(str, 0, "quoted_string");
	BranchList* sub_branches = NULL;

	int i=1,a,b;
	int c = 0;
	if(str[0]== 0x22)
	{
		a=qdtext(str+i,addBranch(&sub_branches));
		if(!a) freeLast(&sub_branches);
		b=quoted_pair(str+i,addBranch(&sub_branches));
		if(!b) freeLast(&sub_branches);

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
			c = i+1;
	}
	if(!c) freeLast(&sub_branches);
	(*branch)->branches = sub_branches;
	(*branch)->data_size = c;
	#ifdef DEBUG
	printf("quoted_string returns %d\n", c);
	#endif /* DEBUG */
	return c;
}

int accept_ext(char *str,Branch** branch) //OWS ";" OWS token [ "=" ( token / quoted-string ) ]
{
	*branch = new_branch(str, 0, "accept_ext");
	BranchList* sub_branches = NULL;

	int i=OWS(str, addBranch(&sub_branches));
	//if(!i) freeLast(&sub_branches);

	int a,b;
	if(str[i]==';')
	{
		int var = OWS(str, addBranch(&sub_branches));
		//if(!var) freeLast(&sub_branches);
		i+=var;

		a=token(str+i, addBranch(&sub_branches));
		if(!a) freeLast(&sub_branches);

		if(a){
			i+=a;
			if(str[i] == '=')
			{
				a=token(str+i+1,addBranch(&sub_branches));
				if(!a) freeLast(&sub_branches);

				b=quoted_string(str+i+1,addBranch(&sub_branches));
				if(!b) freeLast(&sub_branches);

				if(a){
					i+=a+1;
				}
				else if(b){
					i+=b+1;
				}
			}
		}
		else{
			i=0;
		}
	}
	else
	{
		i=0;
	}

	(*branch)->branches = sub_branches;
	(*branch)->data_size = i;
	return i;
}

int accept_params(char* str, Branch** branch) //weight * accept-ext
{
	*branch = new_branch(str, 0, "accept_params");
	BranchList* sub_branches = NULL;

	int i=weight(str, addBranch(&sub_branches));
	if(!i) freeLast(&sub_branches);

	int a;
	if(i){
		a=accept_ext(str+i,addBranch(&sub_branches));
		if(!a) freeLast(&sub_branches);

		while(a){
			i+=a;

			a=Accept(str+i, addBranch(&sub_branches));
			if(!a) freeLast(&sub_branches);
		}

	}
	#ifdef DEBUG
	printf("accept_params returns %d\n", i);
	#endif /* DEBUG */
	(*branch)->branches = sub_branches;
	(*branch)->data_size = i;
	return i;
}


int parameter(char *str, Branch** branch)
{
	*branch = new_branch(str, 0, "parameter");
	BranchList* sub_branches = NULL;

	int i=0;
	int a=token(str, addBranch(&sub_branches));
	if(!a) freeLast(&sub_branches);
	int b;
	int c;
	int d = 0;
	if(a)
	{
		i+=a;
		if(str[i]=='=')
		{
			i+=1;
			b=token(str+i, addBranch(&sub_branches));
			if(!b) freeLast(&sub_branches);
			c=quoted_string(str+i, addBranch(&sub_branches));
			if(!c) freeLast(&sub_branches);
			if(b)
			{
				d = i+b;

			}
			else if(c)
			{
				d = i+c;
			}
		}
	}
	if(!d) freeLast(&sub_branches);
	(*branch)->branches = sub_branches;
	(*branch)->data_size = d;
	#ifdef DEBUG
	printf("parameter return %d\n",d);
	#endif /* DEBUG */
	return d;
}


int media_range(char* str, Branch** branch)
{
	*branch = new_branch(str, 0, "media_range");
	BranchList* sub_branches = NULL;

	int res = 0;
	int i=0;

	int a = type(str, addBranch(&sub_branches));
	if(!a) freeLast(&sub_branches);
	int b = subtype(str+a+1, addBranch(&sub_branches));
	if(!b) freeLast(&sub_branches);

	int c;
	if((str[0]=='*' && str[1]=='/' && str[2]=='*') || (a && str[a]=='/' && b) || (a && str[a]== '/' && str[a+1]=='*'))
	{
		if(a)
		{
			i+=a;
			if(b)
			{
				i+=b+1;
			}
			else i+=2;
		}
		else i=3;
		a=OWS(str+i, addBranch(&sub_branches));
		//if(!a) freeLast(&sub_branches);
		b=OWS(str+i+a+1, addBranch(&sub_branches));
		//if(!b) freeLast(&sub_branches);
		c=parameter(str+i+a+b+i, addBranch(&sub_branches));
		if(!c) freeLast(&sub_branches);
		while(a & b &c){
			i+=a+b+c+1;
		}
		res = i;

	}

	(*branch)->branches = sub_branches;
	(*branch)->data_size = res;
	#ifdef DEBUG
	printf("media_range returns %d\n", res);
	#endif /* DEBUG */
	return res;
}

int sub_delims(char *car, Branch** branch)
{
	*branch = new_branch(car, 0, "sub_delims");
	BranchList* sub_branches = new_branchlist();
	int a = 0;

	if(*car=='!'||*car=='$'||*car=='&'||*car=='\''||*car=='('||*car==')'||*car=='*'||*car=='+'||*car== ',' ||*car== ';' || *car=='=')
	{
		a = 1;
	}
	if(!a) freeLast(&sub_branches);
	(*branch)->branches = sub_branches;
	(*branch)->data_size = a;
	#ifdef DEBUG
	printf("sub_delims returns %d\n", a);
	#endif /* DEBUG */
	return a;
}

int pct_encoded(char *str, Branch** branch)
{
	*branch = new_branch(str, 0, "sub_delims");
	BranchList* sub_branches = new_branchlist();
	int a = 0;
	if (str[0]=='%' && HEXDIG(str+1,addBranch(&sub_branches)) && HEXDIG(str+2,addBranch(&sub_branches)))
	{
		a = 3;
	}
	if(!a) freeLast(&sub_branches);
	(*branch)->branches = sub_branches;
	(*branch)->data_size = a;
	#ifdef DEBUG
	printf("pct_encoded returns %d\n", a);
	#endif /* DEBUG */
	return a;
}

int unreserved(char *car, Branch** branch)
{
	*branch = new_branch(car, 0, "unreserved");
	BranchList* sub_branches = new_branchlist();

	int res;

	int a = ALPHA(car, addBranch(&sub_branches));
	if(!a) freeLast(&sub_branches);
	int b = DIGIT(car, addBranch(&sub_branches));
	if(!b) freeLast(&sub_branches);

	if (a || b || *car=='-' || *car=='.' || *car=='_' || *car=='~')
	{
		res = 1;
	}
	else res = 0;

	(*branch)->branches = sub_branches;
	(*branch)->data_size = res;
	#ifdef DEBUG
	printf("unreserved returns %d\n", res);
	#endif /* DEBUG */
	return res;
}

int BWS(char* str, Branch** branch)
{
	*branch = new_branch(str, 0, "pchar");
	BranchList* sub_branches = new_branchlist();

	int i=OWS(str, addBranch(&sub_branches));

	(*branch)->branches = sub_branches;
	(*branch)->data_size = i;
	#ifdef DEBUG
	printf("OWS returns %d\n", i);
	#endif /* DEBUG */
	return i;
}


int pchar(char* str, Branch** branch)
{
	*branch = new_branch(str, 0, "pchar");
	BranchList* sub_branches = new_branchlist();
	int a = 0;

	if(unreserved(str,addBranch(&sub_branches))  || sub_delims(str,addBranch(&sub_branches)) || str[0]==':' || str[0]=='@'){
		a = 1;
	}
	else if(str[0]=='%')
	{

		a = pct_encoded(str, addBranch(&sub_branches));
	}

	if(!a) freeLast(&sub_branches);
	(*branch)->branches = sub_branches;
	(*branch)->data_size = a;
	return a;
}

int asterisk_form(char *str, Branch** branch)
{
	*branch = new_branch(str, 0, "asterisk_form");

	int i=0;
	if(str[i]=='*')
	{
		*branch = new_branch(str, 0, "asterisk_form");
		i+=1;
	}

	(*branch)->data_size = i;
	return i;
}

int transfer_parameter(char *str, Branch** branch)
{
	*branch = new_branch(str, 0, "sub_delims");
	BranchList* sub_branches = new_branchlist();

	int a;
	int b;

	int i=token(str, addBranch(&sub_branches));
	if(!i) freeLast(&sub_branches);

	if(i){
		a=BWS(str+i, addBranch(&sub_branches));
		//if(!a) freeLast(&sub_branches);

		i+=a;
		if(str[i]=='='){
			a=BWS(str+i+1, addBranch(&sub_branches));
			//if(!a) freeLast(&sub_branches);

			i+=a+1;
			a=token(str+i, addBranch(&sub_branches));
			if(!a) freeLast(&sub_branches);

			b=quoted_string(str+i, addBranch(&sub_branches));
			if(!b) freeLast(&sub_branches);

			if(a){
				i+=a;
			}
			else if(b){
				i+=b;
			}
		}
		else i=0;
	}

	(*branch)->branches = sub_branches;
	(*branch)->data_size = i;
	return i;
}

int transfer_extension(char *str, Branch** branch)
{
	*branch = new_branch(str, 0, "sub_delims");
	BranchList* sub_branches = new_branchlist();

	int i=token(str, addBranch(&sub_branches));
	if(!i) freeLast(&sub_branches);

	int a,b,c;

	if(i)
	{
		a=OWS(str+i, addBranch(&sub_branches));
		//if(!a) freeLast(&sub_branches);
		b=OWS(str+a+i+1,addBranch(&sub_branches));
		//if(!b) freeLast(&sub_branches);
		c=transfer_parameter(str+i,addBranch(&sub_branches));
		if(!c) freeLast(&sub_branches);
		while(a && b && c && str[a+1]==';')
		{
			i+=a+b+c+1;
			a=transfer_parameter(str+i,addBranch(&sub_branches));
			if(!a) freeLast(&sub_branches);
			b=OWS(str+a+i+1,addBranch(&sub_branches));
			//if(!b) freeLast(&sub_branches);
			c=transfer_parameter(str+i,addBranch(&sub_branches));
			if(!c) freeLast(&sub_branches);
		}
	}

	(*branch)->branches = sub_branches;
	(*branch)->data_size = i;
	return i;
}

int transfer_coding(char* str, Branch** branch)
{
	*branch = new_branch(str, 0, "transfer_coding");
	BranchList* sub_branches = new_branchlist();

	int i=transfer_coding(str,addBranch(&sub_branches));
	if(i);
	else if(strcomp(str,"chunked")) i=7;
	else if(strcomp(str,"compress")) i=8;
	else if(strcomp(str,"deflate")) i=7;
	else if(strcomp(str,"gzip")) i=4;

	(*branch)->branches = sub_branches;
	(*branch)->data_size = i;
	return i;
}

int Transfer_Encoding(char* str, Branch** branch)
{
	*branch = new_branch(str, 0, "Transfer_Encoding");
	BranchList* sub_branches = new_branchlist();

	int i=0;
	int a=OWS(str+1, addBranch(&sub_branches));
	//if(!a) freeLast(&sub_branches);

	int b,c;
	while(str[i]==',')
	{
		i+=1+a;
		a=OWS(str+i+1, addBranch(&sub_branches));
		//if(!a) freeLast(&sub_branches);
	}
	a=transfer_coding(str+i, addBranch(&sub_branches));
	if(!a) freeLast(&sub_branches);
	if(a)
	{
		i+=a;
		a=OWS(str+i, addBranch(&sub_branches));
		//if(!a) freeLast(&sub_branches);
		b=OWS(str+i+a+1, addBranch(&sub_branches));
		//if(!b) freeLast(&sub_branches);
		c=transfer_coding(str+i+a+1+b, addBranch(&sub_branches));
		if(!c) freeLast(&sub_branches);
		while(b&&str[i+a]==',')
		{
			i+=a+b+c+1;
			a=OWS(str+i, addBranch(&sub_branches));
			//if(!a) freeLast(&sub_branches);
			b=OWS(str+i+a+1, addBranch(&sub_branches));
			//if(!b) freeLast(&sub_branches);
			c=transfer_coding(str+i+a+1+b, addBranch(&sub_branches));
			if(!c) freeLast(&sub_branches);
		}
	}
	else i=0;

	(*branch)->branches = sub_branches;
	(*branch)->data_size = i;
	return i;
}

int cookie_octet(char *str,Branch** branch)
{

	int i=0;
	if((str[0]==0x21) || (str[0]>=0x23 && str[0]<=0x2B) || (str[0]>=0x2D && str[0]<=0x3A) || (str[0]>=0x3C && str[0]<=0x5B) || (str[0]>=0x5D && str[0]<=0x7E)){
		i=1;
	}

	*branch = new_branch(str, i, "cookie_octet");
	return i;
}

int cookie_name(char *str, Branch** branch)
{
	*branch = new_branch(str, 0, "cookie_name");
	BranchList* sub_branches = new_branchlist();

	int i=token(str, addBranch(&sub_branches));
	if (!i) freeLast(&sub_branches);

	(*branch)->branches = sub_branches;
	(*branch)->data_size = i;
	return i;
}

int cookie_value(char *str, Branch** branch)
{
	*branch = new_branch(str, 0, "cookie_value");
	BranchList* sub_branches = new_branchlist();

	int i=0;
	int a;
	if(str[i]==0x22){
		i+=1;
		a=cookie_octet(str+i, addBranch(&sub_branches));
		if(!a) freeLast(&sub_branches);
		while(a){
			i+=a;
			a=cookie_octet(str+i, addBranch(&sub_branches));
			if(!a) freeLast(&sub_branches);
		}
		if(str[i]==0x22){
			i+=1;
		}
	}
	else
	{
		a=cookie_octet(str, addBranch(&sub_branches));
		if(!a) freeLast(&sub_branches);
		while(a)
		{
			i+=a;
			a=cookie_octet(str+i, addBranch(&sub_branches));
			if(!a) freeLast(&sub_branches);
		}
	}
	(*branch)->branches = sub_branches;
	(*branch)->data_size = i;
	return i;
}

int cookie_pair(char* str, Branch** branch)
{
	*branch = new_branch(str, 0, "cookie_pair");
	BranchList* sub_branches = new_branchlist();

	int i=0;
	int a=cookie_name(str, addBranch(&sub_branches));
	if(!a) freeLast(&sub_branches);
	int b=cookie_value(str+a+1, addBranch(&sub_branches));
	if(!b) freeLast(&sub_branches);
	if(a && str[a]=='=') i = a+b+1;

	(*branch)->branches = sub_branches;
	(*branch)->data_size = i;
	return i;
}

int cookie_string(char* str, Branch** branch)
{
	*branch = new_branch(str, 0, "cookie_pair");
	BranchList* sub_branches = new_branchlist();

	int i=cookie_pair(str, addBranch(&sub_branches));
	if(!i) freeLast(&sub_branches);
	int a;
	if(i)
	{
		a=cookie_pair(str, addBranch(&sub_branches));
		if(!a) freeLast(&sub_branches);
		if(a)
			while(str[i]==';' && str[i+1]==' '&&a)
			{
				i+=2+a;
				a=cookie_pair(str, addBranch(&sub_branches));
				if(!a) freeLast(&sub_branches);
			}
		else i=0;
	}
	(*branch)->branches = sub_branches;
	(*branch)->data_size = i;
	return i;
}

int segment(char *str, Branch** branch)
{
	*branch = new_branch(str, 0, "segment");
	BranchList* sub_branches = new_branchlist();

	int i=0;
	int a = pchar(str+i, addBranch(&sub_branches));
	if(!a) freeLast(&sub_branches);
	while(a)
	{
		a = pchar(str+i, addBranch(&sub_branches));
		if(!a) freeLast(&sub_branches);

		i+=a;
	}
	(*branch)->branches = sub_branches;
	(*branch)->data_size = i;
	return i;
}

int absolute_path(char* str, Branch** branch) //1* ("/" segment)
{
	*branch = new_branch(str, 0, "segment");
	BranchList* sub_branches = new_branchlist();

	int i=0;
	while(str[i]=='/')
	{
		i+=1;
		int var = segment(str+i, addBranch(&sub_branches));
		if(!var) freeLast(&sub_branches);
		i+=var;
	}
	(*branch)->branches = sub_branches;
	(*branch)->data_size = i;
	return i;
}

int query(char* str, Branch** branch) //  * ( pchar / "/" / "?" )
{
	*branch = new_branch(str, 0, "query");
	BranchList* sub_branches = new_branchlist();

	int i=0;

	int var = pchar(str+i, addBranch(&sub_branches));
	if(!var) freeLast(&sub_branches);

	while(var||str[i]=='/'||str[i]=='?')
	{
		if(str[i]=='%')
			i+=3;
		else
			i+=1;
	}
	(*branch)->branches = sub_branches;
	(*branch)->data_size = i;
	return i;
}


int userinfo(char* str,Branch** branch)
{
	*branch = new_branch(str, 0, "userinfo");
	BranchList* sub_branches = new_branchlist();

	int i=0;
	int a=unreserved(str, addBranch(&sub_branches));
	if(!a) freeLast(&sub_branches);
	int b=pct_encoded(str, addBranch(&sub_branches));
	if(!b) freeLast(&sub_branches);
	int c=sub_delims(str, addBranch(&sub_branches));
	if(!c) freeLast(&sub_branches);

	while(str[i]==':' || a || b || c)
	{
		i+=1;
		a=unreserved(str, addBranch(&sub_branches));
		if(!a) freeLast(&sub_branches);
		b=pct_encoded(str, addBranch(&sub_branches));
		if(!b) freeLast(&sub_branches);
		c=sub_delims(str, addBranch(&sub_branches));
		if(!c) freeLast(&sub_branches);
	}
	(*branch)->branches = sub_branches;
	(*branch)->data_size = i;
	return i;
}


int dec_octet(char *str, Branch** branch){
	*branch = new_branch(str, 0, "userinfo");
	BranchList* sub_branches = new_branchlist();

	int i=0;
	int a,b;

	if(str[0]=='2')
	{
		if(str[1]=='5')
		{
			if(str[2]>=0x30 && str[2]<=0x35)
			{
				i=3;
			}
		}
		else if(str[1]>=0x30 && str[1]<=0x34)
		{
			a=DIGIT(str+2, addBranch(&sub_branches));
			if(!a) freeLast(&sub_branches);
			if(a)
			{
				i=3;
			}
		}
	}
	else if(str[0]=='1')
	{
		a=DIGIT(str+1, addBranch(&sub_branches));
		if(!a) freeLast(&sub_branches);
		if(a)
		{
			b=DIGIT(str+2, addBranch(&sub_branches));
			if(!b) freeLast(&sub_branches);
			if(b)
			{
				i=3;
			}
		}

	}


	if(i==0 && str[0]>=0x31 && str[0]<=0x39)
	{
		a=DIGIT(str+1, addBranch(&sub_branches));
		if(!a) freeLast(&sub_branches);
		if(a)
		{
			i=2;
		}
	}
	a=DIGIT(str, addBranch(&sub_branches));
	if(!a) freeLast(&sub_branches);
	if(i==0 && a)
	{
		i=1;
	}
	(*branch)->branches = sub_branches;
	(*branch)->data_size = i;
	#ifdef DEBUG
	printf("dec_octet returns %d\n",i);
	#endif /* DEBUG */
	return i;
}

int IPv4address(char *str,Branch** branch)
{
	*branch = new_branch(str, 0, "userinfo");
	BranchList* sub_branches = new_branchlist();

	int i=dec_octet(str, addBranch(&sub_branches));
	int a;
	if(i && str[i]=='.')
	{
		i++;
		a=dec_octet(str+i, addBranch(&sub_branches));
		if(!a) freeLast(&sub_branches);
		if(a && str[i+a]=='.'){
			i+=a+1;
			a=dec_octet(str+i, addBranch(&sub_branches));
			if(!a) freeLast(&sub_branches);
			if(a && str[i+a]=='.'){
				i+=a+1;
				a=dec_octet(str+i, addBranch(&sub_branches));
				if(!a) freeLast(&sub_branches);
				if(a)
				{
					i+=a;
				}
				else i=0;
			}
			else i=0;
		}
		else i=0;
	}
	else i=0;

	(*branch)->branches = sub_branches;
	(*branch)->data_size = i;
	#ifdef DEBUG
	printf("IPv4address returns %d\n", i);
	#endif /* DEBUG */
	return i;
}

int host(char *str, Branch** branch)
{
	*branch = new_branch(str, 0, "userinfo");
	BranchList* sub_branches = new_branchlist();

	int i;
	int a=IPv4address(str, addBranch(&sub_branches));
	if(!a) freeLast(&sub_branches);
	int b=reg_name(str, addBranch(&sub_branches));
	if(!b) freeLast(&sub_branches);
	if(a)
	{
		i=a;
	}
	else if(b)
	{
		i=b;
	}
	(*branch)->branches = sub_branches;
	(*branch)->data_size = i;
	#ifdef DEBUG
	printf("host returns %d\n", i);
	#endif /* DEBUG */
	return i;
}



int h16(char *str, Branch** branch)
{
	*branch = new_branch(str, 0, "userinfo");
	BranchList* sub_branches = new_branchlist();

	int i=0;
	int var = HEXDIG(str+i, addBranch(&sub_branches));
	if(!var) freeLast(&sub_branches);

	while(var && i<4){
		i++;
	}
	(*branch)->branches = sub_branches;
	(*branch)->data_size = i;
	return i;
}

int ls32(char *str, Branch** branch)
{
	*branch = new_branch(str, 0, "userinfo");
	BranchList* sub_branches = new_branchlist();

	int i=h16(str, addBranch(&sub_branches));
	if(!i) freeLast(&sub_branches);
	int a=IPv4address(str, addBranch(&sub_branches));
	if(!a) freeLast(&sub_branches);

	if(i && str[i]==':')
	{
		i++;
		a=h16(str+i, addBranch(&sub_branches));
		if(!a) freeLast(&sub_branches);

		if(a){
			i+=a;
		}
		else{
			i=0;
		}
	}
	else if(a)
	{
		i=a;
	}
	(*branch)->branches = sub_branches;
	(*branch)->data_size = i;
	return i;
}

int Content_Lenght(char* str, Branch** branch)
{
	*branch = new_branch(str, 0, "userinfo");
	BranchList* sub_branches = new_branchlist();

	int i=0;
	int a=DIGIT(str+i, addBranch(&sub_branches));
	if(!a) freeLast(&sub_branches);
	while(a)
	{
		a=DIGIT(str+i, addBranch(&sub_branches));
		if(!a) freeLast(&sub_branches);
	}

	(*branch)->branches = sub_branches;
	(*branch)->data_size = i;
	return i;
}

int Content_Lenght_Header(char* str, Branch** branch) //"Content-Length" ":" OWS Content-Length OWS
{
	*branch = new_branch(str, 0, "userinfo");
	BranchList* sub_branches = new_branchlist();

	int i=0;
	int a;
	int b;
	if(strcomp(str,"Content-Lenght:"))
	{
		a=OWS(str+15, addBranch(&sub_branches));
		//if(!a) freeLast(&sub_branches);
		b=Content_Lenght(str+i+a+15, addBranch(&sub_branches));
		if(!b) freeLast(&sub_branches);
		if(b)
		{
			i+=a+15+b;
			a=OWS(str+i, addBranch(&sub_branches));
			//if(!a) freeLast(&sub_branches);
			i+=a;
		}
	}
	(*branch)->branches = sub_branches;
	(*branch)->data_size = i;
	return i;
}

int authority(char* str, Branch** branch)
{
	*branch = new_branch(str, 0, "authority");
	BranchList* sub_branches = new_branchlist();

	int i=userinfo(str, addBranch(&sub_branches));
	if(!i) freeLast(&sub_branches);
	int a;
	if((i && str[i]=='@') || !i){
		i+=1;
		a=host(str+i, addBranch(&sub_branches));
		if(!a) freeLast(&sub_branches);
		if(a)
		{
			i+=a;
			if(str[i]==':')
			{
				a=port(str+i+1, addBranch(&sub_branches));
				if(!a) freeLast(&sub_branches);
				i+=1+a;
			}
		}
		else
		{
			i=0;
		}

	}
	(*branch)->branches = sub_branches;
	(*branch)->data_size = i;
	#ifdef DEBUG
	printf("port returns %d\n", i);
	#endif /* DEBUG */
	return i;
}

int hier_part(char* str, Branch** branch) // "//" hority path-abempty / path-absolute / path-rootless / path-empty
{
	*branch = new_branch(str, 0, "hier_part");
	BranchList* sub_branches = new_branchlist();

	int i=0;

	if(str[0]=='/')
	{
		if(str[1]=='/')
		{
			i=authority(str+2, addBranch(&sub_branches));
			if(!i) freeLast(&sub_branches);
			if(i)
			{
				i+=2;
			}
		}
	}
	(*branch)->branches = sub_branches;
	(*branch)->data_size = i;
	return i;
}

int reg_name(char* str, Branch** branch)
{
	*branch = new_branch(str, 0, "hier_part");
	BranchList* sub_branches = new_branchlist();
	int i=0;
	int a=unreserved(str, addBranch(&sub_branches));
	if(!a) freeLast(&sub_branches);
	int b=pct_encoded(str, addBranch(&sub_branches));
	if(!b) freeLast(&sub_branches);
	int c=sub_delims(str, addBranch(&sub_branches));
	if(!c) freeLast(&sub_branches);
	while(a || b || c)
	{
		i+=a+b+c;
		a=unreserved(str+i, addBranch(&sub_branches));
		if(!a) freeLast(&sub_branches);
		b=pct_encoded(str+i, addBranch(&sub_branches));
		if(!b) freeLast(&sub_branches);
		c=sub_delims(str+i, addBranch(&sub_branches));
		if(!c) freeLast(&sub_branches);
	}
	#ifdef DEBUG
	printf("reg_name returns %d\n", i);
	#endif /* DEBUG */
	(*branch)->branches = sub_branches;
	(*branch)->data_size = i;
	return i;
}

int port(char* str, Branch **branch) //* DIGIT
{
	*branch = new_branch(str, 0, "hier_part");
	BranchList* sub_branches = new_branchlist();

	int i=0;
	int a=DIGIT(str, addBranch(&sub_branches));
	if(!a) freeLast(&sub_branches);
	while(a){
		i+=1;
		a=DIGIT(str+i,addBranch(&sub_branches));
		if(!a) freeLast(&sub_branches);
	}
	(*branch)->branches = sub_branches;
	(*branch)->data_size = i;
	return i;
}

int uri_host(char* str, Branch** branch)
{
	*branch = new_branch(str, 0, "uri_host");
	BranchList* sub_branches = new_branchlist();

	int i=host(str, addBranch(&sub_branches));
	if(!i) freeLast(&sub_branches);
	(*branch)->branches = sub_branches;
	(*branch)->data_size = i;
	#ifdef DEBUG
	printf("uri_host returns %d\n", i);
	#endif /* DEBUG */
	return i;
}

int Host(char* str, Branch** branch)
{
	*branch = new_branch(str, 0, "Host");
	BranchList* sub_branches = new_branchlist();

	int i=uri_host(str,addBranch(&sub_branches));
	if(!i) freeLast(&sub_branches);
	int a;
	if(i)
	{
		if(str[i]==':')
		{
			a=port(str, addBranch(&sub_branches));
			if(!a) freeLast(&sub_branches);
			i+=a+1;
		}
	}
	(*branch)->branches = sub_branches;
	(*branch)->data_size = i;
	#ifdef DEBUG
	printf("Host returns %d\n", i);
	#endif /* DEBUG */
	return i;
}



int relative_part(char* str,Branch** branch)
{
	*branch = new_branch(str, 0, "relative_part");
	BranchList* sub_branches = new_branchlist();

	int i=0;
	if(str[0]=='/')
	{
		if(str[1]=='/')
		{
			i=authority(str, addBranch(&sub_branches));
			if(!i) freeLast(&sub_branches);
		}
	}
	(*branch)->branches = sub_branches;
	(*branch)->data_size = i;
	return i;
}

int partial_URI(char* str, Branch** branch)
{
	*branch = new_branch(str, 0, "relative_part");
	BranchList* sub_branches = new_branchlist();

	int i=relative_part(str, addBranch(&sub_branches));
	if(!i) freeLast(&sub_branches);
	int a;
	if(i && str[i]=='?'){
		a=query(str+i+1, addBranch(&sub_branches));
		if(!a) freeLast(&sub_branches);
		i+=a;
	}
	(*branch)->branches = sub_branches;
	(*branch)->data_size = i;
	return i;
}

int absolute_URI(char* str,Branch** branch)
{
	*branch = new_branch(str, 0, "absolute_URI");
	BranchList* sub_branches = new_branchlist();

	int i=scheme(str, addBranch(&sub_branches));
	if(!i) freeLast(&sub_branches);
	int a;
	if(i && str[i]==':'){
		i+=1;
		a=hier_part(str+i, addBranch(&sub_branches));
		if(!a) freeLast(&sub_branches);
		if(a && str[i+a]=='?'){
			i+=a+1;
			a=query(str+i,addBranch(&sub_branches));
			if(!a) freeLast(&sub_branches);
			if(a){
				i+=a;
			}
			else i=0;
		}
		else i=0;
	}
	(*branch)->branches = sub_branches;
	(*branch)->data_size = i;
	return i;
}

int Connection_header(char* str, Branch** branch)
{
	*branch = new_branch(str, 0, "Connection_header");
	BranchList* sub_branches = new_branchlist();
	int i=0;

	int a,b,c;
	if(strcomp(str,"Connection:"))
	{
		i=11;
		a=OWS(str+i, addBranch(&sub_branches));
		//if(!a) freeLast(&sub_branches);
		b=Connection(str + a+i, addBranch(&sub_branches));
		//if(!b) freeLast(&sub_branches);
		c=OWS(str + a+i+b, addBranch(&sub_branches));
		//if(!c) freeLast(&sub_branches);
		if(b)
		{
			i+=a+b+c;
		}
		else i=0;
	}
	(*branch)->branches = sub_branches;
	(*branch)->data_size = i;
	#ifdef DEBUG
	printf("Connection_header returns %d\n",i);
	#endif /* DEBUG */
	return i;
}


int Accept_Encoding_header(char* str, Branch** branch)
{
	*branch = new_branch(str, 0, "Accept_Encoding_header");
	BranchList* sub_branches = new_branchlist();

	int i=0;
	int a,b,c;
	if(strcomp(str,"Accept-Encoding:"))
	{
		i=16;
		a=OWS(str+i, addBranch(&sub_branches));
		//if(!a) freeLast(&sub_branches);
		b=Accept_Encoding(str+a+i, addBranch(&sub_branches));
		if(!b) freeLast(&sub_branches);
		c=OWS(str+i+a+b, addBranch(&sub_branches));
		//if(!c) freeLast(&sub_branches);
		if(b)
		{
			i+=a+b+c;
		}
		else i=0;
	}
	(*branch)->branches = sub_branches;
	(*branch)->data_size = i;
	#ifdef DEBUG
	printf("Accept_Encoding_header returns %d\n", i);
	#endif /* DEBUG */

	return i;
}

int Accept_header(char* str,Branch** branch)
{
	*branch = new_branch(str, 0, "Accept_header");
	BranchList* sub_branches = new_branchlist();
	int i=0;
	int a,b,c;
	if(strcomp(str,"Accept:"))
	{
		i=7;
		a=OWS(str+i, addBranch(&sub_branches));

		b=Accept(str+a+i, addBranch(&sub_branches));
		if(!b) freeLast(&sub_branches);

		c=OWS(str+i+a+b, addBranch(&sub_branches));
		//if(!c) freeLast(&sub_branches);
		if(b)
		{
			i+=a+b+c;
		}
		else i=0;
	}
	(*branch)->branches = sub_branches;
	(*branch)->data_size = i;
	return i;
}

int User_Agent_header(char* str,Branch** branch)
{
	*branch = new_branch(str, 0, "User_Agent_header");
	BranchList* sub_branches = new_branchlist();

	int i=0;
	int a,b,c;
	if(strcomp(str,"User-Agent:"))
	{
		i=11;
		a=OWS(str+i, addBranch(&sub_branches));
		//if(!a) freeLast(&sub_branches);
		b=User_Agent(str+a+i, addBranch(&sub_branches));
		if(!b) freeLast(&sub_branches);
		c=OWS(str+i+a+b, addBranch(&sub_branches));
		//if(!c) freeLast(&sub_branches);
		if(b)
		{
			i+=a+b+c;
		}
		else i=0;
	}
	(*branch)->branches = sub_branches;
	(*branch)->data_size = i;
	#ifdef DEBUG
	printf("User_Agent_header returns %d\n", i);
	#endif /* DEBUG */
	return i;
}

int referer(char* str, Branch** branch)
{
	*branch = new_branch(str, 0, "referer");
	BranchList* sub_branches = new_branchlist();

	int i=absolute_URI(str, addBranch(&sub_branches));
	if(!i) freeLast(&sub_branches);

	(*branch)->branches = sub_branches;
	(*branch)->data_size = i;
	return i;
}

int Referer_header(char* str, Branch** branch)
{
	*branch = new_branch(str, 0, "referer");
	BranchList* sub_branches = new_branchlist();

	int i=0;
	int a,b,c;

	if(strcomp(str,"Referer:"))
	{
		i=8;
		a=OWS(str+i, addBranch(&sub_branches));
		//if(!a) freeLast(&sub_branches);
		b=referer(str+a+i, addBranch(&sub_branches));
		if(!b) freeLast(&sub_branches);
		c=OWS(str+i+a+b, addBranch(&sub_branches));
		//if(!c) freeLast(&sub_branches);
		if(b)
		{
			i+=a+b+c;
		}
		else i=0;
	}
	(*branch)->branches = sub_branches;
	(*branch)->data_size = i;
	return i;
}





int origin_form(char* str, Branch** branch)
{
	*branch = new_branch(str, 0, "origin_form");
	BranchList* sub_branches = new_branchlist();

	int i=absolute_path(str, addBranch(&sub_branches));
	if(!i) freeLast(&sub_branches);

	int res;
	if(i==0)
	{
		res= 0;
	}
	else
	{
		if(str[i]=='?')
		{
			int a = query(str+i, addBranch(&sub_branches));
			if(!a) freeLast(&sub_branches);
			i+=1+a;
			res = i;
		}
		else
		{
			res = i;
		}
	}
	(*branch)->branches = sub_branches;
	(*branch)->data_size = res;
	return res;
}

int request_target(char *str, Branch** branch)
{
	*branch = new_branch(str, 0, "request_target");
	BranchList* sub_branches = new_branchlist();

	int i = origin_form(str, addBranch(&sub_branches));
	if(!i) freeLast(&sub_branches);

	(*branch)->branches = sub_branches;
	(*branch)->data_size = i;
	#ifdef DEBUG
	printf("request_target returns %d\n", i);
	#endif /* DEBUG */
	return i;

}

int HTTP_name(char *str, Branch** branch)
{
	if(*str=='H')
	{
		if(str[1]=='T')
		{
			if(str[2]=='T')
			{
				if(str[3]=='P')
				{
					*branch = new_branch(str, 4, "HTTP_name");
					return 4;
				}
			}
		}
	}
	*branch = new_branch(str, 0, "HTTP_name");
	return 0;
}
int HTTP_version(char* str, Branch** branch)
{
	*branch = new_branch(str, 0, "HTTP_version");
	BranchList* sub_branches = new_branchlist();

	int i=HTTP_name(str, addBranch(&sub_branches));
	if(!i) freeLast(&sub_branches);

	int a,res;

	if(i && str[i]=='/')
	{
		i+=1;
		a=DIGIT(str+i, addBranch(&sub_branches));
		if(!a) freeLast(&sub_branches);

		if(a && str[i+a]=='.')
		{
			i+=a+1;
			a=DIGIT(str+i, addBranch(&sub_branches));
			if(!a) freeLast(&sub_branches);

			if(a)
			{
				res = a+i;
			}
			else res = 0;
		}
		else res = 0;
	}
	else res = 0;

	(*branch)->branches = sub_branches;
	(*branch)->data_size = res;
	#ifdef DEBUG
	printf("HTTP_version returns %d\n", res);
	#endif /* DEBUG */
	return res;
}
int request_line(char *str, Branch** branch)
{
	*branch = new_branch(str, 0, "request_line");
	BranchList* sub_branches = new_branchlist();
	int res;
    int i=0;
    int a=method(str, addBranch(&sub_branches));
	if(!a) freeLast(&sub_branches);
    int b;
    if(a)
    {
    	i+=a;
    	if(str[i]==' ')
    	{
    		i+=1;
    		b=request_target(str+i, addBranch(&sub_branches));
			if(!b) freeLast(&sub_branches);
    		if(b)
    		{
    			i+=b;
    			if(str[i]==' ')
    			{
    				i+=1;
					a=HTTP_version(str+i, addBranch(&sub_branches));
					if(!a) freeLast(&sub_branches);
					if(str[i+a]==13)
					{
						if(str[i+a+1]==10)
						{
							res = i+a+2;
						}
						else res = 0;
					}
					else res = 0;
    			}
				else res = 0;
    		}
			else res = 0;
    	}
		else res = 0;
    }
	else res = 0;

	(*branch)->branches = sub_branches;
	(*branch)->data_size = res;
	#ifdef DEBUG
	printf("request_line returns %d\n", res);
	#endif /* DEBUG */
	return res;
}

int status_code(char* str, Branch** branch)
{
	*branch = new_branch(str, 0, "status_code");
	BranchList* sub_branches = new_branchlist();

	if(DIGIT(str, addBranch(&sub_branches)))
	{
		if(DIGIT(str+1, addBranch(&sub_branches)))
		{
			if(DIGIT(str+2, addBranch(&sub_branches)))
			{
				*branch = new_branch(str, 3, "status_code");
				return 3;
			}
		}
	}
	*branch = new_branch(str, 0, "status_code");
	return 0;
}

int reason_phrase(char *str, Branch** branch)
{
	*branch = new_branch(str, 0, "reason_phrase");
	BranchList* sub_branches = new_branchlist();

	int i=0;

	int a=obs_text(str, addBranch(&sub_branches));
	if(!a) freeLast(&sub_branches);
	int b=vchar(str, addBranch(&sub_branches));
	if(!b) freeLast(&sub_branches);

	while(str[i]== ' ' || str[i]==0x9 || a || b)
	{
		a=obs_text(str, addBranch(&sub_branches));
		if(!a) freeLast(&sub_branches);
		b=vchar(str, addBranch(&sub_branches));
		if(!b) freeLast(&sub_branches);

		i+=1;
	}

	(*branch)->branches = sub_branches;
	(*branch)->data_size = i;
	return i;
}

int status_line(char *str, Branch** branch)
{
	*branch = new_branch(str, 0, "status_line");
	BranchList* sub_branches = new_branchlist();

	int i=HTTP_version(str, addBranch(&sub_branches));
	if(!i) freeLast(&sub_branches);
	int a;
	int res = 0;
	if(i && str[i]==' ')
	{
		i+=1;
		a=status_code(str+i, addBranch(&sub_branches));
		if(!a) freeLast(&sub_branches);
		if(a && str[i+a]==' ')
		{
			i+=a+1;
			a=reason_phrase(str, addBranch(&sub_branches));
			if(!a) freeLast(&sub_branches);
			if(str[i+a]==13)
			{
				if(str[i+a+1]==10)
				{
					res = i+a+2;
				}
				else res = 0;
			}
			else res = 0;
		}
		else res = 0;
	}
	else res = 0;
	(*branch)->branches = sub_branches;
	(*branch)->data_size = res;
	#ifdef DEBUG
	printf("status_line returns %d\n", res);
	#endif /* DEBUG */
	return res;
}

int start_line(char *str, Branch** branch)
{
	int res = 0;

	*branch = new_branch(str, 0, "start_line");
	BranchList* sub_branches = new_branchlist();

    int a=request_line(str, addBranch(&sub_branches));
	if(!a) freeLast(&sub_branches);

	int b= status_line(str, addBranch(&sub_branches));
	if(!b) freeLast(&sub_branches);

	if(a) res = a;
	else if(b) res = b;
	else res = 0;

	(*branch)->branches = sub_branches;
	(*branch)->data_size = res;
	#ifdef DEBUG
	printf("start_line returns %d\n", res);
	#endif /* DEBUG */
	return res;
}


int Accept(char* str, Branch** branch)
{
	*branch = new_branch(str, 0, "Accept");
	BranchList* sub_branches = new_branchlist();

	int i=0;
	int a,b,c,d;
	a=media_range(str, addBranch(&sub_branches));
	if(!a) freeLast(&sub_branches);

	if(a||str[i]==',')
	{
		if(a) {
			i+=a;
			a=accept_params(str+i, addBranch(&sub_branches));
			if(!a) freeLast(&sub_branches);
			i+=a;
		}
		else i+=1;

		a=OWS(str+i, addBranch(&sub_branches));
		//if(!a) freeLast(&sub_branches);
		b=OWS(str+i+a+1, addBranch(&sub_branches));
		//if(!b) freeLast(&sub_branches);
		c=media_range(str+i+a+1+b, addBranch(&sub_branches));
		if(!c) freeLast(&sub_branches);
		d=accept_params(str+i+a+1+b+c, addBranch(&sub_branches));
		if(!d) freeLast(&sub_branches);
		while(c && str[i+a]==',')
		{
			i+=a+b+c+d+1;
			a=OWS(str+i, addBranch(&sub_branches));
			//if(!a) freeLast(&sub_branches);
			b=OWS(str+i+a+1, addBranch(&sub_branches));
			//if(!b) freeLast(&sub_branches);
			c=media_range(str+i+a+1+b, addBranch(&sub_branches));
			if(!c) freeLast(&sub_branches);
			d=accept_params(str+i+a+1+b+c, addBranch(&sub_branches));
			if(!d) freeLast(&sub_branches);
		}
	}
	(*branch)->branches = sub_branches;
	(*branch)->data_size = i;
	#ifdef DEBUG
	printf("Accept returns %d\n", i);
	#endif /* DEBUG */
	return i;
}


int Host_header(char* str, Branch** branch)
{

	*branch = new_branch(str, 0, "Host_header");
	BranchList* sub_branches = new_branchlist();

	int i=0;
	int a,b,c;
	if(strcomp(str,"Host:"))
	{
		i=5;
		a=OWS(str+i, addBranch(&sub_branches));
		//if(!a) freeLast(&sub_branches);
		b=Host(str+a+i, addBranch(&sub_branches));
		if(!b) freeLast(&sub_branches);
		c=OWS(str+i+a+b, addBranch(&sub_branches));

		if(b)
		{
			i+=a+b+c;
		}
		else i=0;
	}
	(*branch)->branches = sub_branches;
	(*branch)->data_size = i;
	#ifdef DEBUG
	printf("Host_header returns %d\n", i);
	#endif /* DEBUG */
	return i;
}

int Cookie_header(char* str, Branch** branch)
{
	*branch = new_branch(str, 0, "Cookie_header");
	BranchList* sub_branches = new_branchlist();

	int i=0;
	int a,b,c;
	if(strcomp(str,"Cookie:"))
	{
		i=7;

		a=OWS(str+i, addBranch(&sub_branches));

		b=cookie_string(str+a+i, addBranch(&sub_branches));
		if(!b) freeLast(&sub_branches);
		c=OWS(str+i+a+b, addBranch(&sub_branches));
		if(b)
		{
			i+=a+b+c;
		}
		else i=0;
	}
	(*branch)->branches = sub_branches;
	(*branch)->data_size = i;
	#ifdef DEBUG
	printf("Cookie_header returns %d\n", i);
	#endif /* DEBUG */
	return i;
}

int Transfer_Encoding_header(char* str, Branch** branch)
{
	*branch = new_branch(str, 0, "Transfer_Encoding_header");
	BranchList* sub_branches = new_branchlist();

	int i=0;
	int a,b,c;
	if(strcomp(str,"Transfer-Encoding:"))
	{
		i=7;
		a=(OWS(str+i, addBranch(&sub_branches)));

		b=Transfer_Encoding(str +a+i,addBranch(&sub_branches));
		if(!b) freeLast(&sub_branches);
		c=OWS(str+i+a+b, addBranch(&sub_branches));

		if(b)
		{
			i+=a+b+c;
		}
		else i=0;
	}

	(*branch)->branches = sub_branches;
	(*branch)->data_size = i;
	#ifdef DEBUG
	printf("Transfer_Encoding returns %d\n", i);
	#endif /* DEBUG */
	return i;
}


int alphanum(char* str, Branch** branch)
{
	*branch = new_branch(str, 0, "alphanum");
	BranchList* sub_branches = new_branchlist();
	int i,a,b;
	a=ALPHA(str,addBranch(&sub_branches));
	if(!a) freeLast(&sub_branches);
	b=DIGIT(str,addBranch(&sub_branches));
	if(!b) freeLast(&sub_branches);
	i=a+b;
	(*branch)->branches = sub_branches;
	(*branch)->data_size = i;
	#ifdef DEBUG
	printf("alphanum returns %d\n", i);
	#endif /* DEBUG */
	return i;

}


int language_range(char* str, Branch** branch)
{
	*branch = new_branch(str, 0, "language_range");
	BranchList* sub_branches = new_branchlist();

	int i=0;
	int a=ALPHA(str,addBranch(&sub_branches));
	if(!a) freeLast(&sub_branches);
	int compt=0;
	while(compt<8 && a)
	{
		compt++;
		a=ALPHA(str+compt,addBranch(&sub_branches));
		if(!a) freeLast(&sub_branches);
	}
	if(compt)
	{

		i+=compt;
		compt=0;
		a=alphanum(str+i+1,addBranch(&sub_branches));
		if(!a) freeLast(&sub_branches);
		while(compt<8 && a)
		{
			a=alphanum(str+compt+i+1,addBranch(&sub_branches));
			if(!a) freeLast(&sub_branches);
			compt++;
		}
		if(compt&& str[i]=='-')
		{
			while(compt && str[i]=='-')
			{
				i+=1+compt;
				compt=0;
		 		a=alphanum(str+i+1,addBranch(&sub_branches));
		 		if(!a) freeLast(&sub_branches);
		 		while(compt<8 && a)
		 		{
		 			compt++;
		 			a=alphanum(str+compt+i+1,addBranch(&sub_branches));
		 			if(!a) freeLast(&sub_branches);
		 		}
			}
		}
		else if(str[i]=='*')
		{
			i++;
		}
	}
	(*branch)->branches = sub_branches;
	(*branch)->data_size = i;
	#ifdef DEBUG
	printf("language_range returns %d\n", i);
	#endif /* DEBUG */
	return i;
}


int Accept_Language(char* str, Branch** branch)
{
	*branch = new_branch(str, 0, "Accept_Language");
	BranchList* sub_branches = new_branchlist();
	int i=0;
	int a,b,c,d;
	while(str[i]==',')
	{
		a=OWS(str+i+1,addBranch(&sub_branches));
		i+=a+1;
	}
	a=language_range(str+i,addBranch(&sub_branches));
	if(!a) freeLast(&sub_branches);
	b=weight(str+i+a,addBranch(&sub_branches));
	if(!b) freeLast(&sub_branches);
	if(a)
	{
		i+=a+b;
		a=OWS(str+i,addBranch(&sub_branches));
		b=OWS(str+i+a+1,addBranch(&sub_branches));
		c=language_range(str+i+a+1+b,addBranch(&sub_branches));
		if(!c) freeLast(&sub_branches);
		if(!c) freeLast(&sub_branches);
		d=weight(str+i+a+1+b+c,addBranch(&sub_branches));
		if(!d) freeLast(&sub_branches);
		while(str[a+i]==',' && c)
		{
			i+=a+1+b+c+d;
			a=OWS(str+i,addBranch(&sub_branches));
			b=OWS(str+i+a+1,addBranch(&sub_branches));
			c=language_range(str+i+a+1+b,addBranch(&sub_branches));
			if(!c) freeLast(&sub_branches);
			d=weight(str+i+a+1+b+c,addBranch(&sub_branches));
			if(!d) freeLast(&sub_branches);
		}
	}
	else i=0;
	(*branch)->branches = sub_branches;
	(*branch)->data_size = i;

	#ifdef DEBUG
	printf("Accept_Language returns %d\n", i);
	#endif /* DEBUG */
	return i;
}


int Accept_Language_Header(char* str, Branch** branch)
{
	*branch = new_branch(str, 0, "Accept_Language_Header");
	BranchList* sub_branches = new_branchlist();

	int i=0;
	int a,b,c;
	if(strcomp(str,"Accept-Language:"))
	{

		i=16;

		a=(OWS(str+i, addBranch(&sub_branches)));

		b=Accept_Language(str +a+i,addBranch(&sub_branches));
		if(!b) freeLast(&sub_branches);
		c=OWS(str+i+a+b, addBranch(&sub_branches));

		if(b)
		{
			i+=a+b+c;
		}
		else i=0;
	}

	(*branch)->branches = sub_branches;
	(*branch)->data_size = i;
	#ifdef DEBUG
	printf("Accept_Language_Header returns %d\n", i);
	#endif /* DEBUG */
	return i;
}

int obs_fold(char* str, Branch** branch)
{
	*branch = new_branch(str, 0, "obs_fold");
	int i=0;
	if(str[i]==13)
	{
		if(str[i+1]==10){
			if(str[i+2]==' ' || str[i+2]==9)
			{
				i=3;
				while(str[i]==' ' || str[i]==9)
				{
					i++;
				}
			}
		}
	}
	(*branch)->data_size = i;
	#ifdef DEBUG
	printf("obs_fold returns %d\n", i);
	#endif /* DEBUG */
	return i;
}

int field_vchar(char* str, Branch** branch)
{
	*branch = new_branch(str, 0, "field_vchar");
	BranchList* sub_branches = new_branchlist();

	int i=0;
	int a,b;
	a=vchar(str,addBranch(&sub_branches));
	if(!a) freeLast(&sub_branches);
	b=obs_text(str,addBranch(&sub_branches));
	if(!b) freeLast(&sub_branches);
	i=a+b;
	(*branch)->branches = sub_branches;
	(*branch)->data_size = i;
	#ifdef DEBUG
	printf("field_vchar returns %d\n", i);
	#endif /* DEBUG */
	return i;
}

int field_content(char* str, Branch** branch)
{
	*branch = new_branch(str, 0, "field_content");
	BranchList* sub_branches = new_branchlist();

	int i=field_vchar(str,addBranch(&sub_branches));
	int a,b;
	if(!i) freeLast(&sub_branches);
	if(str[i]==' ' ||  str[i]==9)
	{
		b=i;
		b++;
		while(str[b]==' ' || str[b]==9)
		{
			b++;
		}
		a=field_vchar(str,addBranch(&sub_branches));
		if(!a) freeLast(&sub_branches);
		if(a)
		{
			b+=a;
			i=b;
		}
	}
	(*branch)->branches = sub_branches;
	(*branch)->data_size = i;
	#ifdef DEBUG
	printf("field_content returns %d\n", i);
	#endif /* DEBUG */
	return i;
}



int field_value(char* str,Branch** branch)
{
	*branch = new_branch(str, 0, "field_value");
	BranchList* sub_branches = new_branchlist();
	int i=0;
	int a,b;
	a=field_content(str, addBranch(&sub_branches));
	if(!a) freeLast(&sub_branches);
	b=obs_fold(str, addBranch(&sub_branches));
	if(!b) freeLast(&sub_branches);
	while(a || b)
	{
		i+=a+b;
		a=field_content(str+i, addBranch(&sub_branches));
		if(!a) freeLast(&sub_branches);
		b=obs_fold(str+i, addBranch(&sub_branches));
		if(!b) freeLast(&sub_branches);
	}
	(*branch)->branches = sub_branches;
	(*branch)->data_size = i;
	#ifdef DEBUG
	printf("field_value returns %d\n", i);
	#endif /* DEBUG */
	return i;
}

int field_name(char* str,Branch** branch)
{
	*branch = new_branch(str, 0, "field_name");
	BranchList* sub_branches = new_branchlist();
	int i=token(str, addBranch(&sub_branches));
	if(!i) freeLast(&sub_branches);
	(*branch)->branches = sub_branches;
	(*branch)->data_size = i;
	#ifdef DEBUG
	printf("field_name returns %d\n", i);
	#endif /* DEBUG */
	return i;
}

int header_field(char* str, Branch** branch)
{
	*branch = new_branch(str, 0, "header_field");
	BranchList* sub_branches = new_branchlist();
	int a,b,c,d,e,f,g,h,i,j,k,l,m,n;
	a=Content_Lenght_Header(str, addBranch(&sub_branches));
	if(!a) freeLast(&sub_branches);
	b=Transfer_Encoding_header(str, addBranch(&sub_branches));
	if(!b) freeLast(&sub_branches);
	c=Cookie_header(str, addBranch(&sub_branches));
	if(!c) freeLast(&sub_branches);
	d=Host_header(str, addBranch(&sub_branches));
	if(!d) freeLast(&sub_branches);
	e=Referer_header(str, addBranch(&sub_branches));
	if(!e) freeLast(&sub_branches);
	f=User_Agent_header(str, addBranch(&sub_branches));

	if(!f) freeLast(&sub_branches);
	g=Accept_header(str, addBranch(&sub_branches));

	if(!g) freeLast(&sub_branches);
	h=Accept_Encoding_header(str, addBranch(&sub_branches));
	if(!h) freeLast(&sub_branches);
	i=Connection_header(str, addBranch(&sub_branches));
	if(!i) freeLast(&sub_branches);
	j=Accept_Language_Header(str, addBranch(&sub_branches));
	if(!j) freeLast(&sub_branches);

	if(a+b+c+d+e+f+g+h+i+j==0){
	k=field_name(str, addBranch(&sub_branches));
	if(!k) freeLast(&sub_branches);
		if(k && str[k]==':')
		{
			l=OWS(str+k+1, addBranch(&sub_branches));
			m=field_value(str+k+1+l, addBranch(&sub_branches));
			n=OWS(str+k+1+m+l, addBranch(&sub_branches));
			i=l+m+k+1+n;
		}
	}
	(*branch)->branches = sub_branches;
	(*branch)->data_size = a+b+c+d+e+f+g+h+i+j;
	#ifdef DEBUG
	printf("header_field returns %d\n", a+b+c+d+e+f+g+h+i+j);
	#endif /* DEBUG */
	return a+b+c+d+e+f+g+h+i+j;

}

int message_body(char* str, Branch** branch)
{
	*branch = new_branch(str, 0, "message_body");
	BranchList* sub_branches = new_branchlist();

	int i=0;
	while(str[i]!='\0')
	{
		i+=1;
	}
	(*branch)->branches = sub_branches;
	(*branch)->data_size = i;
	#ifdef DEBUG
	printf("message_body returns %d\n", i);
	#endif /* DEBUG */
	return i;
}

int HTTP_message(char* str, Branch** branch)
{
	*branch = new_branch(str, 0, "HTTP_message");
	BranchList* sub_branches = new_branchlist();
	int i=start_line(str, addBranch(&sub_branches));
	if(!i) freeLast(&sub_branches);
	int a;
	if(i)
	{
		a=header_field(str+i, addBranch(&sub_branches));
		if(!a) freeLast(&sub_branches);
		while((str[i+a]!=13 || str[i+a+1]!=10 || str[i+a+2]!=13 || str[i+a+3]!=10)&& str[i+a]!='\0')
		{
			i+=a+2;
			a=header_field(str+i, addBranch(&sub_branches));
			if(!a) {
				freeLast(&sub_branches);
			}
		}


		if(str[i]==13 )
		{
			i++;
			if(str[i]==10)
			{
				i++;
				a=message_body(str, addBranch(&sub_branches));
				if(!a) freeLast(&sub_branches);
				i+=a;
			}
		}
	}
	(*branch)->branches = sub_branches;
	(*branch)->data_size = i;

	#ifdef DEBUG
	printf("HTTP_message returns %d\n", i);
	#endif /* DEBUG */


	return i;
}
