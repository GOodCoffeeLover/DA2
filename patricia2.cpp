#include<iostream>
#include<fstream>
#include<cstring>
const int MAX_SIZE_STR=257;
struct String{
    int Size=0;
    char Str[MAX_SIZE_STR]={};
};

struct Node{
    unsigned long long Key; // номер, поставленный в соответсвие слову
    String Word; //слово, хранящееся в словаре
    int CheakBit=0; //проверяемый бит
    Node* Ptrs[2]; //ссылки на левые и правые вершины
};
bool Equ(const String& a, const String& b ){
    int i=0;
    if(a.Size!=b.Size){
        return false;
    }
    while(a.Str[i]!='\0' && b.Str[i]!='\0'){
        if(a.Str[i]!=b.Str[i]){
            return false;
        }
        i+=1;
    }
    return true;
}

class TPatricia{
    public:
    TPatricia(){
        Head=NULL;
    }
    ~TPatricia(){
        if(Head != NULL && Head->CheakBit< Head->Ptrs[0]->CheakBit){
            Del(Head->Ptrs[0]);
        }
        delete Head;
        Head=NULL;
    }
    int Insert(String Str, unsigned long long k){
        if(Head==NULL){
            Head=new Node;
            Head->Key=k;
            Head->Word.Size=Str.Size;
            memcpy(Head->Word.Str, Str.Str, MAX_SIZE_STR * sizeof(char));
            Head->CheakBit=-1;
            Head->Ptrs[0]=Head;
            Head->Ptrs[1]=NULL;
            return 1;
        }
        Node* pos_elem=Find(Str);
        if( Equ(pos_elem->Word,Str) ){
            return 0;
        }
        int diff_bit=0;
        while( (diff_bit/8 < Str.Size) && (diff_bit/8 < pos_elem->Word.Size) && (pos_elem->Word.Str[diff_bit/8] == Str.Str[diff_bit/8]) ){
            diff_bit+=8;
        }
        while( (GetBit(pos_elem->Word, diff_bit) == GetBit(Str, diff_bit)) ){
            diff_bit+=1;    
        } 
        Node *cur=Head;
        pos_elem=Head->Ptrs[0];
        while( diff_bit>pos_elem->CheakBit ){
            cur=pos_elem;
            pos_elem=pos_elem->Ptrs[GetBit(Str, cur->CheakBit)];
            if(cur->CheakBit >=pos_elem->CheakBit ){ 
                break;
            }
            
        }
        cur->Ptrs[GetBit(Str, cur->CheakBit)]=new Node;
        cur->Ptrs[GetBit(Str, cur->CheakBit)]->Ptrs[GetBit(Str, diff_bit)] = cur->Ptrs[GetBit(Str, cur->CheakBit)];
        cur->Ptrs[GetBit(Str, cur->CheakBit)]->Ptrs[ GetBit(Str, diff_bit) ^ 1] =pos_elem;
        cur->Ptrs[GetBit(Str, cur->CheakBit)]->CheakBit=diff_bit;
        cur->Ptrs[GetBit(Str, cur->CheakBit)]->Key=k;
        cur->Ptrs[GetBit(Str, cur->CheakBit)]->Word.Size=Str.Size;
        memcpy(cur->Ptrs[GetBit(Str, cur->CheakBit)]->Word.Str, Str.Str, MAX_SIZE_STR * sizeof(char));
        return 1;
    }

    int Remove(String Str){
        Node *cur=Find(Str);
        if(cur==NULL || !Equ(cur->Word, Str)){
            return 0;
        }
        if(cur==Head && Head==Head->Ptrs[0]){
            delete Head;
            Head=NULL; 
            return 1;
        }
        if(cur->Ptrs[GetBit(cur->Word, cur->CheakBit)]==cur){
            Node *father=HasPointr(Head, cur->Word);
            father->Ptrs[GetBit(cur->Word, father->CheakBit)]=cur->Ptrs[GetBit(cur->Word, cur->CheakBit) ^ 1];
            delete cur;
            return 1;
        }
        Node *mving=HasPointr(cur->Ptrs[GetBit(cur->Word, cur->CheakBit)], cur->Word);
        Node *chging=HasPointr(mving, mving->Word);
        Node *father_mving=HasPointr(Head, mving->Word);
        chging->Ptrs[GetBit(mving->Word, chging->CheakBit)]=cur;
        father_mving->Ptrs[GetBit(mving->Word, father_mving->CheakBit)]=mving->Ptrs[GetBit(Str, mving->CheakBit) ^ 1];
        cur->Word.Size=mving->Word.Size;
        memcpy(cur->Word.Str, mving->Word.Str, sizeof(char) * MAX_SIZE_STR);
        cur->Key=mving->Key;
        delete mving;
        return 1;
    }

