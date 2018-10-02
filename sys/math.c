#include <stdio.h>
#include <math.h>

double pow(double x, int y){
	int i;
	double pow=1;
	if(y==0)
	pow=1;
	//printf("x: %2f\n",x);
	for(i=1;i<=y;i++){
		pow=pow*x;
		//printf("multi: %f\n", pow);
	}
	//printf("Power: %d\n",(int)pow);
	return pow;
}

double logx(double x){

	//Using the logic of the tailer series for log x
	//(x-1)-1/2(x-1)^2+1/3(x-1)^3 .....
	
	double log_val=0;
	int i;
	for(i=0;i<20;i++){
		log_val+=pow(-1,i)*pow((x-1),(i+1))/(i+1);
		//printf("Log_val: %d\n",(int)log_val);
	}
	//printf("Log value : %d\n",(int)log_val);
	return log_val;
}

double expdev(double lambda) {
	    double dummy;
		do{
	        dummy = (double)rand() / RAND_MAX;
	        //dummy = (double)((randx1 = randx1*1103515245 + 12345) & 077777);
		}while(dummy == 0.0);
		//printf("Dummy: %d\n",dummy);
		//while(dummy == 0.0);
		   // double l = -logx(0.2);
			//printf("l1: %u",l);
			//l= l*10;
			//printf("l2: %d",l);
		    //return l;
		    double val = -logx(dummy) / lambda;
		    //printf("val: %d\n",(int)val);
		    return val;
}
