static	long	randx = 1;
//long RAND_MAX;

srand(unsigned x)
{
	randx = x;
	
}

rand()
{	
	//RAND_MAX=(randx*1103515245 + 12345) & 077777;
	return(((randx = randx*1103515245 + 12345)>>16) & 077777);
}
