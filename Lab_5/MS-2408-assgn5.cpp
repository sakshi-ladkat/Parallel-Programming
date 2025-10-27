#include<iostream>
#include<fstream>
#include<vector>
#include<mpi.h>
using namespace std;

int main(int argc,char *argv[])
 {
    MPI_Init(&argc,&argv);
    MPI_Status status;
    
    int rank,size;
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    
    int n, m; // Dimension Of A
    int p, k; //Dimension  of B
    
    vector<int>A,B;
    if(rank == 0)
    {
      ifstream fileA("matrixA.txt");
      ifstream fileB("matrixB.txt");
      
      if(!fileA.is_open() || !fileB.is_open())
      {
        cerr<<"Error : Could not open input files !\n";
        MPI_Abort(MPI_COMM_WORLD,1);
      }
      
      
      fileA >> n >> m;
      A.resize(n*m);
      for(int i = 0;i< n*m;++i)
       {  fileA >> A[i]; }
      fileA.close();
      
      fileB >> p >> k;
      B.resize(p*k);
      for(int i = 0; i< p*k;++i)
      { fileB >> B[i]; }
      fileB.close();
      
      if(m != p)
      {
        cerr<<"Error : Cannot multiply matrices" 
        <<"matrix A has"<< m << "columns but matrix B has"<< p << "rows\n";
        MPI_Abort(MPI_COMM_WORLD,1);
      }
      
      cout<< "Matrix Multiplication possible:("<< n << "x" << m << ") + ("<< p << "x" << k << ")\n";
    }
    
    MPI_Bcast(&n,1,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Bcast(&m,1,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Bcast(&p,1,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Bcast(&k,1,MPI_INT,0,MPI_COMM_WORLD);
    
    if(rank != 0)
    {  B.resize(p*k); }
    
    MPI_Bcast(B.data(),p*k,MPI_INT,0,MPI_COMM_WORLD);
    
    int base_rows = n/size;
    int remainder = n%size;
    
    vector<int>rows_count(size);
    for(int i=0;i<size;i++)
    {
      rows_count[i] = base_rows + (i < remainder ? 1 : 0);
    }
    
    vector<int> sendcounts(size) , displs(size);
    int offset = 0;
    for(int i = 0 ;i< size;i++)
    {
       sendcounts[i] = rows_count[i]*m;
       displs[i] = offset;
       offset += sendcounts[i];
    }
    
    int local_row = rows_count[rank];
    vector<int>local_A(local_row* m);
    
    // Scatter Matrix A 
    
    MPI_Scatterv(A.data(),sendcounts.data(),displs.data(),MPI_INT,local_A.data(),local_row * m,MPI_INT,0,MPI_COMM_WORLD);
    
    
    vector<int>local_C(local_row * k,0);
    for(int i = 0;i< local_row;i++)
    {
      for(int j = 0;j< k;j++)
       {
         for(int x = 0;x < m;x++)
         {
           local_C[i *k +j] += local_A[i* m+x] * B[x *k +j];
          }
        }
     }
     
     //Gather results
     
     vector<int> recvcounts(size),recv_displs(size);
     offset = 0;
     for(int i = 0;i<size;i++)
     {
       recvcounts[i] = rows_count[i]*k;
       recv_displs[i] = offset;
       offset += recvcounts[i];
     }
     
     vector<int> C;
     if(rank == 0)
     { C.resize(n*k); }
     
     MPI_Gatherv(local_C.data(),local_row *k,MPI_INT,C.data(),recvcounts.data(),recv_displs.data(),MPI_INT,0,MPI_COMM_WORLD);
     
     if(rank == 0)
     {
       ofstream out("matrixC.txt");
       out << n << " "<< k << "\n";
       for(int i = 0;i < n;i++)
       {
        for(int j = 0;j < n;j++)
        {
           out<< C[i*k+j]<<" ";
        }
        out<<"\n";
       }
       out.close();
       cout<<"Result Written to MatrixC.txt\n";
      }
      
      MPI_Finalize();
      return 0;
      }
