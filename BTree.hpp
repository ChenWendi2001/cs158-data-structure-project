#include "utility.hpp"
#include <functional>
#include <cstddef>
#include <cstring>
#include "exception.hpp"

#define offset int
#define Rank unsigned int
#define m 257//m-tree

int max(int a,int b){return a>b?a:b;}

namespace sjtu {
    template <class Key, class Value>
    class BTree {
    private:
        // Your private members go here
        struct Node{
            offset addr;//地址
            offset prev;
            offset next;//兄弟节点的地址
            Rank keySize;//键值个数
            Rank pointSize;//指针个数
            bool isLeaf;//是否为叶子节点
            char *info;
            int dataSize;
            Node* child[m+2];
            Node(bool is = false):isLeaf(is),keySize(0),pointSize(0),next(-1),prev(-1),addr(0){
                dataSize=max(sizeof(offset),sizeof(Value));
                info = new char[sizeof(Key) * (m+1) + dataSize * (m+1)];
                memset(info,0,sizeof(Key) * (m+1) + dataSize * (m+1));
                memset(child,0,(m+2)*sizeof(Node*));
            }
            Node(const Node &other){
                if(&other==this)return;
                addr=other.addr;
                prev = other.prev;
                next=other.next;
                keySize=other.keySize;
                pointSize=other.pointSize;
                isLeaf=other.isLeaf;
                dataSize=other.dataSize;
                info = new char[sizeof(Key) * (m+1) + dataSize * (m+1)];
                memcpy(info,other.info,sizeof(Key) * (m+1)+ dataSize * (m+1));
                memcpy(child,other.child,(m+2)*sizeof(Node*));
            }
            ~Node(){delete info;}
            Key getKey(int n)const{
                int pos = (n-1)*sizeof(Key)+(n-1)*dataSize;
                char temp[sizeof(Key)];
                memcpy(temp,info+pos,sizeof(Key));
                return *(reinterpret_cast<Key*>(temp));
            }
            void modifyKey(int n,Key ret){
                int pos = (n-1)*sizeof(Key)+(n-1)*dataSize;
                memcpy(info+pos, reinterpret_cast<char*>(&ret),sizeof(Key));
            }
            Value getVal(int n)const{
                int pos = (n)*sizeof(Key)+(n-1)*dataSize;
                char temp[sizeof(Value)];
                memcpy(temp,info+pos,sizeof(Value));
                return *(reinterpret_cast<Value*>(temp));
            }
            void modifyVal(int n,Value val){
                int pos = (n)*sizeof(Key)+(n-1)*dataSize;
                memcpy(info+pos, reinterpret_cast<char*>(&val),sizeof(Value));
            }
            offset getOff(int n)const{
                int pos = (n)*sizeof(Key)+(n-1)*dataSize;
                char temp[sizeof(offset)];
                memcpy(temp,info+pos,sizeof(offset));
                return *(reinterpret_cast<offset*>(temp));
            }
            void modifyOff(int n,offset off){
                int pos = (n)*sizeof(Key)+(n-1)*dataSize;
                memcpy(info+pos, reinterpret_cast<char*>(&off),sizeof(offset));
            }
            Node& operator=(const Node &other){
                addr=other.addr;
                prev = other.prev;
                next=other.next;
                keySize=other.keySize;
                pointSize=other.pointSize;
                isLeaf=other.isLeaf;
                dataSize=other.dataSize;
                info = new char[sizeof(Key) * (m+1) + dataSize * (m+1)];
                memcpy(info,other.info,sizeof(Key) * (m+1)+ dataSize * (m+1));
                memcpy(child,other.child,(m+2)*sizeof(Node*));
            }

            void move(int n){//将第m个Key后的节点信息后移动，以便空出空间插入新信息
                if(keySize==m+1||n==m+1)return;
                int pos = (n-1)*sizeof(Key)+(n-1)*dataSize;
                memmove(info+pos+sizeof(Key)+dataSize,
                        info+pos,
                        (keySize-n+1)*sizeof(Key)+(keySize-n+1)*dataSize);
                memmove(child+(n+1),
                        child+n,
                        (keySize-n+1)*sizeof(Node*));
            }
        };

        int size;//目前所含节点个数
        offset locate;//根节点地址
        char filename[100];//文件名
        FILE *file;//文件指针
        Node* root;
        Node* last;
        bool flagRevise;//树是否被修改

