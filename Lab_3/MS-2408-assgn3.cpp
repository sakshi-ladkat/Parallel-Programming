#include<mpi.h>
#include<iostream>
#include<vector>
#include<fstream>
#include<algorithm>
using namespace std;


// Partitioning local array 
int partition(vector<int>arr,int pivot)
{
  int i = 0;
  int n = arr.size();
  for(int j = 0;j < n;j++)
  {
    if(arr[j] <= pivot)
       {
         swap(arr[i],arr[j]);
         i++;
       }
   }
   
   return i;
  }
  
  
 // Quicksort 
 void quicksort(vector<int> &localData,MPI_Comm comm)
  {
    int rank,size;
    
    MPI_Comm_rank(comm,&rank);
    MPI_Comm_size(comm,&size);
    
    if(size == 1)
    {
      sort(localData.begin(),localData.end());
      return;
    }
    
    int pivot = 0;
    if(rank == 0)
    {
     if(!localData.empty())
     {
       pivot = localData[localData.size() / 2];
     }
    }
    
    MPI_Bcast(&pivot,1,MPI_INT,0,comm);
    
    int mid = partition(localData,pivot);
    vector<int>left(localData.begin(),localData.begin()+mid);
    vector<int>right(localData.begin()+mid,localData.end());
    
    int color;
    int half = size/2;
    int partner = -1;
    
    if(rank < half)
     { 
        partner = rank + half;
        color = 0; }
    else if (rank >= half && rank < 2* half)
     {
       partner = rank - half;
       color = 1;
     }
     else
     {   partner = MPI_PROC_NULL;
         color = 2; }
     
     MPI_Status status;
     int sendCount = 0 , recvCount = 0;
     vector<int>recvbuf;
     
     if(color == 0)
      {
          sendCount = right.size();
          MPI_Sendrecv(&sendCount,1,MPI_INT,partner,0,&recvCount,1,MPI_INT,partner,0,comm,&status);
          
          recvbuf.resize(recvCount);
          MPI_Sendrecv(right.data(),sendCount,MPI_INT,partner,1,recvbuf.data(),recvCount,MPI_INT,partner,1,comm,&status);
          
          left.insert(left.end(),recvbuf.begin(),recvbuf.end());
          localData = left;
       }
    
    	else if( color == 1)
       {
         int sendCount = left.size();
         MPI_Sendrecv(&sendCount,1,MPI_INT,partner,0,&recvCount,1,MPI_INT,partner,0,comm,&status);
         
         recvbuf.resize(recvCount);
         MPI_Sendrecv(left.data(),sendCount,MPI_INT,partner,1,recvbuf.data(),recvCount,MPI_INT,partner,1,comm,&status);
         
         right.insert(right.end(),recvbuf.begin(),recvbuf.end());
         
         localData = right;
         
        }
        
        else
        {
        
        }
        
        MPI_Comm newcomm;
        MPI_Comm_split(comm,color,rank,&newcomm);
        
        if(color == 0 || color == 1)
        {
          quicksort(localData,newcomm);
        }
        else
        {
         sort(localData.begin(),localData.end());
        }
        MPI_Comm_free(&newcomm);
        
       }
       
       int main(int argc,char **argv)
       {
         
         MPI_Init(&argc,&argv);
         
         int rank,size;
         MPI_Comm_rank(MPI_COMM_WORLD, &rank);
         MPI_Comm_size(MPI_COMM_WORLD, &size);
         
         vector<int> data;
         int n = 0;
         
         if(rank == 0)
         {
            ifstream fin("input.txt");
            if(!fin)
            {
              cerr << "Error : cannot open input.txt\n";
              MPI_Abort(MPI_COMM_WORLD,1);
            }
            fin>>n;
            data.resize(n);
            for(int i = 0;i < n;i++)
               { fin >> data[i]; }
             fin.close();
          }
          
          MPI_Bcast(&n,1,MPI_INT,0,MPI_COMM_WORLD);
          
          vector<int> counts(size),displs(size);
          int base = n/size;
          int rem =  n%size;
          int offset = 0;
          for(int i = 0;i < size;i++)
          {
            counts[i] = base + (i < rem ? 1:0);
            displs[i] = offset;
            offset += counts[i];
          }
          
          int local_n = counts[rank];
          vector<int> localData(local_n);
          
          MPI_Scatterv(data.data(),counts.data(),displs.data(),MPI_INT,localData.data(),local_n,MPI_INT,0,MPI_COMM_WORLD);
          
          quicksort(localData,MPI_COMM_WORLD);
          
          vector<int>recvCounts(size),recvDispls(size);
          int localSize = localData.size();
          MPI_Gather(&localSize,1,MPI_INT,recvCounts.data(),1,MPI_INT,0,MPI_COMM_WORLD);
          
          if(rank == 0)
          {
            recvDispls[0]=0;
            for(int i = 1;i< size;i++)
             {
               recvDispls[i] = recvDispls[i-1] + recvCounts[i - 1];
             }
             
            data.resize(recvDispls[size - 1]+recvCounts[size - 1]);
          }
          
          MPI_Gatherv(localData.data(),localSize,MPI_INT,data.data(),recvCounts.data(),
          recvDispls.data(),MPI_INT,0,MPI_COMM_WORLD);
          
          if(rank == 0)
          {
             //sort(data.begin(),data.end());
             
             ofstream fout("output.txt");
             for (int val : data)
             {
                fout << val << " ";
             }
             
             fout<<endl;
             fout.close();
             cout<< " Sorted Data Written to output.txt\n";
           }
           
           MPI_Finalize();
           return 0;
         }
       
       
         
         

       
    
     
    
     
