
#include "comdev.h"


/***********************************************************
Function Name:     ComOpen
Brief:             open Serail Port  
Parameters:        port (char *) string of the port
Return:            int
*************************************************************/
int ComOpen(const char *port)
{
   int fd; 
   
   fd = open(port, O_RDWR|O_NOCTTY); //|O_NDELAY
   if(fd==-1){
      //perror("ArmCom:   Can't Open Com Port");
      return(-1);
   }
	
   return fd;
}

/****************************************************************
Function Name:	   ComSetBaud
Brief:	           Set Baudrate of Serial Port 
Parameters:        fd (int) handler of the file opened
Parameters:        baud (int) baudrate of setting
Return:            void
******************************************************************/
int baud_attr[]={B115200, B38400, B19200, B9600, B4800, B2400, B1200, B300};
int name_attr[]={115200, 38400, 19200, 9600, 4800, 2400, 1200, 300};
int ComSetBaud(int fd, int baud)
{
   int    i, j;
   int    status;
   struct termios Opt;
   
   tcgetattr(fd, &Opt);
   
   for(j=0; j<2; j++){
      for(i=0; i<sizeof(baud_attr)/sizeof(int); i++){
         if(baud==name_attr[i]){
				    tcflush(fd, TCIOFLUSH);
				    cfsetispeed(&Opt, baud_attr[i]);
				    cfsetospeed(&Opt, baud_attr[i]);
				    status = tcsetattr(fd, TCSANOW, &Opt);
				    if(status != 0){
					     perror("ArmCom:   tcsetattr fail");
					     return(-1);
				    }
				    tcflush(fd, TCIOFLUSH);				
			   }
		  }
	 }
	 return (0);	
}

/***************************************************************
function Name:     ComSetParity
Brief:             Set parity of Serial Port
Parameters:        fd(int) databits(int) stopbits(int) parity(int)
Return:            int 
****************************************************************/
int ComSetParity(int fd, int databits, int stopbits, int parity)
{
   struct termios options;
   
   if(tcgetattr(fd, &options) != 0){
      perror("ArmCom:   tcgetattr fail");
		  return(-1);
   }
   
	 options.c_cflag &= ~CSIZE;
	 
	 switch(databits){
      case 7:
			   options.c_cflag |= CS7;
			   break;
      case 8:
      	 options.c_cflag |= CS8;
         break;
      default:
         fprintf(stderr, "ArmCom:   Unsupported data size"); 
         return(-1);
   }
   
   switch(parity){
      case 'n':
      case 'N':
         options.c_cflag &= ~PARENB;                       //���Ч��
         options.c_iflag &= ~INPCK;                        //Clear parity checking
         break;
      case 'o':
      case 'O':
         options.c_cflag |= (PARODD | PARENB);
         options.c_iflag |= INPCK;
         break;
		  case 'e':
		  case 'E':
         options.c_cflag |= PARENB;
         options.c_cflag &= ~PARODD;
         options.c_iflag |= INPCK;
         break;
		  case 's':
		  case 'S':
         options.c_cflag &= ~PARENB;
         options.c_cflag &= ~CSTOPB;
			   break;
		  default:
			   fprintf(stderr, "ArmCom:   Unsupported parity");
			   return(-1);
	 }
	 
	 switch(stopbits){
		  case 1:
			   options.c_cflag &= ~CSTOPB;
			   break;
		  case 2:
			   options.c_cflag |= CSTOPB;
		     break;
		  default:
			   fprintf(stderr, "ArmCom:   Unsupported stop bits");
			   return(-1);
	 }
	 
	 //Set Input parity option
	 if(parity=='n' || parity=='N') options.c_iflag =0;
		
   //if tht port is not for moden, then set it as raw model
	 options.c_lflag &= ~(ICANON | ECHO | ECHOE |ISIG);      //Input/
	 options.c_oflag = 0;                                    //Clear All (Expecial: XOFF/XON)
	
	 tcflush(fd, TCIFLUSH);
	 options.c_cc[VTIME] = 150;                                //set timeout of 15 seconds
	 options.c_cc[VMIN]  = 0;     
	 
	 //Update the option and do it NOW*/
	 if(tcsetattr(fd, TCSANOW, &options) != 0){
		  perror("ArmCom:   Setup Serial Port");
		  return (-1);	
	 }
	 return(0);
}

/*****************************************************
Function Name:     ComInit
Brief:             initial com n (n: 0-4)
Parameters:        com (from 0-4)
Return:            fd(return the file number), -1(initial fail) 
******************************************************/
int ComInit(const char * dev, int baudrate,int databits,int stopbits,int parity) 
{
   int  fd;
//    char dev[] = "/dev/ttyS0";

   //if((com>4)||(com<0))	return(-1);
//    dev[9] += com; 
   
   fd = ComOpen(dev);	
   if(fd==-1)
   {
      printf("ArmCom: Fail to Open COM(%s)\n", dev);
      return(-1);
   }
   else
   {
      	ComSetBaud(fd, baudrate);	
	   	if(ComSetParity(fd,databits,stopbits,parity)==-1)
		{	
			   printf("ArmCom: Set COM(%s) Parity Error\n", dev);
			   exit(0);
		}
   		printf("Armcom: Open COM(%s), Baudrate=%d\n", dev, baudrate);
	   	return fd;
	 }
}

/*********************************************
Function Name:     ComClose
Brief:             close the com
Parameters:        fd(file number)
Return:            none
**********************************************/
void ComClose(int fd)
{
   close(fd);	
}

/********************************************************************
Function Name:     ComReceive
Brief:             read data from com
Parameter:         fd(the number of the com), *rbuf(the buffer to store the data)
Return:            -1(overflow), sum(the number of receive bytes)
*********************************************************************/
int ComReceive(int fd, char *rbuf, int flag)
{
   int r, sum = 0;
   char *tbuf = rbuf;
   if(flag == 1)
   {
   	  	while((r=read(fd, rbuf, (512-sum)))>0)
	  	{
		  sum += r;
		  rbuf[r]=0;
		  if(strstr(tbuf, "\x0A\x0D")!=NULL) /*Enter to return*/
		  {
			  break;
		  }
		  rbuf += r;
		  if(sum >= 512 - 1){
			   perror("ArmCom:Receive buffer overflow!");
			   return -1;
          }
   		}
   		return sum;
   }
   else
   {
	   return read(fd, rbuf, 512);
   }
}

/********************************************************************
Function Name:     ComSend
Brief:             send data to com
Parameter:         fd(the number of the com), *wbuf(the buffer to store the data)
Return:            w(the number of sent bytes)
*********************************************************************/
int ComSend(int fd, const char *wbuf)
{
   int w = 0;
   
   w = write(fd, wbuf, strlen(wbuf));
   
   return(w);
}


