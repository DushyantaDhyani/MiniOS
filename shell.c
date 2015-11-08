int strcmp(char* string1,char* string2);
void main()
{
char buffer[80];
char buffer1[80];
enableInterrupts();
interrupt(0x21,0,"SHELL>",0,0);
interrupt(0x21,1,buffer,0,0);
interrupt(0x21,1,buffer1,0,0);

if(!strcmp(buffer,"dir"))
	{
		
		interrupt(0x21,3,0,0,0);
	}
else if(!strcmp(buffer,"execute"))
{
	interrupt(0x21,9,buffer1,0,0);
}
else if(!strcmp(buffer,"type"))
{
	char bufferb[13312];
	interrupt(0x21, 6,buffer1, bufferb, 0); 
	 interrupt(0x21, 0, bufferb, 0, 0);
}
else if(!strcmp(buffer,"create"))
{
        char bufferb[13312];
        interrupt(0x21,1,bufferb,0, 0); 
         interrupt(0x21,8,buffer1,bufferb,0);
}
else if(!strcmp(buffer,"del"))
{
	interrupt(0x21,4,buffer1,0,0);
}
else if(!strcmp(buffer,"kill"))
{
       interrupt(0x21,10,buffer1,0,0);
}
	
else 
{
	interrupt(0x21,0,"bad command",0,0);
}
	
interrupt(0x21,5,0,0,0);

}

int strcmp(char* string1,char* string2)
{
	int i=0;
	char null=0x00;
	while(string1[i]!=null && string2[i]!=null)
	{
		if(string1[i]!=string2[i])
			{
				return 1;
			}
		else
		{
			i++;
		}
	}
  return 0;
}