        void write_block(Node &cur) {
            fseek(file, cur.addr, SEEK_SET);
            fwrite(&cur.prev, sizeof(offset), 1, file);
            fwrite(&cur.next, sizeof(offset), 1, file);
            fwrite(&cur.keySize, sizeof(Rank), 1, file);
            fwrite(&cur.pointSize, sizeof(Rank), 1, file);
            fwrite(&cur.isLeaf, sizeof(bool), 1, file);
            //fwrite(cur.child,sizeof(Node*),m+2,file);
            if (cur.isLeaf) {
                fwrite(cur.info, 1, sizeof(Key) * (m+1) + cur.dataSize * (m+1), file);
            }
            else {
                fwrite(cur.info, 1, sizeof(Key) * (m+1) + cur.dataSize * (m+1), file);
            }
        }
        void get_block(offset off, Node &ret) {
            ret.addr = off;
            fseek(file, off, SEEK_SET);
            fread(&ret.prev, sizeof(offset), 1, file);
            fread(&ret.next, sizeof(offset), 1, file);
            fread(&ret.keySize, sizeof(Rank), 1, file);
            fread(&ret.pointSize, sizeof(Rank), 1, file);
            fread(&ret.isLeaf, sizeof(bool), 1, file);
            //fread(ret.child,sizeof(Node*),m+2,file);
            if (ret.isLeaf) {
                fread(ret.info, 1, sizeof(Key) * (m+1) + ret.dataSize * (m+1), file);
            }
            else {
                fread(ret.info, 1, sizeof(Key) * (m+1) + ret.dataSize * (m+1), file);
            }
        }

        void init(){ //更新内存中根节点信息
            fseek(file, 0, SEEK_SET);
            fread(&size,sizeof(int),1,file);
            fread(&locate,sizeof(offset),1,file);
            get_block(locate,*root);
        }

        void update(){//更新外存中的初始信息,只增加不减少
            fseek(file, 0, SEEK_SET);
            fwrite(&size,sizeof(int),1,file);
            fwrite(&locate,sizeof(offset),1,file);
        }
        void getLast(){
            Node* r = root;
            while(!r->isLeaf){
                r->child[r->pointSize]=new Node;
                get_block(r->getOff(r->pointSize),*(r->child[r->pointSize]));
                r=r->child[r->pointSize];
            }
            last=r;
        }

        bool ifExist(const Key &key) {//判断某个键值的元素是否存在
            //init();
            Node* r = root;
            int i;
            while(1){
                i = r->keySize;
                while(i>=2&&r->getKey(i)>key)i--;
                if(r->isLeaf)break;
                if(r->child[i]!=NULL){
                    r = r->child[i];
                }
                else{
                    r->child[i]=new Node;
                    get_block(r->getOff(i),*(r->child[i]));
                    r=r->child[i];
                }
            }
            while(i>=1&&r->getKey(i)>key)i--;
            if(i==0||i>=1&&r->getKey(i)<key)return false;
            else return true;
        }
        offset getAddr(){
            return sizeof(int)+sizeof(offset)+
            (sizeof(offset)*2+sizeof(Rank)*2+sizeof(bool)+sizeof(Key) * (m+1) + max(sizeof(Value),sizeof(offset)) * (m+1))*size;
        }
        void split(Node* father, Node* son,int i){//分裂节点，此时father未满(有小于m个key)，son已满（keySize==m），son是father的第i个指针
            Node*bro=new Node;
            bro->addr = getAddr();
            size++;
            bro->isLeaf=son->isLeaf;
            bro->next = son->next;
            bro->prev = son->addr;
            son->next = bro->addr;
            bro->keySize=son->keySize = son->keySize/2;
            bro->pointSize=son->pointSize = son->pointSize/2;
            memcpy(bro->info,son->info+son->pointSize*(sizeof(Key)+son->dataSize),son->pointSize*(sizeof(Key)+son->dataSize));
            memcpy(bro->child+1,son->child+son->pointSize+1,son->pointSize*sizeof(Node*));
            father->move(i+1);
            father->modifyKey(i+1,bro->getKey(1));
            father->modifyOff(i+1,bro->addr);
            father->child[i+1]=bro;
            father->keySize++;
            father->pointSize++;
            write_block(*father);
            write_block(*son);
            write_block(*bro);
            update();
        }
        int insert(Node* son,const Key &key, const Value &value){
            int i;
            if(son->isLeaf){
                i=son->keySize;
                while(i>=1&&son->getKey(i)>key)i--;
                if(son->pointSize==0){
                    son->modifyVal(1,value);
                    son->modifyKey(1,key);
                    son->pointSize++;
                    son->keySize++;
                }
                else{
                    son->move(i+1);
                    son->modifyVal(i+1,value);
                    son->modifyKey(i+1,key);
                    son->pointSize++;
                    son->keySize++;
                }
                write_block(*son);
                if(son->keySize==m+1)return 1;
                else return 0;
            }
            else{
                i = son->keySize;
                while(i>=2&&son->getKey(i)>key)i--;
                if(son->child[i]==NULL){
                    son->child[i]=new Node;
                    get_block(son->getOff(i),*(son->child[i]));
                }
                if(insert(son->child[i],key,value)) {
                    split(son,son->child[i],i);
                    if(son->keySize==m+1)return 1;
                    else return 0;
                }
                else{
                    return 0;
                }
            }

        }

