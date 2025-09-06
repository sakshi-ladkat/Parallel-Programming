#include<iostream>
#include<mpi.h>
#include<vector>
using namespace std;
int main(int argc,char *argv[])
{
   
   MPI_Init(&argc,&argv);
   MPI_Status status;
   int rank,size;
   MPI_Comm_size(MPI_COMM_WORLD, &size);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   //std::cout<<" My rank "<<rank<<"  Out of "<<size<<std::endl;
   int n=100;
   int part =n/size;
   int remainder = n%size;
   if(rank==0)
   {   int localsum=0;
      std::vector<int>A;
      for(int i=0;i<n;i++)
       { A.push_back(i+1);}
      int current_offset = 0;
       // Calculating local sum on Array having rank 0
       for(int i=0;i<part;i++)
       { localsum+=A[i];}
    
       // Printing localsum on processor 0
       std::cout<<rank<<" "<<localsum<<std::endl;
      for(int i=1;i<size;++i)
      { int send_count;
	if( i < remainder)
         { send_count = (part+1);
	   current_offset = part+1;
	 }
	else
         { send_count = part; 
	   current_offset += part;
	 }   

      // Send data to process 'i' starting from the calculated offset
      MPI_Send(A.data()+current_offset,send_count,MPI_INT,i,1,MPI_COMM_WORLD);       
      }

      for(int i=1;i<size;++i)
      {
       int sum1;
       MPI_Recv(&sum1,1,MPI_INT,i,2,MPI_COMM_WORLD,&status);
       std::cout<<rank<<" Localsum received from "<<i<<" Processor = "<<sum1<<std::endl;
       int totalsum=localsum+sum1;
       std::cout<<rank<<" Totalsum ="<<totalsum<<std::endl;
   }
      
  }
   if(rank!=0)
   {
     int localsum=0;
   std::vector<int>B(part);
      MPI_Recv(B.data(),part,MPI_INT,0,1,MPI_COMM_WORLD,&status);
      for(int i=0;i<part;i++)
        {    B.push_back(i);
        }
       /* Checking Whether Data received by B or Not 
        for(int i=0;i<part;i++)
        {
           std::cout<<B[i]<<std::endl;
        }
        */
         for(int i=0;i<part;i++)
       { 
       localsum+=B[i];
       }
       //Printing localsum on each processor 
       std::cout<<rank<<" Localsum send "<<localsum<<std::endl;
       MPI_Send(&localsum,1,MPI_INT,0,2,MPI_COMM_WORLD);
       }
   MPI_Finalize();
 }
