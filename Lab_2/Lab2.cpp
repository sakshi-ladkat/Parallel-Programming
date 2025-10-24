#include<iostream>
#include<mpi.h>
#include<cmath>
double Function(double x)
{
   double y = x*x*exp(-x);
   return y;
}
   
using namespace std;
int main(int argc,char *argv[])
{
   
   MPI_Init(&argc,&argv);
   MPI_Status status;
   int rank,size;
   MPI_Comm_size(MPI_COMM_WORLD, &size);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   int n;
   double a = 0 , b =4;
  
   if(rank == 0)
   {
     cout<<"Enter value of n"<<endl;
     cin>>n;
     for(int i=1;i<size;i++)
     {
     MPI_Send(&n,1,MPI_INT,i,1,MPI_COMM_WORLD);
     }
   }
   
   if(rank !=0)
   {
      MPI_Recv(&n,1,MPI_INT,1,1,MPI_COMM_WORLD,&status);
   }
   int part = n/size;
   int remainder = n%size;
   if(rank < remainder)
      part = (part+1);
   int start = (rank*part)+min(rank,remainder)+1;
   int end   = start+part-1;
   double h = (b-a)/n;
   
   double localsum = 0;
   for(int i=start;i<=end;++i)
   { 
       if(i%3==0)
       { localsum += 2*Function(a+(i*h)); }
       else
       { localsum += 3*Function(a+(i*h)); }
   }
   cout<<"Localsum on processor "<<rank<<"="<<localsum<<endl;
   if(rank !=0)
   {
      MPI_Send(&localsum,1,MPI_INT,0,2,MPI_COMM_WORLD);
   }
   
   if(rank == 0)
   { double globalsum = localsum;
     cout<<"Localsum on processor 0 "<<localsum<<endl;
     int sum1;
     for(int i=1;i<size;i++)
     {
     MPI_Recv(&sum1,1,MPI_INT,i,2,MPI_COMM_WORLD,&status);
     globalsum += sum1;
     }
   cout<<"Globalsum ="<<globalsum<<endl;
   double ans = ((3*h)/8) * globalsum;
   cout<<"Ans : "<<ans<<endl;
   }
   MPI_Finalize();
 }