        void clear(Node* node){
            for(int i = node->pointSize;i>0;i--){
                if(node->child[i]!=NULL)clear(node->child[i]);
            }
            delete node;
        }
    public:
        BTree(const char *fname="MyDatabase") {
            FILE* flag = fopen(fname,"rb");//用于判断改文件是否已经存在
            if(!flag){  //该文件不存在
                strcpy(filename,fname);
                root = new Node(true);//初始化根节点信息
                root->addr=sizeof(int)+sizeof(offset);
                root->pointSize=root->keySize=0;
                size=1;locate = root->addr;
                file=fopen(filename,"wb+");
                fwrite(&size,sizeof(int),1,file);//
                fwrite(&locate,sizeof(offset),1,file);
                write_block(*root);
                flagRevise=true;
            }
            else{
                fclose(flag);
                strcpy(filename,fname);
                file = fopen(filename,"rb+");
                root = new Node(true);
                init();
                flagRevise=false;
                getLast();
            }
        }

        ~BTree() {
            clear(root);
            fclose(file);
        }

        // Clear the BTree
        void clear() {//用wb+模式即可
            flagRevise=true;
            clear(root);
            fclose(file);
            root = new Node(true);//初始化根节点信息
            root->addr=sizeof(int)+sizeof(offset);
            root->pointSize=root->keySize=0;
            size=1;locate = root->addr;
            file=fopen(filename,"wb+");
            fwrite(&size,sizeof(int),1,file);//
            fwrite(&locate,sizeof(offset),1,file);
            write_block(*root);
        }

        bool insert(const Key &key, const Value &value) {
            flagRevise=true;
            //init();
            if(ifExist(key)) return false;
            else{
                if(insert(root,key,value)){
                    //init();
                    Node*newNode=new Node;
                    Node*bro=new Node;
                    bro->addr = getAddr();
                    size++;
                    bro->isLeaf=root->isLeaf;
                    bro->next = root->next;
                    bro->prev=root->addr;
                    root->next = bro->addr;
                    bro->keySize=root->keySize = root->keySize/2;
                    bro->pointSize=root->pointSize = root->pointSize/2;
                    memcpy(bro->info,root->info+root->pointSize*(sizeof(Key)+root->dataSize),
                            root->pointSize*(sizeof(Key)+root->dataSize));
                    newNode->addr=getAddr();//获得新节点地址
                    size++;
                    newNode->keySize=2;
                    newNode->pointSize=2;
                    newNode->modifyOff(1,root->addr);
                    newNode->modifyOff(2,bro->addr);
                    newNode->child[1]=root;
                    newNode->child[2]=bro;
                    newNode->modifyKey(1,root->getKey(1));
                    newNode->modifyKey(2,bro->getKey(1));
                    write_block(*newNode);
                    write_block(*root);
                    write_block(*bro);
                    locate=newNode->addr;
                    root = newNode;
                    update();
                    return true;
                }
                else{
                    return true;
                }
            }
        }

        bool modify(const Key &key, const Value &value) {
            //init();
            Node* r = root;
            int i;
            while(1){
                i = r->keySize;
                while(i>=2&&r->getKey(i)>key)i--;
                if(r->isLeaf)break;
                if(r->child[i]==NULL){
                    r->child[i]=new Node;
                    get_block(r->getOff(i),*(r->child[i]));
                }
                r=r->child[i];
            }
            if(r->getKey(i)<key)return false;
            else {
                r->modifyVal(i,value);
                write_block(r);
                return true;
            }
        }

        Value at(const Key &key) {
            //init();
            Node* r = root;
            int i;
            while(1){
                i = r->keySize;
                while(i>=2&&r->getKey(i)>key)i--;
                if(r->isLeaf)break;
                if(r->child[i]==NULL){
                    r->child[i]=new Node;
                    get_block(r->getOff(i),*(r->child[i]));
                }
                r=r->child[i];
            }
            if(i==0||r->getKey(i)<key)return Value();
            else return r->getVal(i);
        }

