void main2();
void main()
{
	main2();
}
 struct process
{
	int active;
	int sp;
	
}p[8];
int currentProcess;
void readFile (char* filename, char outbuf[] );
void writeFile (char* filename, char inbuf[] );
void directory();
void deleteFile(char* filename);
void readSector(char* buffer, int sector);
void writeSector(char* buffer, int sector);
void readString(char buffer[]);
void printString(char* str);
void handleInterrupt21(int ax, int bx, int cx, int dx);
void executeProgram(char* name) ;
void terminate();
void printChar(char ch);
void printInteger(int X);
char* getString(int X);
int getReverse(int X);
void handleTimerInterrupt(int segment, int sp);
void kill(int p);

void main2() {
	int i;
	char buffer[13312];
	char shell[6];
  	shell[0]='s';
 	shell[1]='h';
  	shell[2]='e';
  	shell[3]='l';
  	shell[4]='l';
  	shell[5]=0;
	setKernelDataSegment() ;
	for(i=0;i<8;i++);
	{
		p[i].active=0;
		p[i].sp=0x11;
	}
	if(p[0].sp==0x11)
		printString("sp==0xff00");
	currentProcess=0;
	restoreDataSegment();
	makeInterrupt21();
//	makeTimerInterrupt() ;
	interrupt(0x21, 9,shell,0, 0);
	while (1); 
}


void handleTimerInterrupt(int segment, int sp)
{
	
	int i,process,count=1;
	setKernelDataSegment();
	if(segment==0x1000)
	{	printString("segment=1000");
		currentProcess=-1;
	}
	else
	{
		p[currentProcess].sp=sp;
	}
	process=currentProcess+1;
		
	if (segment == 0x1000)
	{
	 	putInMemory(0xB000, 0x8162, 'K');
	 	putInMemory(0xB000, 0x8163, 0x7);
	}
	else if (segment == 0x2000)
	 {
		putInMemory(0xB000, 0x8164, '0');
		putInMemory(0xB000, 0x8165, 0x7);
	 }
	else if (segment == 0x3000)
	 {
		putInMemory(0xB000, 0x8166, '1');
	 	putInMemory(0xB000, 0x8167, 0x7);
	 }
	else
	 {
		putInMemory(0xB000, 0x8160, 'X');
		putInMemory(0xB000, 0x8161, 0x7); 
       	 }
	
	while(process<8)
	{
		
		if(p[process].active==1)
		{
			printString("process active");
			currentProcess=process;
			segment=process+2;
			segment=segment*0x1000;
			if( p[process].sp == 0xff00 )
				printString("segment=0x2000");
			restoreDataSegment();
			returnFromTimer(segment,p[process].sp);
		}	
		if(count==8)
		{
			printString("breaking");
			break;
		}	
		if(process==7)
		{
			printString("process==7");
			process=0;
		}
		else
		{
			process++;
		}
			count++;
			
	}

//	printString("Tic");
	restoreDataSegment();
        returnFromTimer(segment,sp);
}

void executeProgram(char* name) 
{
	char buffer[13312];
	int i=0,process,seg;
	setKernelDataSegment() ;
	readFile(name,buffer);	
	process=0;
	while(process<8)
	{
		if(p[process].active==0)
		{
			p[process].active=1;
			seg=process+2;
			seg=seg * 4096;
			currentProcess=process;
			break;
		}
		process++;
	}
	  
	while(i < 13312)
	{
		putInMemory(seg,i,buffer[i]);
		i++;
	}
	printString("hi");
	initializeProgram(seg);
	printString("process initialised");
	restoreDataSegment();
}

void terminate()
{
	setKernelDataSegment() ;
	p[currentProcess].active=0;
	p[currentProcess].sp=0xff00;
	restoreDataSegment();
	while(1);        
}

void kill(int process)
{
	setKernelDataSegment();
	p[process].active=0;
	printInteger(p[process].active);
	restoreDataSegment();
}

void deleteFile(char* filename)
{
        int m,i,j,start;
        char map[512];
        char dir[512];
        readSector(map,1);
        readSector(dir,2);

        while(i<16)
        {
                j=0;
                while(j<6)
                {
                        if( dir[start+j]!=filename[j])
                        {       
                                break;
                        }
                        j++;

                }
                if(j==6)
                {

                        while(j<32)
                        {
                                m= dir[start+j] ;
                                map[m]= 0x00;
                                j++;
                        }
                        for(j=0;j<32;j++)
                        {
                                dir[start+j]=0x00;
                        }
                        break;

                }
                start=start+32;
                i++;
        }
        writeSector(dir,2);
        writeSector(map,1);
       
}


void readFile(char* filename, char buffer[])
{	
	char temp[512];
	char dir[512];
	int i ,k,l,j,start,m ;
	char LineFeed=0xa;
	i=0;
	j=0;
	start=0;
	readSector(dir,2);
	while(i<16)
	{
		j=0;
		while(j<6)
		{
			if( dir[start+j]!=filename[j])
			{	
				break;
			}
			j++;

		}
		if(j==6)
		{

			l=0;
			while(j<32)
			{
				m=dir[start+j];
				readSector(temp,m);
				for(k=0;k<512;k++)
				{
					buffer[k+l]=temp[k];

				}
				l+=512;
				j++;
			}
			break;

		}
		start=start+32;
		i++;
	}
}