    Node* Find(String Str){
        if(Head==NULL){
            return NULL;
        }
        Node* cur=Head;
        Node* next=Head->Ptrs[GetBit(Str, Head->CheakBit)];
        while(cur->CheakBit < next->CheakBit){
            cur=next;
            next=next->Ptrs[GetBit(Str, cur->CheakBit)];
        } 
        return next;
    }
    void SaveTrie(String path){
        std::ofstream file(path.Str,std::ios::binary | std::ios::out);
        if(Head!=NULL){
            Save(file, Head);
        }
        file.close();
    }
    void LoadTrie(String path){
        if(Head != NULL && Head->CheakBit < Head->Ptrs[0]->CheakBit){
            Del(Head->Ptrs[0]);
        }
        delete Head;
        Head=NULL;
        std::ifstream file(path.Str,std::ios::binary | std::ios::in);
        
        Load(file);
        file.close();
    }
    private:
    void Load(std::ifstream& f){
        String Word;
        unsigned long long Key=0;
        while(1){ 
            f.read((char*)Word.Str, sizeof(char)*MAX_SIZE_STR);
            f.read((char*)&(Word.Size), sizeof(int));
            f.read((char*)&(Key), sizeof(Key));
            if(f.fail()) return;
            Insert(Word, Key);
        }
    }
    void Save(std::ofstream& f, Node* n){
        f.write((char*)(n->Word).Str, sizeof(char)*MAX_SIZE_STR);
        f.write((char*)&(n->Word.Size), sizeof(int));
        f.write((char*)&(n->Key), sizeof(unsigned long long));
        if(n->Ptrs[0]->CheakBit > n->CheakBit){
            Save(f, n->Ptrs[0]);
        }
        if(n->Ptrs[1]!=NULL && n->Ptrs[1]->CheakBit > n->CheakBit){
            Save(f, n->Ptrs[1]);
        }
        return;
    }
        
    int GetBit(String Str, int bit){
        if( (bit/8 < 0) || (bit/8 >Str.Size) ){
            return 0;
        }
        return (Str.Str[bit/8]>>(7-bit%8))%2;
    }

    Node* HasPointr(Node* a, String s){
        while(!Equ(a->Ptrs[GetBit(s, a->CheakBit)]->Word, s)){
            a=a->Ptrs[GetBit(s, a->CheakBit)];
        }
        return a;
    }

    void Del(Node* a){
        if(a->Ptrs[0]->CheakBit > a->CheakBit){
            Del(a->Ptrs[0]);
        }
        if(a->Ptrs[1]->CheakBit > a->CheakBit){
            Del(a->Ptrs[1]);
        }
        delete a;
    }
    Node* Head;
};
    
int main(){
    std::ios_base::sync_with_stdio(false);
    TPatricia t;
    String Str;
    while(true){
        std::cin>>Str.Str;
        if(!std::cin) break;
        switch(Str.Str[0]){
            case '+': {
                std::cin>>Str.Str;
                unsigned long long Key;
                std::cin>>Key;
                int i=0;
                while(Str.Str[i]!='\0'){
                    if(Str.Str[i]>='A'&&Str.Str[i]<='Z'){
                        Str.Str[i]=Str.Str[i]-'A'+'a';
                    }
                    i+=1;
                }
                Str.Size=i;
                if(t.Insert(Str, Key)){
                    std::cout<<"OK"<<std::endl;
                    break;
                }else{
                    std::cout<<"Exist"<<std::endl;
                    break;
                }
            }
            case '-':{
                std::cin>>Str.Str;
                int i=0;
                while(Str.Str[i]!='\0'){
                    if(Str.Str[i]>='A'&&Str.Str[i]<='Z'){
                        Str.Str[i]=Str.Str[i]-'A'+'a';
                    }
                    i+=1;
                }
                Str.Size=i;
                if(t.Remove(Str)){
                    std::cout<<"OK"<<std::endl;
                }else{
                    std::cout<<"NoSuchWord"<<std::endl;
                }
                break;
            }
            case '!':{
                std::cin>>Str.Str;
                if(Str.Str[0]=='S'){
                    std::cin>>Str.Str;
                    t.SaveTrie(Str);
                    std::cout<<"OK"<<std::endl;
                }else{
                    std::cin>>Str.Str;
                    t.LoadTrie(Str);
                    std::cout<<"OK"<<std::endl;
                }
                break;
            }
            default:{
                int i=0;
                while(Str.Str[i]!='\0'){
                    if(Str.Str[i]>='A'&&Str.Str[i]<='Z'){
                        Str.Str[i]=Str.Str[i]-'A'+'a';
                    }
                    i+=1;
                }
                Str.Size=i;
                Node* f=t.Find(Str);
                if(f==NULL || !Equ(f->Word,Str)){
                    std::cout<<"NoSuchWord"<<std::endl;
                }else{
                    std::cout<<"OK: "<<f->Key<<std::endl;
                }
                break;
            }
        }
    }    
    return 0;
}