        bool erase(const Key &key) {
            
        }
        
        
        class iterator {
        private:
            FILE* file;
            void write_block(Node &cur) {
                fseek(file, cur.addr, SEEK_SET);
                fwrite(&cur.prev, sizeof(offset), 1, file);
                fwrite(&cur.next, sizeof(offset), 1, file);
                fwrite(&cur.keySize, sizeof(Rank), 1, file);
                fwrite(&cur.pointSize, sizeof(Rank), 1, file);
                fwrite(&cur.isLeaf, sizeof(bool), 1, file);
                //fwrite(cur.child,sizeof(Node*),m+2,file);
                if (cur.isLeaf) {
                    fwrite(cur.info, 1, sizeof(Key) * (m+1) + cur.dataSize * (m+1), file);
                }
                else {
                    fwrite(cur.info, 1, sizeof(Key) * (m+1) + cur.dataSize * (m+1), file);
                }
            }
            void get_block(offset off, Node &ret) {
                ret.addr = off;
                fseek(file, off, SEEK_SET);
                fread(&ret.prev, sizeof(offset), 1, file);
                fread(&ret.next, sizeof(offset), 1, file);
                fread(&ret.keySize, sizeof(Rank), 1, file);
                fread(&ret.pointSize, sizeof(Rank), 1, file);
                fread(&ret.isLeaf, sizeof(bool), 1, file);
                //fread(ret.child,sizeof(Node*),m+2,file);
                if (ret.isLeaf) {
                    fread(ret.info, 1, sizeof(Key) * (m+1) + ret.dataSize * (m+1), file);
                }
                else {
                    fread(ret.info, 1, sizeof(Key) * (m+1) + ret.dataSize * (m+1), file);
                }
            }

            // Your private members go here
        public:
            void set(FILE* other){file = other;}
            BTree<Key,Value>::Node* cur;
            int num;

            iterator() {
                cur=new Node;
                num=1;
            }
            iterator(const iterator& other) {
                cur=new Node;
                *cur=*(other.cur);
                num=other.num;
            }
            ~iterator() {
                delete cur;
            }
            // modify by iterator
            bool modify(const Value& value) {
                if(num<=cur->pointSize){
                    cur->modifyKey(num,value);
                    return true;
                }
                else
                    return false;
            }


            Key getKey() const {
                return cur->getKey(num);
            }

            Value getValue() const {
                return cur->getVal(num);
            }

            iterator operator++(int) {
                iterator temp = *this;
                if(num<cur->pointSize){num++;return temp;}
                else if(cur->next==-1){num++;return temp;}
                else{
                    get_block(cur->next,*cur);
                    num=1;
                    return temp;
                }
            }

            iterator& operator++() {
                if(num<cur->pointSize){num++;return *this;}
                else if(cur->next==-1){num++;return *this;}
                else{
                    get_block(cur->next,*cur);
                    num=1;
                    return *this;
                }
            }
            iterator operator--(int) {
                iterator temp = *this;
                if(num>1){num--;return temp;}
                else{
                    get_block(cur->prev,*cur);
                    num=cur->pointSize;
                    return temp;
                }
            }

            iterator& operator--() {
                if(num>1){num--;return *this;}
                else{
                    get_block(cur->prev,*cur);
                    num=cur->pointSize;
                    return *this;
                }
            }

            // Overloaded of operator '==' and '!='
            // Check whether the iterators are same
            bool operator==(const iterator& rhs) const {
                return(cur->addr==rhs.cur->addr&&num==rhs.num);
            }

            bool operator!=(const iterator& rhs) const {
                return!(cur->addr==rhs.cur->addr&&num==rhs.num);
            }
        };
        
        iterator begin() {
            //init();
            iterator a;
            a.set(file);
            *(a.cur) = *root;
            while(!a.cur->isLeaf) get_block(a.cur->getOff(1),*(a.cur));
            return a;
        }
        
        // return an iterator to the end(the next element after the last)
        iterator end() {
            //init();
            iterator a;
            a.set(file);
            if(flagRevise) getLast();
            *(a.cur)=*last;
            a.num=a.cur->pointSize+1;
            return a;
        }

        iterator find(const Key &key) {
            //init();
            iterator a;
            *(a.cur) =*root;
            int i;
            while(1){
                i = a.cur->keySize;
                while(i>=2&&a.cur->getKey(i)>key)i--;
                if(a.cur.isLeaf)break;
                get_block(a.cur->getOff(i),a.cur);
            }
            a.num = i;
            return a;
        }
        
        // return an iterator whose key is the smallest key greater or equal than 'key'
        iterator lower_bound(const Key &key) {
            //init();
            iterator a;
            a.set(file);
            *(a.cur) =*root;
            int i;
            while(1){
                i = a.cur->keySize;
                while(i>=2&&a.cur->getKey(i)>=key)i--;
                if(a.cur->isLeaf)break;
                get_block(a.cur->getOff(i),*(a.cur));
            }
            while(i>=1&&a.cur->getKey(i)>=key)i--;
            a.num = i+1;
            return a;
        }
    };
}  // namespace sjtu

