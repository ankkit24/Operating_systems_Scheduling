/* resched.c  -  resched */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <math.h>
#include <sched.h>

unsigned long currSP;	/* REAL sp of current process */
extern int ctxsw(int, int, int, int);
int stype=0;a=0;b=0;c=0;
int epticks=0;
int epoch=0,counter=0,flag=0;
//extern int getnextproc(int);:
//extern int getproc(int);
/*-----------------------------------------------------------------------
 * resched  --  reschedule processor to highest priority ready process
 *
 * Notes:	Upon entry, currpid gives current process id.
 *		Proctab[currpid].pstate gives correct NEXT state for
 *			current process if other than PRREADY.
 *------------------------------------------------------------------------
 */

//setter to select the schedular type
void setschedclass(int type){
	stype = type;   
}

//getter to select the schedular type
int getschedclass(){
	return stype;
}

//procedure to get the next process for exponential type schedular
int getnextproc(int random_priority){
	int this_process, round_r;

	if(isempty(rdyhead))
		return NULLPROC;

	this_process = q[rdyhead].qnext;
	//printf("head prio: %d\n",q[q[rdyhead].qnext].qkey);
	
	//comparing the random nuumber generated with the queue priorities
	while(random_priority > q[this_process].qkey && this_process!=rdytail){
		this_process = q[this_process].qnext;
	}
	
	//implementing Round Robin
	while(q[this_process].qkey == q[q[this_process].qnext].qkey){
		this_process=q[this_process].qnext;
	}
	
	if(this_process == rdytail)
		this_process = q[rdytail].qprev;

		return (this_process);
}



int resched()
{
	register struct	pentry	*optr;	/* pointer to old process entry */
	register struct	pentry	*nptr;	/* pointer to new process entry */
	int next_process;
        
	// Condition for Exponential schedular
	if(stype==0){
		//get the current running process
		optr = &proctab[currpid];
		// push the current process on the queue
		if (optr->pstate == PRCURR ) {
                	        optr->pstate = PRREADY;
                               	insert(currpid,rdyhead,optr->pprio);
        	}
		
		//setting lambda value and calling the exponential function
		double lambda = 0.1;
		int exp= (int)expdev(lambda);
		//printf("Expdev value: %ld\n",exp);
		
		// for testing the output of the exponential values
		if(exp>0 && exp<=10)
			a++;
		else if(exp>10  && exp<=20)
			b++;
		else if(exp>20  && exp<=30)
			c++;		
		
		//get the next process on the ready queue as per the scheduling logic	
		next_process = getnextproc(exp);
		//printf("current pid: %d\n",currpid);

	}

	// Condition for Linux type schedular
	if(stype==1){
		//struct pentry *pointer;
		int next=currpid;  		/* current process id */
		int eflag=1;      		/* flag for new epoch is required */
		optr = &proctab[currpid]; 	/* pointer to the current running process*/
		
		//Handling the condition for NULL process execution
		if(preempt<=0){
			preempt=0;
			proctab[currpid].counter = 0;
			proctab[currpid].goodness= 0;
		}
		
		optr->goodness = optr->goodness - optr->counter + preempt;
		optr->counter = preempt;
		
		//checking if new epoch needs to be set
		int i=0;
		while(i < NPROC){
			if((proctab[i].pstate == PRREADY)&&(proctab[i].counter>0)){
				eflag = 0;
				break;
			} i++;
		}
		//printf("eflag: %d\n",eflag);
		//Current process runs if its counter value is more than zero, with its counter value as quantum
                if((eflag==1) && proctab[currpid].pstate==PRCURR && proctab[currpid].counter>0){
                        preempt = proctab[currpid].counter;
                        return OK;
                }
		
		// For new Epoch
		if(eflag==1){
        		int i=0;
        		while(i<NPROC){
				//adding the given counter condition
                		if(proctab[i].counter > 0)
                        		proctab[i].counter =  proctab[i].pprio + proctab[i].counter/2;
               			else
                        		proctab[i].counter = proctab[i].pprio;
				//adding given goodness condition
                        	proctab[i].goodness = proctab[i].counter + proctab[i].pprio;
				i++;
        		}	
		}
		//printf("check3\n");
		
		//finding the maximum goodness to get the next running process
		int maxgoodness = 0;
		int this_process = q[rdyhead].qnext;
		while(this_process!=rdytail){
			if(proctab[this_process].goodness>maxgoodness){
				maxgoodness = proctab[this_process].goodness;
				next = this_process;
			}
			this_process = q[this_process].qnext;
		}
		//printf("maxgoodness: %d\n",maxgoodness);

		//If goodness of current process is greater than the max value from the queue
		if((maxgoodness < proctab[currpid].goodness)&&(proctab[currpid].pstate == PRCURR)){
				preempt = proctab[currpid].counter;
				return OK;			
		}
		else 
		{
			//current process to ready state
			if (optr->pstate == PRCURR)
                       	{
                               	optr->pstate = PRREADY;
                               	insert(currpid,rdyhead,optr->pprio);
                       	}
			//scheduling the next process with max goodness value
                       	dequeue(currpid=next);
                       	nptr = &proctab[currpid];
                       	nptr->pstate = PRCURR;
			preempt = nptr->counter;
                       	ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
			return OK;
		}
		
	}

	/* force context switch*/	
	//switched the next process to the current process
	
	if( stype == 0 ){
			nptr = &proctab[ (currpid = dequeue(next_process)) ];
	}
	else{
			nptr = &proctab[ (currpid = getlast(rdytail)) ];
	}

	
	nptr->pstate = PRCURR;		/* mark it currently running	*/

	//default preempt value
	#ifdef	RTCLOCK
		preempt = QUANTUM;	/* reset preemption counter	*/
	#endif

	ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);

	/* The OLD process returns here when resumed. */
	return OK;
}
