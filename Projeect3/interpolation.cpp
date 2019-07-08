#include "stdafx.h"
#include "Interpolation.h"



double Interpolation::result(double var)
{
	double x[n_array] = {23.5,24.8,26,27.8,29,30.6,32.3,34,35.82,37.69,40.3,42,44.5,47,49.81,52,58.6,62.3,66.2,70.7,75.2,80.66,86,100.6,115.2,136.8,166.6,206.1,260.8,362.9,556.8,1113};
	double y[n_array] = {35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,52,53,54,55,56,57,58,60,62,64,66,68,70,72,74,76};
	double temp,f[n_array],sum =0,p = var;
	int i,j,k=0;
	if(p  <= 0) return 0;


if(p<=50) {
	for(i=0;i<16;i++)
	{
		
		temp = 1;
		k = i;
		for(j=0;j<16;j++)
		{
			if(k==j)
			{
				continue;
			}
			else
			{
				temp = temp * ((p-x[j])/(x[k]-x[j]));

			}
		}
		f[i]=y[i]*temp;
		
	}
	
		for(i=0;i<16;i++)
	{
		sum = sum + f[i];
	}
		return sum;
}

else if(p>50&&p<120)

{
	for(i=16;i<25;i++)
	{

		temp = 1;
		k = i;
		for(j=16;j<25;j++)
		{
			if(k==j)
			{
				continue;
			}
			else
			{
				temp = temp * ((p-x[j])/(x[k]-x[j]));

			}
		}
		f[i]=y[i]*temp;

	}

	for(i=16;i<25;i++)
	{
		sum = sum + f[i];
	}
	return sum;
}
else if(p>=120&&p<134) return 63;
else if(p>=134&&p<140) return 64;
else if(p>=140&&p<155) return 65;
else if(p>=155&&p<180) return 66;
else if(p>=180&&p<190) return 67;
else if(p>=190&&p<210) return 68;
else if(p>=210&&p<250) return 69;
else if(p>=250&&p<310) return 70;
else if(p>=310&&p<350) return 71;
else if(p>=350&&p<390) return 72;
else if(p>=390&&p<450) return 73;
else if(p>=450&&p<580) return 74;
else if(p>=580&&p<730) return 75;
else if(p>=730&&p<1200) return 76;


	
}