void writeFile(char* filename, char buffer[])
{
	char temp[512];
	char dir[512];
	char map[512];
	int i ,k,j,m,l,p,start;
	char null=0x00;
	char LineFeed=0xa;
	i=0;
	j=0;
	start=0;
	interrupt(0x21,2,dir,2,0);
	while(i<16)
	{

		j=0;
		if( dir[start+j] == null)
		{
			while(j<6)
			{
				dir[start+j]=filename[j];
				j++;
			}
			p=0;
			l=0;
			while(j<32)
			{

				while(map[p]!=null)
				{ p++ ;}
				dir[start+j]=map[p];
				m=dir[start+j];
				for(k=0;k<512;k++)
				{
					temp[k]=buffer[k+l];
				}
				writeSector(temp,m);
				j++;
				l+=512;
			}
			break;
		}


		start=start+32;
		i++;
	}
	writeSector(dir,2);
	writeSector(map,1);

}

void directory()
{
	char dir[512];
	int i ,j,start;
	int ax,ah,al;
	char LineFeed=0xa;
	char  Cr=0xd;
	i=0;
	j=0;
	start=0;
	interrupt(0x21,2,dir,2,0);

	while(i<16)
	{

		j=0;
		while(j<6)
		{
			ah=0xe;
			al=dir[start+j];
			ax=ah*256 + al ;
			interrupt(0x10,ax, 0, 0, 0);
			j++;
		}
		interrupt(0x10,ah*256 + ' ', 0, 0, 0);
		start=start+32;
		i++;
	}
	interrupt(0x10,ah*256 + LineFeed, 0, 0, 0);
	interrupt(0x10,ah*256 + Cr, 0, 0, 0);

}

void handleInterrupt21(int ax, int bx, int cx, int dx)
{
	
	if(ax==0)
	{
		printString(bx); 
	} 
	else if(ax==1) {
		readString(bx);
	} 
	else if(ax==2) {
		readSector(bx,cx); 
	}
	else if(ax==3)
	{
		directory();
	}
	else if(ax==4)
	{
		deleteFile(bx);
	}
	else if(ax==5)
	{
		terminate();
	} 
	else if(ax==6)
	{
		readFile(bx,cx);
	}
	else if(ax==7)
	{
		writeSector(bx,cx);
	}
	else if(ax==8)
	{
		writeFile(bx,cx);
	}
	else if(ax==9)
	{
		executeProgram(bx);
	}
	else if(ax==10)
        {
                kill(bx);
        }

	else
	{
		printString("Invalid use of interrupt 21");
	}

}

void readString(char buffer[])
{

	int i=0;
	char enter = 0xd; 
	char LineFeed = 0xa ;
	char nullChar = 0x0 ; 
	char back = 0x8 ;
	char ah = 0xe ;
	char al=interrupt(0x16, 0, 0, 0, 0);
	int ax=ah * 256 + al;
	interrupt(0x10, ax, 0, 0, 0);  

	for(i=0 ;al!= enter && al!=32; )
	{
		if(al == back && i>0)
		{
			i--;    
		}
		else if(al == back)
		{
			i=0;
		}
		else
		{
			buffer[i]=al;   
			i++;
		}


		al = interrupt(0x16, 0, 0, 0, 0);
		ax = ah * 256 + al ;
		interrupt(0x10, ax, 0, 0, 0);  

	}
	buffer[i]=LineFeed;
	buffer[i+1]=nullChar;
	if(al==enter)
	{
		interrupt(0x10,ah*256+LineFeed,0,0,0);

	}
}

void printString(char* chars){ 


	int i=0; 
	char al;
	char ah;
	int ax;
	for(i=0;chars[i]!='\0';i++){ 
		al = chars[i];
		ah = 0xe;
		ax = ah * 256 + al;
		interrupt(0x10, ax, 0, 0, 0);
	}
	
}


void writeSector(char* buffer, int sector){ 


	int ah = 3; 
	int al = 1; 
	int ax = ah * 256 + al; 
	int bx = buffer;  
	int ch = div(sector,36); 
	int cl = mod(sector,18)+1;  
	int cx = ch * 256 + cl;
	int dh = mod(div(sector,18),2); 
	int dl = 0; 
	int dx = dh * 256 + dl; 
	setKernelDataSegment() ;
	interrupt(0x13, ax, bx, cx, dx);
	restoreDataSegment();
}
void readSector(char* buffer, int sector){ 

	int ah = 2; 
	int al = 1; 
	int ax = ah * 256 + al; 
	int bx = buffer;  
	int ch = div(sector,36); 
	int cl = mod(sector,18)+1;  
	int cx = ch * 256 + cl;
	int dh = mod(div(sector,18),2); 
	int dl = 0; 
	int dx = dh * 256 + dl; 
	interrupt(0x13, ax, bx, cx, dx);
}
int div(int a, int b)
{ 

	int q = 0; 
	while (a>=b){
		q++;
		a=a-b;
	}
	return q; 
} 

int mod(int a, int b){
	while(a>=b)
		a=a-b;
	return a; 
} 


void printChar(char ch){
	char al=ch;
  	char ah=0xe;
 	int ax=ah*256+al;
  	interrupt(0x10,ax,0,0,0);
}
void printInteger(int X){
  	int Y=getReverse(X);
  	while(Y>0)
	{
  	printChar(48+mod(Y,10));
  	Y=div(Y,10);
 	 }
}
int getReverse(int X)
{
	int num=0;
  	while(X>0){
  		num=num*10+mod(X,10);
  		X=div(X,10);
  	}
  	return num;
}
