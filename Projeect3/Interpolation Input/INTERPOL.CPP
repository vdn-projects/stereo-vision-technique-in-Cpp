#include<iostream.h>
#include<conio.h>
#include<stdlib.h>
#define MENU1 49
#define MENU2 50
#define MENU3 51
#define MENU4 52
class Numerical{
			public:
					  Numerical(){}
                 float f(float);
					  float df(float);
					  float power(float,int);
					  int factorian(int);
					  void Newton_Raphson();
					  void get_function();
					  float*coefficient,degree;};
void Numerical::get_function()
{
 cout<<"Enter the degree of the function:"<<endl;
 cin>>degree;
 coefficient=new float[degree+1];
 for(int i=degree;i>=0;i--)
	 {cout<<"Enter the coefficient of x^"<<i<<":"<<endl;
	  cin>>coefficient[i];}
}
float Numerical:: power(float a,int i)
{
 float temp=1;
 if(i!=0)
 {for(int ii=1;ii<=i;ii++)
	  temp*=a;}
 return temp;
}
float Numerical:: f(float x)
{
 float value=0;
 for(int i=degree;i>=0;i--)
	  value+=coefficient[i]*power(x,i);
 return value;
}
float Numerical:: df(float x)
{
 float value=0;
 for(int i=degree;i>0;i--)
	 value+=coefficient[i]*i*power(x,i-1);
 return value;
}
void Numerical::Newton_Raphson()
{
 get_function();
 cout<<"Enter an intial value:"<<endl;
 float intial;
 int counter=0;
 cin>>intial;
 float a=intial;
 float c=1,previous_c=0;
 while(c!=previous_c)
		{++counter;
		 c=a-f(a)/df(a);
		 previous_c=a;
		 a=c;
		 cout<<"x"<<counter<<"="<<a<<endl;}
 cout<<"Completed in "<<counter<<" iterations with your intial value."<<endl;
}
int Numerical::factorian(int n)
{
 int value=1;
 if(n)
	value*=n*factorian(n-1);
 return value;
}
class interpolation:public Numerical{
									protected:
												 int n,r;
												 float*y,p;
										public:
												 inline float delta(int,const float*,int);
												 void get_points();
												 void show_table_values();
												 float get_p(int);
												 void forward_interpolate();
												 };

void interpolation::get_points()
{
 cout<<"Enter number of points:"<<endl;
 cin>>n;
 y=new float[n];
 for(int i=0;i<n;i++)
	  {cout<<"Enter y["<<i<<"]"<<endl;
		cin>>y[i];}
}
void interpolation::show_table_values()
{
 while(1){
			 cout<<"Enter the order of delta(0 to exit):"<<endl;
			 cin>>n;if(n==0)break;
			 cout<<"Enter the the r value :"<<endl;
			 cin>>r;
			 cout<<delta(n,y,r)<<endl;}
}
float interpolation::delta(int n,const float*f,int r)
{
 float delta_value;
 if(n==1)
	 delta_value=f[r+1]-f[r];
 else
	delta_value=delta(n-1,f,r+1)-delta(n-1,f,r);
 return delta_value;
}
void interpolation::forward_interpolate()
{
 float x0,xp,h,yp;
 cout<<"Enter x[0]:"<<endl;
 cin>>x0;
 cout<<"Enter the step h:"<<endl;
 cin>>h;
 while(1){cout<<"Enter the interpolated point x[p](0 to exit):"<<endl;
			 cin>>xp;if(xp==0)break;
			 p=(xp-x0)/h;
			 yp=y[0];
			 for(int i=1;i<=n-1;i++)
				 yp+=get_p(i)*delta(i,y,0);
			 cout<<"The function at "<<xp<<" has the value "<<yp<<endl;}
}
float interpolation::get_p(int term)
{
 float temp=1;
 for(int i=0;i<term;i++)
	  temp*=p-i;
 return (temp/factorian(term));
}
class inverse_interpolation:interpolation{
											  public:
														void inverse_interpolate();
													  };
void inverse_interpolation::inverse_interpolate()
{
 float yp,previous_p,temp,p_i,x0,h;
 int counter=0;
 previous_p=0;
 temp=0;
 p_i=1;
 p=0;
 get_points();
 cout<<"Enter yp:"<<endl;
 cin>>yp;
 cout<<"Enter x[0]"<<endl;
 cin>>x0;
 cout<<"Enter the step h"<<endl;
 cin>>h;
 cout<<"Starting at p[0]=0"<<endl;
 while(p_i!=previous_p)
		{
		 counter++;
		 for(int i=2;i<n;i++)
		 temp+=get_p(i)*delta(i,y,0);
		 p_i=(1/delta(1,y,0))*(yp-y[0]-temp);
		 previous_p=p;
		 p=p_i;
		 cout<<"p["<<counter<<"]="<<p<<endl;
		}
 cout<<"The function has the value "<<yp<<" at the point "<<(x0+p*h)<<endl;
}
void case_();
main()
{
 interpolation object;
 inverse_interpolation ob;
 while(1){
 cout<<"1.Newton_Raphson"<<endl;
 cout<<"2.Newton_Forward"<<endl;
 cout<<"3.Inverse Interpolation"<<endl;
 cout<<"4.Exit"<<endl;
 switch (getch())
  {
	case MENU1:
				  object.Newton_Raphson();
				  break;
	case MENU2:
				  case_();
				  break;
	case MENU3:
				  ob.inverse_interpolate();
				  break;
	case MENU4:
				exit(0);
	default :
				exit(0);
				break;
  }
 }
}
void case_()
{
 interpolation object;
 cout<<"   1.Interpolation"<<endl<<"   2.Table values"<<endl<<"   3.Exit"<<endl;
					switch(getch())
							{
							 case MENU1:
											object.get_points();
											object.forward_interpolate();
											break;
							 case MENU2:
											object.get_points();
											object.show_table_values();
											break;
							 case MENU3:
											break;
							 default   :
											break;}
}
