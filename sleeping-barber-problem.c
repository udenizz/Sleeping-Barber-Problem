#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include<string.h>
#include<semaphore.h>
#include <stdbool.h>


//implementation first for shared data

sem_t semCust;
sem_t semBarber;
sem_t semCustDone;
sem_t semBarberDone;

sem_t lockUDZ;

int chair=5;
int inShopCustomer=0;
int cuttingtime=0;

void* hairCutting()
{
	sleep(cuttingtime);
}

void* timeToCutting()
{
	sleep(cuttingtime);
}

void * customerFunction(void *customer)//customer function
{
	int numofCust=*(int *)customer;//intialize customer number
	
	sem_wait(&lockUDZ);
	
	if(inShopCustomer==chair)//checking available chair for customer
	{
		printf("Customer %d cant come in all chairs are full!!!\n",numofCust);
		sem_post(&lockUDZ);//available mutex
		while(true)
		{
		
		}
	}
	else
	{
		inShopCustomer++;
	}
	
	printf("Customer %d comes and sitting charis. Available chairs number:%d\n",numofCust,(chair - inShopCustomer));
	sem_post(&lockUDZ);//availabla mutex
	
	sem_post(&semCust);//available customer
	sem_wait(&semBarber);//locking barber
	printf("Customer number %d haircutting now!!\n",numofCust);
	cuttingtime=rand()%8-1;//this random provide that cutting hair time may be more than customer arrival time 
	hairCutting();//calling barber cutting function
	sem_post(&semCustDone);//customer done by barber
	sem_wait(&semBarberDone);//wait till barber done jobe
	
	sem_wait(&lockUDZ);//barber available after these condition now wait till mutexlock
	inShopCustomer--;
	sem_post(&lockUDZ);//release mutex
	
}

void* BarberFunction()
{
	while(true)
	{
		sem_wait(&semCust);//wait till customer come
		sem_post(&semBarber);//available barber
		printf("Barber cutting customer hair now!!\n");
		timeToCutting();//barber cutting here
		printf("Barber finished cutting customer hair now!!\n");
		sem_wait(&semCustDone);//customer done waiting 
		sem_post(&semBarberDone);//available barber done 
	}
}

int main()
{
	int expectCust,i;
	
	printf("Enter number of total customer barber expect daily:");
	scanf("%d" ,&expectCust);
	
	pthread_t barberid;
	int customerArr[expectCust];
	pthread_t customerid[expectCust];
	int j=1;
	for(i=0;i<expectCust;i++)
	{
		customerArr[i]=j;
		j++;
	}
	
	sem_init(&semCust,0,0);//not shared with any thread, not initiliazied any value
	sem_init(&lockUDZ,0,1);//not shared with any thread or process,not iniliazied any value
	sem_init(&semBarber,0,0);//not shared with any thread, not initiliazied any data
	sem_init(&semCustDone,0,0);//not shared with any thread, not initiliazied any data
	sem_init(&semBarberDone,0,0);//not shared any thread, not initiliazied any data

	pthread_create(&barberid,NULL,BarberFunction,NULL);//Creation barber thread
	int arrivaltime=rand()%4-1;//this random time needed for not create customer same time
	for(i=0;i<expectCust;i++)
	{
		pthread_create(&customerid[i],NULL,customerFunction,(void*)&customerArr[i]);//creation of customer 
	
		sleep(arrivaltime);
	}
	
	for(i=0;i<expectCust;i++)
	{
		pthread_join(customerid[i],NULL);
	}
	
	pthread_join(barberid,NULL);
}





