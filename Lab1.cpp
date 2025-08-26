#include<iostream>
#include<mpi.h>
#include<vector>
int main(int argc,char *argv[])
{

    MPI_Init(&argc,&argv);
    MPI_Status status;
    int rank,size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    //std::cout<<" My rank "<<rank<<"  Out of "<<size<<std::endl;
    int n=100;int part =n/size;
    if(rank==0)
    {   int localsum=0;
       std::vector<int>A;
       for(int i=1;i<=n;i++)
        { A.push_back(i);}
       MPI_Send(A.data()+part,part,MPI_INT,1,1,MPI_COMM_WORLD);
        for(int i=0;i<part;i++)
        { localsum+=A[i];}

        /* Printing localsum on processor 0
        std::cout<<"For "<<rank<<" "<<localsum<<std::endl;
        */
        int sum1;
        MPI_Recv(&sum1,1,MPI_INT,1,2,MPI_COMM_WORLD,&status);
        int totalsum=localsum+sum1;
        std::cout<<"Totalsum ="<<totalsum<<std::endl;
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
        { localsum+=B[i];}
         /* Printing localsum on processor 0
        std::cout<<"For "<<rank<<" "<<localsum<<std::endl;
        */
        MPI_Send(&localsum,1,MPI_INT,0,2,MPI_COMM_WORLD);
        }
    MPI_Finalize();
  }
