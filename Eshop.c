#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/stat.h>
#include <errno.h>

#define BUFFER_SIZE 256
#define EURO 300.0f

struct product
{	
	int description;
	float price;
	int item_count;
 	int request_count;
	int sold_count;
	int customerNotSupportedCtr[5];
};
	
int main()    
{
	struct product catalog[20];
	pid_t pid,waitpid;
	srand(time(NULL));
	time_t t;
	int i,j,k;
	int status;
	int fd[2];
	int fd1[2];
	int retVal, retVal1;
	int selected[10];
	int ordersCtr=0, orderSuccessfulCtr=0, orderUnsuccessfulCtr=0;
	float totalTurnover=0.0f;
	int buf;   
    int buf1; 
	int order_result;
	
	printf("----------------CATALOG------------\n");
	printf("Product Description: \t Product Availability: \t Product Price: \t \n");
	for(i=0;i<20;i++)
	{
		catalog[i].item_count=2;
		catalog[i].description=i;
		catalog[i].price=((float)rand()/RAND_MAX)*(float)(101);   
		printf("%d\t\t\t %d\t\t\t %.2f \n",catalog[i].description,catalog[i].item_count,catalog[i].price);
		catalog[i].request_count=0;
		catalog[i].sold_count=0;
	}
	
	for (i=0;i<5;i++)
    {
		retVal = pipe(fd);
		retVal1 = pipe(fd1);
	
		if(retVal==-1)
        {
        	perror("PIPE #1 ERROR.");
       		exit(1);
        }

		if(retVal1==-1)
        {
        	perror("PIPE #2 ERROR.");
       		exit(1);
        }
    	
       	pid=fork();

		if(pid<0)
		{
			perror("fork error");
		}

        if(pid==0)   //CHILD
        {
			srand(time(NULL));
			printf("\n-----------Customer %d----------\n",i+1);
          	for (j=0; j<10; j++)
         	{
         		close (fd[0]);
         		printf("Order Number:%d \n",j+1); 
				selected[j] = (rand() % 20);
				printf("Product Selected By Customer: %d\n",selected[j]);
				
				write(fd[1],&selected[j],sizeof(int));
				sleep(1);
				close(fd1[1]);
				read(fd1[0],&buf1,sizeof(int));
		
				if(buf1==1)
				{
					printf("Product Available.\n");
				}
				else
				{
					printf("Product Unavailable.\n"); 
				}
				printf("%d\n",buf1);
         	}	
			 exit(1);  
        } 
    	else     //PARENT
		{
			int p;
			for (p=0;p<10;p++)
			{
				close(fd[1]);
				read(fd[0],&buf,sizeof(int));  
				printf("%d\n",buf); 

				if (catalog[buf].item_count>0)
				{
					order_result=1;
					catalog[buf].item_count=catalog[buf].item_count-1;
					totalTurnover+=catalog[buf].price;
					catalog[buf].sold_count++;
					orderSuccessfulCtr++;
					catalog[buf].customerNotSupportedCtr[j]=0;    
					
				}
				
				else
				{
					order_result=0;
					orderUnsuccessfulCtr++;
					catalog[buf].customerNotSupportedCtr[j]++;      
        		}
					    

				catalog[buf].request_count++;
				write(fd1[1],&order_result,sizeof(int));
				sleep(1);
			}
       	
       		waitpid = wait(NULL);
        	if(waitpid == -1)
        	{
            	perror("Error");
            	exit(1);
        	}
			sleep(1);
    	}
	}	
	ordersCtr=orderSuccessfulCtr + orderUnsuccessfulCtr;

	printf("-------------------INFORMATION:--------------------\n");
	for(i=0;i<20;i++)
	{
		printf("\nProduct Description:%d\n",catalog[i].description);
		printf("Requested %d Times.\n",catalog[i].request_count);
		printf("Sold Products %d\n",catalog[i].sold_count);

		if(catalog[i].sold_count>=2)
		{
			printf("\nSold Out\n");
		}
		
		for(j=0; j<5; j++)
		{
			if(catalog[i].customerNotSupportedCtr[j]>0 && catalog[i].customerNotSupportedCtr[j]<5)
			{
				//printf("\nWASN'T FOUND BY CUSTOMER: %d\n",j+1);
			}
			else if(catalog[i].customerNotSupportedCtr[j]>=5)  
			{
				printf("\nwasn't found by customer: %d\n",j+1);   
				break;
			}	
		}
	}
	printf("\nTOTAL ORDERS: %d\n", ordersCtr);
	printf("TOTAL SUCCESSFUL ORDERS: %d\n", orderSuccessfulCtr);
	printf("TOTAL UNSUCCESSFUL ORDERS: %d\n", orderUnsuccessfulCtr);
	printf("TOTAL TURNOVER: %.2f\n", totalTurnover);
}