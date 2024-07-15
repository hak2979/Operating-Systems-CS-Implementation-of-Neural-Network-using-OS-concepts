#include<string.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/shm.h>
#include<sys/wait.h>
#include<iostream>
#include<pthread.h>
#include<fstream>
#include <mutex>
using namespace std;
std::mutex mtx;
int xxx=0;

class mutx{
public:
int lock=0;
}mm;
class datacalulation{
   public:
 double **a;
 int rows;
 int cols;
 double *resul;
 double m;
};

class hiddendatacalulation{
   public:
 double ***a;
 int k;
 int rows;
 int cols;
 int id;
 double *resul;
 double m;
};
void* hiddenthread(void*args)
{
   mtx.lock();
   hiddendatacalulation *info=(hiddendatacalulation*)args;
   double x=(info->a[info->k][info->rows][info->cols]*info->m);
   info->resul[info->cols]+=x;
   mtx.unlock();
   return (NULL);
}
void *threads(void *args)
{
   while(mm.lock==1);
   mm.lock=1;
   datacalulation *info=(datacalulation*)args;
   double x=0;
      
      x+=(info->a[info->rows][info->cols]*info->m);
      info->resul[info->cols]+=x;
   mm.lock=0;
   return(NULL);
}
int main() {
   double arrprevious[2]={0,0};
   int x1 = 2;
   int y1 = 3;
   int z1 = 3;
   ifstream myfile("e.txt");
  string word="";
  char temp;
  int inputdata=2;
   double *input;
   int neurons=8;
   double **inputlweight;
   // double layer[2][3]={{0,0,0},{0,0,0}};
   int nooflayer=5;
   double *output;
   double ***hiddenlayer;
  myfile>>temp;
  while(temp!=',')
  {
    word+=temp;
    myfile>>temp;
  }
  inputdata=stoi(word);
  cout<<inputdata<<endl;
  word="";
  myfile>>temp;
  while(temp!=',')
  {
    word+=temp;
    myfile>>temp;
  }
  nooflayer=stoi(word)-2;
  cout<<nooflayer<<endl;
  word="";
  myfile>>temp;
  while(temp!='I')
  {
    word+=temp;
    myfile>>temp;
  }
  neurons=stoi(word);
  cout<<neurons<<endl;
  getline(myfile,word);
  inputlweight=new double*[inputdata];
  for(int i=0;i<inputdata;i++)
  {
    inputlweight[i]=new double[neurons];
  }
  for(int i=0;i<inputdata;i++)
  {
    getline(myfile,word);
    int index=0;
    while(word[index]!='\0')
    index++;
    string temp="";
    int j=0;
    int num=0;
    while(j<index)
    {
        temp="";
        while(word[j]!=','&& j<index)
        {
           temp+=word[j];
           j++;
        }
        inputlweight[i][num]=stod(temp);
        num++;
        j++;
    }
  }
  for(int i=0;i<inputdata;i++)
  {
    for(int j=0;j<neurons;j++)
    {
        cout<<inputlweight[i][j]<<" ";
    }
    cout<<endl;
  }

  hiddenlayer=new double**[nooflayer];
  for(int i=0;i<nooflayer;i++)
  {
    hiddenlayer[i]=new double*[neurons];
    for(int j=0;j<neurons;j++)
    {
        hiddenlayer[i][j]=new double[neurons];
    }
  }
  int i_hid=0;
  getline(myfile,word);
  while(word!="Output layer weights")
  {
    if(word[0]=='H')
    {
        for(int m=0;m<neurons;m++)
        {
            getline(myfile,word);
            if( ((word[0]=='-') || (word[0]=='+') || (word[0]>='0' && word[0]<='9')))
            {
                int index=0;
                while(word[index]!='\0')
                index++;
                string temp="";
                int j=0;
                int num=0;
                while(j<index)
                {
                    temp="";
                    while(word[j]!=','&& j<index)
                    {
                        temp+=word[j];
                        j++;
                    }
                    hiddenlayer[i_hid][m][num]=stod(temp);
                    num++;
                    j++;
                }
            }
            else
            {
                m--;
            }
        }
        i_hid++;
    }
    getline(myfile,word);
    
  }
  for(int i=0;i<nooflayer;i++)
  {
    for(int j=0;j<neurons;j++)
    {
        for(int a=0;a<neurons;a++)
        {
            cout<<hiddenlayer[i][j][a]<<" ";
        }
        cout<<endl;
    }
    cout<<"======================="<<endl;
  }
  getline(myfile,word);
  output=new double[neurons];
  int number_input_num=0;
  while(word!="Input data")
  {
    if(word[0]=='-' ||word[0]=='+'|| (word[0]>='0' && word[0]<='9'))
    {
      output[number_input_num]=stod(word);
      number_input_num+=1;
    }
    getline(myfile,word);
  }
  for(int i=0;i<neurons;i++)
  {
    cout<<output[i]<<" ";
  }
    cout<<endl;
    input=new double[inputdata];
    getline(myfile,word);
    int indexs=0;
    while(word[indexs]!='\0')indexs++;
    string temps="";
    int j=0;
    int num=0;
    while(j<indexs)
    {
        temps="";
        while(word[j]!=','&& j<indexs)
        {
            temps+=word[j];
            j++;
        }
        input[num]=stod(temps);
        num++;
        j++;
    }
    for(int i=0;i<inputdata;i++)
    {
        cout<<input[i]<<" ";
    }
    cout<<endl;
   int n=nooflayer;
   n+=2;
//-----------Check for checking if we have do foreward backward
 int forwardon=0;

 //----------Pipe for forward data
 int **fd;
 fd=new int*[n];
 for(int i=0;i<n;i++)
 {
    fd[i]=new int[2];
 }
 //----------Pipe for forward signaling
 int **fd2;
 fd2=new int*[n];
 for(int i=0;i<n;i++)
 {
    fd2[i]=new int[2];
 }
 //----------Pipe for backward signaling
 int **back;
 back=new int*[n];
 for(int i=0;i<n;i++)
 {
    back[i]=new int[2];
 }
 //----------Pipe for backward data
 int **backinput1;
 backinput1=new int*[n];
 for(int i=0;i<n;i++)
 {
    backinput1[i]=new int[2];
 }
 //----------Pipe for Again forward data
 int **fd7;
 fd7=new int*[n];
 for(int i=0;i<n;i++)
 {
    fd7[i]=new int[2];
 }
 //-----------Pipe for Again forward SIgnaling
 int **fd8;
 fd8=new int*[n];
 for(int i=0;i<n;i++)
 {
    fd8[i]=new int[2];
 }
 for(int i=0;i<n;i++)
 {
    ::pipe(fd[i]);
 }
 for(int i=0;i<n;i++)
 {
    ::pipe(fd2[i]);
 }
 for(int i=0;i<n;i++)
 {
    ::pipe(back[i]);
 }
  for(int i=0;i<n;i++)
 {
    ::pipe(backinput1[i]);
 }
 for(int i=0;i<n;i++)
 {
    ::pipe(fd7[i]);
 }
 for(int i=0;i<n;i++)
 {
    ::pipe(fd8[i]);
 }
//-----------------------------------------------------------------------------------------------------------
 int mutex1=0;
 //------------fork array for storing id so that we wait until all process finished
 pid_t *pid;
 pid=new pid_t[n];
 for(int i=0;i<n+1;i++)
 {
   int s=i;
   int ss=i;
    pid[i]=fork();
    if(pid[i]>0)
    {

    }
    else if(pid[i]==0)
    {
      //---------------------If Child Process is First
        if(s==0)
        {
           //------------------Coping array
            double **temp=new double*[inputdata];
            for(int j=0;j<neurons;j++)
            {
               temp[j]=new double[neurons];
            }

            for(int i2=0;i2<inputdata;i2++)
            {
               for(int j=0;j<neurons;j++)
               {
                  temp[i2][j]=inputlweight[i2][j];
               }
            }
            //------------Reasult array=Store data from input layer to hidden layer
            double * result=new double[neurons]{0};
            //-----------Thread array for each op
            pthread_t **t=new pthread_t*[inputdata];
            for(int i2=0;i2<inputdata;i2++)
            {
               t[i2]=new pthread_t[neurons];
            }
               for(int j=0;j<neurons;j++)
               {
                  for(int i2=0;i2<inputdata;i2++)
                  {
                     datacalulation *te=new datacalulation();
                     te->a=temp;
                     te->resul=result;
                     te->rows=i2;
                     te->cols=j;
                     te->m=input[i2];
                     pthread_create(&t[i][j],NULL,threads,(void*)te);
                  }
               }
               //-------------------------------------------------------------
            
            //---------------Assurance of Completion of all threads
            for(int i2=0;i2<inputdata;i2++)
            {
               for(int j=0;j<neurons;j++)
               {
                  pthread_join(t[i2][j],NULL);
               }
            }
            //------------------------------------------------------------------------

            //-----------------Writing in fd
           ::close(fd[i][0]);
           ::write(fd[i][1],result,neurons*sizeof(double));
           ::close(fd[i][1]);
           ::close(fd2[i][0]);
           int xx=1;
           //------------------Signaling forward fd2
           ::write(fd2[i][1],&xx,sizeof(int));
           ::close(fd2[i][1]);
           //------------------Wait of Backward
           int bkdir=0;
           while(bkdir==0)
           {
            ::close(back[i][1]);
            ::read(back[i][0],&bkdir,sizeof(int));
            ::close(back[i][0]);
           }
           //-----------------Backward data
           double *arr=new double[2];
           ::read(backinput1[i][0],arr,2*sizeof(double));
           ::close(backinput1[i][0]);
           //-------------------------------------------------
           cout<<arr[0]<<"+"<<arr[1]<<endl;
           forwardon=1;
           input[0]=arr[0],input[1]=arr[1];
           //------------------Again Repeating
           inputdata=2;
           //------------Reasult array=Store data from input layer to hidden layer
            double * resultagain=new double[neurons]{0};
            //-----------Thread array for each op
            pthread_t **tagain=new pthread_t*[inputdata];
            for(int i2=0;i2<inputdata;i2++)
            {
               tagain[i2]=new pthread_t[neurons];
            }
               for(int j=0;j<neurons;j++)
               {
                  for(int i2=0;i2<inputdata;i2++)
                  {
                     datacalulation *teagain=new datacalulation();
                     teagain->a=temp;
                     teagain->resul=resultagain;
                     teagain->rows=i2;
                     teagain->cols=j;
                     teagain->m=input[i2];
                     pthread_create(&tagain[i][j],NULL,threads,(void*)teagain);
                  }
               }
               //-------------------------------------------------------------
            
            //---------------Assurance of Completion of all threads
            for(int i2=0;i2<inputdata;i2++)
            {
               for(int j=0;j<neurons;j++)
               {
                  pthread_join(tagain[i2][j],NULL);
               }
            }
           
           ::write(fd7[i][1],&xx,sizeof(int));
           ::close(fd7[i][1]);
           ::write(fd8[i][1],resultagain,neurons*sizeof(double));
           ::close(fd8[i][1]);
           
        }
        else
        {
            //----I am checking if this process is needed or remain free
           int condition=0;
           while(condition==0)
           {
           ::close(fd2[i-1][1]);
           ::read(fd2[i-1][0],&condition,sizeof(int));
           }
           //-----Here you will apply code
           double *recieved=new double[neurons];
           ::close(fd[i-1][1]);
           ::read(fd[i-1][0],recieved,neurons* sizeof(double));
           
           for(int i2=0;i2<neurons;i2++)
           {
            cout<<recieved[i2]<<" ";
           }
           cout<<"    "<<"layer = "<<i<<endl;
           cout<<"==============================================================";
           cout<<endl;
           double *result=new double[neurons]{0};
           //-------------------Output layer
            if(s==n)
            {
               double sm=0;
                 for(int i=0;i<neurons;i++)
                 {
                  sm+=(recieved[i]*output[i]);
                 }
                 double *arr=new double[2];
                 arr[0]=(sm*sm)+sm+1;
                 arr[0]/=2;
                 arr[1]=(sm*sm)-sm;
                 arr[1]/=2;


                 cout<<arr[0]<<"//"<<arr[1]<<endl;
                 cout<<"    "<<"layer = "<<i<<endl;
                 cout<<"==============================================================";
                 cout<<endl;
                 if(forwardon==0)
                 {
                 cout<<"Backing";
                 cout<<"    "<<"layer = "<<i<<endl;
                 for(int i2=0;i2<neurons;i2++)
                 {
                     cout<<recieved[i2]<<" ";
                 }cout<<endl;
                 cout<<"==============================================================";
                 cout<<endl;
                ::close (back[i-1][0]);
                int fs=1;
                ::write(back[i-1][1],&fs,sizeof(int));
                ::close(back[i-1][1]);
                ::write(backinput1[i-1][1],arr,2*sizeof(double));
                ::close(backinput1[i-1][1]);
                 }
                                       int conditionagain=0;
                                       while (conditionagain==0)
                                       {
                                          ::close (fd7[i-1][1]);
                                          ::read(fd7[i-1][0],&conditionagain,sizeof(int));
                                       }
                                     
                                      double *receivedagain=new double[neurons];
                                      ::close(fd8[i-1][1]);
                                      ::read(fd8[i-1][0],receivedagain,neurons*sizeof(double));
                                       
                                       sm=0;
                                       for(int i=0;i<neurons;i++)
                                       {
                                          sm+=(receivedagain[i]*output[i]);
                                       }
                                       double *arragain=new double[2];
                                       arragain[0]=(sm*sm)+sm+1;
                                       arragain[0]/=2;
                                       arragain[1]=(sm*sm)-sm;
                                       arragain[1]/=2;


                                       cout<<arragain[0]<<"//"<<arragain[1]<<endl;
                                       cout<<"    "<<"layer = "<<i<<endl;
                                       cout<<"==============================================================";
                                       cout<<endl;
            }
            //---------------------------------------------------------------------
            else
            {
               //------------------------If in between layer
               if(s!=n-1)
               {
                                          pthread_t **t;
                                          t=new pthread_t*[neurons];
                                          for(int k=0;k<neurons;k++)
                                          {
                                             t[k]=new pthread_t[neurons];
                                          }
                                          double * tr=new double[neurons];
                                          int coun=0;
                                          int count1=0;
                                          for(int j=0;j<neurons;j++)
                                             {
                                                for(int i2=0;i2<neurons;i2++)
                                                {

                                                   hiddendatacalulation * ts=new hiddendatacalulation();
                                                   ts->a=new double**[neurons];
                                                   for(int i33=0;i33<neurons;i33++)
                                                   {
                                                      ts->a[i33]=new double*[neurons];
                                                      for(int i56=0;i56<neurons;i56++)
                                                      {
                                                         ts->a[i33][i56]=new double[neurons];
                                                      }
                                                   }
                                                   //-----------------------Coping array
                                                   for(int i11=0;i11<nooflayer;i11++)
                                                   {
                                                      for(int i22=0;i22<neurons;i22++)
                                                      {
                                                         for(int i33=0;i33<neurons;i33++)
                                                         {
                                                            ts->a[i11][i22][i33]=hiddenlayer[i11][i22][i33];
                                                         }
                                                      }
                                                   }
                                                  
                                                   //-----------------------------
                                                   ts->k=i-1;
                                                   
                                                   ts->resul=result;
                                                   ts->rows=i2;
                                                   ts->cols=j;
                                                   ts->m=recieved[coun];
                                                   pthread_create(&t[i2][j],NULL,hiddenthread,(void*)ts);
                                                   coun++;
                                                   if(coun>neurons-1)
                                                   coun=0;
                                                   count1++;
                                                   cout<<" ";
                                                }
                                             }
                                          //-------------------------

                                          //-----------------Assurance of completion of theads
                                       for(int j=0;j<neurons;j++)
                                       {
                                          for(int jj=0;jj<neurons;jj++)
                                          {
                                             pthread_join(t[j][jj],NULL);
                                          }
                                       }
                                       //-----------------------------------
                                       
                                       ::close(fd[i-1][0]);
                                       ::write(fd[i][1],result,neurons*sizeof(double));
                                       ::close(fd[i][1]);
                                       //------Signaling forward to next layer
                                       ::close (fd2[i-1][0]);
                                       condition=1;
                                       ::write(fd2[i][1],&condition,sizeof(int));
                                       ::close(fd2[i][1]);
               }
               else
               {
                                       ::close(fd[i-1][0]);
                                       ::write(fd[i][1],recieved,neurons*sizeof(double));
                                       ::close(fd[i][1]);
                                       //------Signaling forward to next layer
                                       ::close (fd2[i-1][0]);
                                       condition=1;
                                       ::write(fd2[i][1],&condition,sizeof(int));
                                       ::close(fd2[i][1]);
               }
               if(forwardon==0)
               {
                                       int bw=0;
                                       while(bw==0)
                                       {
                                          ::close(back[i][1]);
                                          ::read(back[i][0],&bw,sizeof(int));
                                          ::close(back[i][0]);
                                       }
                                       cout<<endl;
                                       cout<<"Backing";
                                       cout<<"    "<<"layer = "<<i<<endl;
                                       for(int i2=0;i2<neurons;i2++)
                                       {
                                          cout<<recieved[i2]<<" ";
                                       }cout<<endl;
                                       cout<<"==============================================================";
                                       cout<<endl;
                                       ::close (back[i][0]);
                                       ::write(back[i-1][1],&bw,sizeof(int));
                                       double *arr=new double[2];
                                       ::close(backinput1[i][1]);
                                       ::read(backinput1[i][0],arr,2*sizeof(double));
                                       ::close(backinput1[i][0]);
                                       ::write(backinput1[i-1][1],arr,2*sizeof(double));
                                       ::close(backinput1[i-1][1]);
                           
//------------------------------------------------------------------------------------------------------------------------------------------//
                                       //--------------------------Again forward signal
                                       int conditionagain=0;
                                       while (conditionagain==0)
                                       {
                                          ::close (fd7[i-1][1]);
                                          ::read(fd7[i-1][0],&conditionagain,sizeof(int));
                                       }
                                       
                                       //----------------------

                                       
                                       double *recievedagain=new double[neurons];
                                       ::close(fd8[i-1][1]);
                                       ::read(fd8[i-1][0],recievedagain,neurons *sizeof(double));
                                       ::close(fd8[i-1][0]);
                                       for(int i2=0;i2<neurons;i2++)
                                       {
                                          cout<<recievedagain[i2]<<" ";
                                       }
                                       cout<<"    "<<"layer = "<<i<<endl;
                                       cout<<"==============================================================";
                                       cout<<endl;
                                       double *resultagain=new double[neurons];

                                    if(s!=n-1)
                                    {
                                          pthread_t **t;
                                          t=new pthread_t*[neurons];
                                          for(int k=0;k<neurons;k++)
                                          {
                                             t[k]=new pthread_t[neurons];
                                          }
                                          double * tr=new double[neurons];
                                          int coun=0;
                                          int count1=0;
                                          for(int j=0;j<neurons;j++)
                                             {
                                                for(int i2=0;i2<neurons;i2++)
                                                {

                                                   hiddendatacalulation * ts=new hiddendatacalulation();
                                                   ts->a=new double**[neurons];
                                                   for(int i33=0;i33<neurons;i33++)
                                                   {
                                                      ts->a[i33]=new double*[neurons];
                                                      for(int i56=0;i56<neurons;i56++)
                                                      {
                                                         ts->a[i33][i56]=new double[neurons];
                                                      }
                                                   }
                                                   //-----------------------Coping array
                                                   for(int i11=0;i11<nooflayer;i11++)
                                                   {
                                                      for(int i22=0;i22<neurons;i22++)
                                                      {
                                                         for(int i33=0;i33<neurons;i33++)
                                                         {
                                                            ts->a[i11][i22][i33]=hiddenlayer[i11][i22][i33];
                                                         }
                                                      }
                                                   }
                                                  
                                                   //-----------------------------
                                                   ts->k=i-1;
                                                   
                                                   ts->resul=resultagain;
                                                   ts->rows=i2;
                                                   ts->cols=j;
                                                   ts->m=recievedagain[coun];
                                                   pthread_create(&t[i2][j],NULL,hiddenthread,(void*)ts);
                                                   coun++;
                                                   if(coun>neurons-1)
                                                   coun=0;
                                                   count1++;
                                                   cout<<" ";
                                                }
                                             }
                                          //-------------------------

                                          //-----------------Assurance of completion of theads
                                       for(int j=0;j<neurons;j++)
                                       {
                                          for(int jj=0;jj<neurons;jj++)
                                          {
                                             pthread_join(t[j][jj],NULL);
                                          }
                                       }
                                          
                                       
                                       
                                       
                                       //-----------------------------------
                                       cout<<endl;
                                       ::write(fd8[i][1],resultagain,neurons *sizeof(double));
                                       ::close(fd8[i][1]);
                                       condition=1;
                                       ::write(fd7[i][1],&condition,sizeof(int));
                                       ::close(fd7[i][1]);
                                    }
                                    else
                                    {
                                       ::write(fd8[i][1],recievedagain,neurons *sizeof(double));
                                       ::close(fd8[i][1]);
                                       //------Signaling forward to next layer
                                       ::close (fd7[i-1][0]);
                                       condition=1;
                                       ::write(fd7[i][1],&condition,sizeof(int));
                                       ::close(fd7[i][1]);
                                    }

               }
               
            }
        }
        std::exit(0);
    }
 }

  for (int i = 0; i < n; i++) {
        waitpid(pid[i], NULL, 0);
  }

}
