#include<bits/stdc++.h>
#include <fstream>
using namespace std;
map<string , pair<int ,map< string,set<int> > > >AllWords;
set<pair<string,string>>tok;
map<string,bool >save;
string stopwords[18]={"about","above","actually","after",
"again","against","all","almost","also","although","always"
,"am","an","and","any","are","as","at"};
string Query[2000];
map<string ,int >freq;
map<string,double>idf;
map<string,vector<double>>tf;
long double ln[200000];
void Tokenization(string index)
{
    string filename=index+".txt";
    ifstream file(filename);
    string str;
    int num_word=1;
    while(getline(file,str)) // line by line
    {


        string word="";
        for(int i=0;i<str.size();i++)
        {
            if(str[i]!=' ')
            {
                word+=str[i];
            }
            else
            {
                if(word.size())
                {

                      tok.insert({word,index});

                }
                word="";
            }
        }
        if(word.size())
        {

            tok.insert({word,index});

        }

    }
    file.close();
}
void StopWord()
{
    for(int i=0;i<18;i++)
        save[stopwords[i]]=1;

    for(auto term : tok)
    {
        if(save[term.first])
        {
           tok.erase(term);
        }
    }
}
void pos_index()
{
    for(auto term : tok)
    {
      AllWords[term.first].first++;
      string filename=term.second+".txt";
      ifstream file(filename);
      string str;
      int num_word=1;
      while(getline(file,str))
      {
          string word="";
          for(int i=0;i<str.size();i++)
          {
            if(str[i]!=' ')
            {
                word+=str[i];
            }
            else
            {
                if(word.size())
                {

                    if(word==term.first)
                    {
                        AllWords[term.first].second[term.second].insert(num_word);
                    }
                    num_word++;
                }
                word="";
            }

          }
          if(word.size())
          {
               if(word==term.first)
                {
                   AllWords[term.first].second[term.second].insert(num_word);
                }
          }
      }
      file.close();

    }
}
bool findquery(int index,string doc,int last,int len)
{
  if(index==len)return 1;
  auto res=AllWords[Query[index]].second[doc].upper_bound(last);// binary search
  if(res!=AllWords[Query[index]].second[doc].begin()&&
     res==AllWords[Query[index]].second[doc].end())res--;
  if(last+1!=*res)return 0;
  return findquery(index+1,doc,*res,len);

}
vector<int> matchQueury(int len)
{
    vector<int>answers;
    for(int i=1;i<=10;i++)
    {
        string filename="";
        if(i==10)filename="10";
        else filename+=(i+'0');
        for(auto j : AllWords[Query[0]].second[filename])
        {

           bool state=findquery(1,filename,j,len);
           if(state||len==1)
           {
               answers.push_back(i);
               break;
           }
        }


    }
    return answers;
}
void compute_idf()
{
    map<string,int>visted;
    for(auto term : tok)
    {
        if(!visted[term.first])
        {

            double IDF=log10((double)10/(double)AllWords[term.first].first);
            idf[term.first]=IDF;

        }
        visted[term.first]=1;
    }
}
void compute_tf()
{
    map<string,bool>visted;
    for(auto term : tok)
    {
        if(!visted[term.first])
        {
          for(int i=1;i<=10;i++)
          {
              string doc="";
              if(i==10)doc="10";
              else doc+=(i+'0');

              tf[term.first].push_back(AllWords[term.first].second[doc].size());
          }
        }
        visted[term.first]=1;
    }
}
void compute_tfidf()
{
    map<string,int>visted;
    for(auto term : tok)
    {
        if(!visted[term.first]){
        for(int i=0;i<tf[term.first].size();i++)
        {
         if(idf[term.first]&&tf[term.first][i])
         tf[term.first][i]=((double)1+log10(tf[term.first][i]))*idf[term.first];

        }
        }
        visted[term.first]=1;
    }
}
void WeightQueury(vector<int>answer,int len)
{
    map<string,double>wt;
    map<pair<string,int>,double>wtdoc;
    set<string>st;

    for(int i=0;i<len;i++)st.insert(Query[i]);
    double Querylen=0;
    for(auto str:st)
    {
       wt[str]=((double)1+(double)log10((double)freq[str]))*idf[str];
       Querylen+=wt[str]*wt[str];
    }
    Querylen=sqrt(Querylen);
    for(auto str:st)
    {
        if(Querylen)
        wt[str]=wt[str]/Querylen;
    }
    map<string,bool>s;
    for(auto term:tok)
    {
        if(!s[term.first]){
                string str=term.first;
       for(int i=0;i<tf[term.first].size();i++)
       {

                wtdoc[{str,i+1}]=tf[str][i];
                ln[i+1]+=(wtdoc[{str,i+1}]*wtdoc[{str,i+1}]);

               // cout<<ln[i+1]<<endl;

       }
        }
        s[term.first]=1;
    }
    cout<<"-----------------\n";
    cout<<"     len\n";
    for(int i=1;i<=10;i++)
    {
        ln[i]=sqrt(ln[i]);
        cout<<"d"<<i<<" "<<ln[i]<<endl;
    }
    cout<<"--------------------\n";
    cout<<"    Normalized TF.IDF\n";
     s.clear();
     for(auto term:tok)
      {
          if(!s[term.first])
          {
              cout<<term.first<<" ";
             for(int i=1;i<=10;i++)
             {
                 cout<<idf[term.first]/ln[i]<<" ";
             }
             cout<<endl;
          }
          s[term.first]=1;
      }




    for(auto str:st)
    {
       int l=0;
       for(int i=0;i<tf[str].size();i++)
       {
            if(i+1==answer[l])
            {
                if(ln[i+1])
                wtdoc[{str,i+1}]=wtdoc[{str,i+1}]/ln[i+1];
                l++;
            }
       }
    }
    set<pair<double,int>,greater<>>finalweight;
    for(auto i : answer)
    {
        double res=0;
        for(auto str : st)
        {
            res+=wt[str]*wtdoc[{str,i}];
        }
        finalweight.insert({res,i});
    }
    cout<<"------------------\n";
    for(auto i :finalweight)
    {
        cout<<"DOC "<<i.second<<" is = "<<i.first<<endl;
    }

}
int main()
{
    string files[10]={"1","2","3","4","5","6","7","8","9","10"};
    for(int i=0;i<10;i++)Tokenization(files[i]);
    StopWord();
    pos_index();
    compute_idf();
    cout<<"Please Enter query : ";
    string query;
    getline(cin,query);
    string word="";
    int len=0;
    for(int i=0;i<query.size();i++)
    {
        if(query[i]!=' ')
            word+=query[i];
        else
        {
            if(word.size()&&!save[word])
            {
              Query[len++]=word;
              freq[Query[len-1]]++;
            }
            word="";
        }
    }
    if(word.size())Query[len++]=word,freq[Query[len-1]]++;
    vector<int>answers;
    answers=matchQueury(len);
    for(auto i : answers)
            cout<<"Doc num "<<i<<endl;
    if(!answers.size())
            cout<<"Not Found\n";

      compute_tf();
      map<string,bool>s;
      cout<<"---------------------------\n";
      cout<<"          TF\n\n";
      for(auto term:tok)
      {   if(!s[term.first]){
          cout<<term.first<<" ";
          for(auto i: tf[term.first])
          {
              cout<<i<<" ";
          }
          cout<<endl;
        }
        s[term.first]=1;
      }
      cout<<"\n";
      s.clear();
      cout<<"---------------------------\n";
      cout<<"          Wtf(1 + log tf)\n\n";
      for(auto term:tok)
      {   if(!s[term.first]){
          cout<<term.first<<" ";
          for(auto i: tf[term.first])
          {
              if(i)
              cout<<(double)1+log10((double)i)<<" ";
              else cout<<i<<" ";
          }
          cout<<endl;
        }
        s[term.first]=1;
      }
      cout<<"---------------------\n";
      cout<<"    IDF\n";
      s.clear();
      for(auto term:tok)
      {
          if(!s[term.first])
          {
              cout<<term.first<<" "<<AllWords[term.first].first<<" "<<idf[term.first]<<endl;
          }
          s[term.first]=1;
      }
      cout<<"---------------------\n";
      compute_tfidf();
        map<string,int>ss;

      cout<<"      TF.IDf\n";
       cout<<"\n";
      for(auto term :tok)
      {
          if(!ss[term.first]){
          cout<<term.first<<" ";

          for(int i=0;i<tf[term.first].size();i++)
          {
              cout<<tf[term.first][i]<<" ";
          }
          cout<<endl;
          }
          ss[term.first]=1;

      }
      cout<<"\n";
      cout<<"-------------------\n";
      WeightQueury(answers,len);

    return 0;
}
