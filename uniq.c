#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"

char buf[51200];
char *args;

char toUpper(char s)
{
    if(s >= 'a' && s <= 'z') {
         s = s -32;
    }
    return s;
}

int stringCmpi (char *s1, char *s2)
{
    int i=0;
    for(i=0; s1[i]!='\0'; i++)
    {
        if( toUpper(s1[i])!=toUpper(s2[i]) )
            return 1;
    }
    return 0;
}

void
uniq(int fd, char *name)
{
	int final=0,k,p,q,index=0,m=0,l=0,b,n,i,count=0,count1 = 0; char pilot[100];
    char *output[30];
	int repeat[100];
    int ignoreCase = 0;
	if(strcmp(args, "-i")==0)
        ignoreCase = 1;

    while((n = read(fd, buf, sizeof(buf))) > 0)
    {        
        for(b=0; b<n; b++)
		{
            if(buf[b] == '\n')
            l++;
        }
        for(i=0;buf[i]!='\n';i++)
        {
            count++;
            pilot[i]=buf[i];
        }
           
    	pilot[i]='\0';
		repeat[0]=1;
        output[0]=(char*)malloc((count+1)*sizeof(char*));
        for(i=0;i<count+1;i++)
        {
            output[index][i]=pilot[i];
        }
		output[index][i]='\0';
        k=i;
        while(final<l-1)
	    {
		    final++;
        	count1=0;
		    for(i=k;buf[i]!='\n';i++)
        	{
			    count1++;
        		pilot[m++]=buf[i];
        	}
            pilot[m]='\0';
            k=k+count1+1;
            m=0;
            if(ignoreCase)
            {
                if(stringCmpi(output[index],pilot)!=0)
                {
                    index = index + 1;
                    output[index]=(char*)malloc((count1+1)*sizeof(char*));
                    for(p=0;p<count1;p++)
                    {
                        output[index][p]=pilot[p];
                    }
                    output[index][p]='\0';
                    repeat[index]=1;
                }
                else
                {
                   repeat[index]=repeat[index]+1;
                }
            }
            else
            {
                if(strcmp(output[index],pilot)!=0)
                {
                    index = index + 1;
                    output[index]=(char*)malloc((count1+1)*sizeof(char*));
                    for(p=0;p<count1;p++)
                    {
                        output[index][p]=pilot[p];
                    }
                    output[index][p]='\0';
                    repeat[index]=1;
                }
                else
                {
                   repeat[index]=repeat[index]+1;
                }
            }
	    }
    }
    if(strcmp(args,"-c")==0 || strcmp(args,"-C")==0 )
	{
		for(q=0;q<index+1;q++)
        { 
            printf(1,"%d %s \n",repeat[q],output[q]);
        }
    }
    else  if(strcmp(args,"-d")==0 || strcmp(args,"-D")==0 )
	{
	    for(q=0;q<index+1;q++)
        {
            if(repeat[q]>1)
            {	
                printf(1,"%s \n",output[q]);
                repeat[q] = 0;
            }
	    }	
    }
	else
	{
	    for(q=0;q<index+1;q++)
        {
            printf(1,"%s \n",output[q]);
        }

	}
}


int main(int argc, char **argv)
{
  int fd, r;
  if(argc <= 1)
  {
    uniq(0, "");
    exit();
  }
  else if(argc == 2)
  {
  for(r = 1; r < argc; r++)
  {
    if((fd = open(argv[r], 0)) < 0)
    {
      printf(1,"uniq: cannot open %s\n", argv[r]);
      exit();
    }
    uniq(fd, argv[r]);
    close(fd);
  }
  exit();
  }
  else
  {
    for(r = 2; r < argc; r++)
  {
    if((fd = open(argv[r], 0)) < 0)
    {
      printf(1,"uniq: cannot open %s\n", argv[r]);
      exit();
    }
    args=argv[1];
    uniq(fd, argv[r]);
    close(fd);
  }
  exit();
  }
} 
