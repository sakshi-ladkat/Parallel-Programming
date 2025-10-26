#include<mpi.h>
#include<iostream>
#include<vector>
#include<fstream>
#include<iomanip>

using namespace std;

int main(int argc,char ** argv)
{

   MPI_Init(&argc,&argv);
   
   int rank,size;
   MPI_Comm_rank(MPI_COMM_WORLD,&rank);
   MPI_Comm_size(MPI_COMM_WORLD,&size);
   
   int row_size = 0 , col_size = 0;
   vector<double>A;
   vector<double>x;
   
   /* 
   Rank 0 reads data from input file and store 
   Matrix in Vector A and Vector in Vector X 
   */
   if(rank == 0)
   {
     cout<< "--Rank 0 : starting File Read ---\n";
     ifstream fin("input.txt");
     if(!fin)
     {
       cerr<<"Rank 0 Error : Cannot open input.txt"<<endl;
       MPI_Abort(MPI_COMM_WORLD,1);
   }
   
   cout<<"Rank 0 : file 'input.txt' opened successfully.\n";
   
   cout << "Rank 0 : --- Reading dimention ----\n";
   if(!(fin>> row_size >> col_size))
     {
        cerr << "Rank 0 Error : Failed to read rows and columns.\n";
        MPI_Abort(MPI_COMM_WORLD,1);
     }
     
   cout <<"Rank 0 : rows"<<row_size<<"\n cols :"<<col_size<<"\n";
   A.resize(row_size * col_size);
   int m = row_size * col_size;
   for(int i=0 ;i< m ;++i)
   fin>>A[i];
   
   int vec_size;
   fin>> vec_size;
   if(vec_size != col_size)
   {
      cerr<<"Vector length must match matrix columns ! \n";
      MPI_Abort(MPI_COMM_WORLD,1);
   }
   
   x.resize(vec_size);
   for(int i=0;i < vec_size;++i)
   {
     fin>> x[i];
   }
   
   fin.close();
  }
  
  // Broadcasting row_size , col_size and vec_size
  
  MPI_Bcast(&row_size,1,MPI_INT,0,MPI_COMM_WORLD);
  MPI_Bcast(&col_size,1,MPI_INT,0,MPI_COMM_WORLD);
  
  if(rank !=0)  x.resize(col_size);
  
  MPI_Bcast(x.data(),col_size,MPI_DOUBLE,0,MPI_COMM_WORLD);
  
  
  
  // Computing Row Distribution for each processor
  
  vector<int> rows_counts(size,0);
  for(int i=0;i < size;++i)
   {
     rows_counts[i] = (row_size / size) + (i < (row_size % size)  ? 1 : 0);
   }
   
   vector<int> sendcounts(size) , displs(size);
   int offset = 0;
   for(int i =0;i<size;++i)
   {
      sendcounts[i] = rows_counts[i] * col_size;
      displs[i] = offset;
      offset += sendcounts[i]; 
   }
   
   int local_rows = rows_counts[rank];
   int localA_size = local_rows * col_size;
   vector<double>localA(localA_size);
   
   
   // Scatter rows to all ranks
   
   MPI_Scatterv(A.data(),sendcounts.data(),displs.data(),MPI_DOUBLE,localA.data(),localA_size,MPI_DOUBLE,0,MPI_COMM_WORLD);
   
   
   // Local Matrix vector Multiplication
   
   vector<double> localY(local_rows,0.0);
   for(int i=0;i<local_rows;++i)
     {
      for(int j=0;j< col_size;++j)
        {
          localY[i] += localA[i * col_size + j] * x[j];
        }
     }
     
     // Gather results
     
     vector<double> y;
     vector<int> recv_displs(size);
     
     if(rank == 0)
      {
         y.resize(row_size);
      }
      recv_displs[0] = 0;
      for(int i=1;i<size;++i)
      {
           recv_displs[i] = recv_displs[i-1] + rows_counts[i-1];
      }
         
        MPI_Gatherv(localY.data(),local_rows,MPI_DOUBLE,y.data(),rows_counts.data(),
        recv_displs.data(),MPI_DOUBLE,0,MPI_COMM_WORLD);
        
        
        // Output Result
        
        if(rank == 0)
        {
           ofstream fout("output.txt");
           for(int i = 0;i<row_size;++i)
           {  fout << y[i]<<" "; }
           
           fout << endl;
           fout.close();
           cout<< "Result Written to output.txt\n";
        }
        
        MPI_Finalize();
        return 0;
    }
        
  
  
