#include<iostream>
#include<mpi.h>
<<<<<<< HEAD
=======
#include<vector>
>>>>>>> 82f56282393c336fb48084a966a4addbbdad2a5d
using namespace std;
int main(int argc,char *argv[])
{
   
   MPI_Init(&argc,&argv);
   MPI_Status status;
   int rank,size;
   MPI_Comm_size(MPI_COMM_WORLD, &size);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   int n;
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
      MPI_Recv(&n,1,MPI_INT,1,1,MPI_COMM_WORLD,status);
   }
   int part = n/size;
   int remainder = n%size;
   if(rank < remainder)
      part = (part+1);
   int start = (rank*part)+min(rank,remainder)+1;
   int end   = start+part-1;
   for(int i=start;i<=end;++i)
   { int localsum += i; }
   cout<<"Localsum on processor "<<rank<<"="<<localsum<<endl;
   if(rank !=0)
   {
      MPI_send(&localsum,1,MPI_INT,0,2,MPI_COMM_WORLD);
   }
   
   if(rank == 0)
   { int totalsum = localsum;
     cout<<"Localsum on processor 0 "<<localsum<<endl;
     int sum1;
     for(int i=1;i<size;i++)
     {
     MPI_Recv(&sum1,1,MPI_INT,i,2,MPI_COMM_WORLD,status);
     totalsum += sum1;
     }
   cout<<"Totalsum ="<<totalsum<<endl;
   }
   MPI_Finalize();
 }