#include<iostream>
#include<mpi.h>
#include<vector>

double Function(double x)
 { double y =1/((x*x)+1);
   return y;
 }
int main(int argc,char *argv[])
 {     
	MPI_Init(&argc,&argv);
	MPI_Status status;
	int rank,size;
        MPI_Comm_size(MPI_COMM_WORLD,&size);
        MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	int n =105;
	int part=(n-1)/size;
	std::cout<<"part"<<part;
	int remainder =(n-1)%size;
	double a = 1,b=2;
	double h=(b-a)/n;
	int start = 0;
     
	if(rank==0)
        { double localsum =0.0;
	  double globalsum = 0.0;
	  
	 for(int i=1;i<size;i++)
	 {
           start = (i*part)+1;
           MPI_Send(&start,1,MPI_INT,i,1,MPI_COMM_WORLD);
	 }
         
	 for(int i=1;i<=part;++i)
	{ 
          if(i%3==0)
           { localsum += 2*Function(a + (i*h)); }
	  else
           { localsum += 3*Function(a + (i*h)); }
	}
	 //Printing localsum on processor zero 
	  std::cout<<rank<<" " <<"Localsum"<<localsum<<std::endl;
        double totalsum = localsum;
	 for(int i=1;i<size;++i)
      {      double sum;
             MPI_Recv(&sum,1,MPI_DOUBLE,i,2,MPI_COMM_WORLD,&status);
             totalsum +=sum;
	     std::cout<<"Local sum on rank "<<i<<" = "<<sum<<std::endl;
             //std::cout<<rank<<i<<" Totalsum ="<<totalsum<<std::endl;
           }
	 
	 globalsum = Function(a) + Function(b) + totalsum;

	double ans = ((3*h)/8)*globalsum; 
        std::cout << "Ans "<<ans<<std::endl;
      }
	
      if(rank !=0)
      {   for(int i=1;i<size;i++)
         {

           MPI_Recv(&start,1,MPI_INT,0,1,MPI_COMM_WORLD,&status);
	 int end = (start+part)-1;
	 std::cout<<"start"<<start<< "end  "<<end<<std::endl; 
         double localsum = 0.0;
        for(int i=start;i<=end;++i)
        {  
          if(i%3==0)
           { localsum += 2*Function(a+(i*h)); }
          else
           { localsum += 3*Function(a+(i*h)); }
	  MPI_Send(&localsum,1,MPI_DOUBLE,0,2,MPI_COMM_WORLD);

        }
	std::cout<<"My local sum on "<<rank<< "= "<<localsum<<std::endl;
       }
	MPI_Finalize();
 
      }
 }
