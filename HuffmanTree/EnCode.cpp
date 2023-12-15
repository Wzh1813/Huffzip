#include <iostream>
#include <string>
#include <fstream>
#include <cstring>
#include <vector>
#include <algorithm>
#include <bitset>
#include <utility>

#include "HuffChar.h"
#include "Macro.h"

void print(HuffForest* forest){ //DEBUG:打印链表用
    HuffForest::iterator ptree; 
    ptree = forest->begin(); 
    while(ptree!=forest->end()){ 
        std::cout << (*ptree)->root()->data.ch << (*ptree)->root()->data.weight << std::endl;
        ptree++;
    }
    std::cout << "链表打印完成" << std::endl;
}

int* statistics (){ //统计字符频次
    int *freq = new int[52]; //存放26个英文字母的大小写出现次数
    char ch = 0;
    memset(freq , 0 , sizeof(int) * 52); //初始化

    std::string filename("sample.txt");//需要打开的文件名的参数初始化一个 ifstream 对象
    std::ifstream input_file(filename); //打开文件
    if (!input_file.is_open()) { //验证文件的打开是否成功
    std::cerr << "Could not open the file - '" << filename << "'" << std::endl;
    return nullptr;
    }

    while(input_file.get(ch)){ //逐字符扫描整篇文档，统计字符频次
        if(0x41<=ch && ch<=0x5A) freq[ch-0x41]++;
        if(0x61<=ch && ch<=0x7A) freq[ch-0x47]++;
    }

    input_file.close(); //关闭io流
    return freq;
}

//初始化Huffman森林
HuffForest* initForest(int* freq){
    HuffForest* forest = new HuffForest; //建立森林
    for(int i=0;i<52;i++){ //为每个字符
        forest->push_back(new HuffTree()); //建立一棵树，并将将字符、频次（权重）
        if(i<=25) forest->back()->insert(HuffChar(0x41+i,freq[i])); //存入其中
        if(i>25) forest->back()->insert(HuffChar(0x61+i-26,freq[i]));
    }
    return forest;
}

//算法核心:MinChar:找出当前森林中权重最小的树
HuffTree* MinChar(HuffForest* forest,HuffTree* x = nullptr){
    HuffForest::iterator ptree; //用迭代器遍历森林
    ptree = forest->begin(); //从第一棵树开始
    HuffTree* mintree = *(forest->begin()); //储存最小树
    if(!x){ //找出最小树
        while(ptree!=forest->end()){ //不断迭代
            if(mintree->root()->data.weight > (*ptree)->root()->data.weight) mintree = *ptree; //遍历过程中，不断更新最小树
            //std::cout << mintree->root()->data.ch << mintree->root()->data.weight << std::endl; //DEBUG:观察最小树的变化
            //std::cout << (*ptree)->root()->data.ch << (*ptree)->root()->data.weight << std::endl; //DEBUG:观察遍历是否正确进行
            ptree++;
        }
    }else{ //找出除x外最小的树
        if(x == mintree) mintree = *(++(std::find(forest->begin(),forest->end(),mintree))); //写的时候掉进大坑：如果此时第一个值（mintree的初始化值）、最小值同时为x，那么这里还是会返回x，没有找到次小的树
        while(ptree!=forest->end()){ //不断迭代
            if((mintree->root()->data.weight > (*ptree)->root()->data.weight) && (*ptree)!=x) mintree = *ptree; 
            //std::cout << mintree->root()->data.ch << mintree->root()->data.weight << std::endl; //DEBUG：观察最小树的变化
            ptree++;
        }
    }
    return mintree;
}

//算法核心:generateTree 按照编码规则,生成Huffman编码树
HuffTree* generateTree(HuffForest* forest){
    int cnt = 0; //DEBUG:计数器，观测第几轮迭代出现问题
    while(1<forest->size()){
        HuffTree* T1 = MinChar(forest); //找出最小树
        HuffTree* T2 = MinChar(forest,T1); //找出次小树
        //std::cout << T1->root()->data.ch << T1->root()->data.weight << ' '; //DEBUG：显示最小树的相关信息
        //std::cout << T2->root()->data.ch << T2->root()->data.weight << std::endl; //DEBUG：显示次小树的相关信息
        HuffTree* S = new HuffTree(); //S用于表示合并后的两棵树
        S->insert(HuffChar('^',(T1->root()->data.weight)+(T2->root()->data.weight))); //S的权重为两棵最小树之和
        S->attach(T1,S->root()); //将最小树接为S的左子树
        S->attach(S->root(),T2); //将次小树接为S的右子树
        forest->push_back(S); //将S接入森林
        forest->erase(std::find(forest->begin(),forest->end(),T1)); //从森林中剔除最小树（注意list里存的是指针，所以只是把指针从list中剔除）
        forest->erase(std::find(forest->begin(),forest->end(),T2)); //从森林中剔除次小树（把指针从list中剔除）
        //std::cout << "第" << cnt << "轮结束" << std::endl; //DEBUG：显示迭代轮次
        //cnt++; //DEBUG
    }
    return forest->front();
}

void generateCT(int length,std::bitset<1000>* code,BinNode<HuffChar>* v, std::vector<std::pair<char,std::string>> *codeorder){ //递归生成编码表
    if(IsLeaf(*v)){ //递归基：当前节点为叶节点
        std::string s = code->to_string(); //先将bitset转为string
        std::reverse(s.begin(), s.end()); //由于bitset的第0个索引是从最右开始的，所以这里转成string需要反转字符串
        //std::cout << v->data.ch << s.substr(0,length) << std::endl; //只需要前length位作为编码即可 (DEBUG)
        codeorder->push_back(std::make_pair(v->data.ch,s.substr(0,length))); //在内存中的编码表

        //保存编码表
        std::ofstream output_file; //初始化一个 ofstream 对象
        output_file.open("coding_table.txt",std::ios::app); //打开文件
        if (!output_file.is_open()) { //验证文件的打开是否成功
            std::cerr << "Could not open the file " << std::endl;
            return;
        }
        //output_file << v->data.ch << ' ' <<s.substr(0,length) << std::endl; //编码输出至文件中
        output_file.close(); //关闭文件

    } //若当前节点非叶节点，则
    if(HasLChild(*v)) //递归左子树寻找叶节点
    { code->reset(length); generateCT(length+1,code,v->lc,codeorder); }
    if(HasRChild(*v)) //递归右子树寻找叶节点
    { code->set(length); generateCT(length+1,code,v->rc,codeorder); }
}

//压缩文件
bool Filezip(std::vector<std::pair<char,std::string>> *codeorder){
    //DEBUG:测试codeorder是否为空
    // int cnt = 1;
    // for(auto iter : *codeorder){
    //     std::cout << cnt++ << iter.first << ' ';
    // }

    std::string filename("sample.txt");//需要打开的文件名的参数初始化一个 ifstream 对象
    std::ifstream input_file(filename); //打开文件
    if (!input_file.is_open()) { //验证文件的打开是否成功
    std::cerr << "Could not open the file - '" << filename << "'" << std::endl;
    return false;
    }

    char buf;
    std::string codestring; //用于保存01编码串
//    int cnt1 = 0;//DEBUG
    while(true){
        if(input_file.peek() == EOF) break; //文件读取完毕则终止循环
        input_file.read(&buf,1); //从文件中读取一个字符
        for(auto iter : *codeorder){ //对于每个读取的字符
            if(buf == iter.first) codestring += iter.second; //查找编码表，将其转换为01串
        }
    }
//    std::cout << "在第" << cnt1 << "轮终止读取源文本"; //DEBUG，截至目前，codestring正确
    input_file.close(); //读取完毕，关闭输入流

    std::ofstream output_file;
    output_file.open("zipped.huf",std::ios::out|std::ios::binary);
    if (!output_file.is_open()) { //验证文件的打开是否成功
        std::cerr << "Could not open the file " << std::endl;
        return 0;
    }
    //以下：将01字符串转换为bit存储
    int cnt = 0; //DEBUG:记录迭代轮次
    while((codestring.size())>=8){
        std::bitset<8> bits(codestring,0,8); //将从第0位开始的往后8个字符转为bit存储
        codestring.erase(0,8); //删除从字符串中第0个字符开始的往后8个字符
        output_file << static_cast<char>(bits.to_ulong());
        cnt++; //DEBUG
    }
//    std::cout << "在第" << cnt << "轮终止"; //DEBUG：迭代轮次显然要更少，因为huff的编码并不都是8位的，而这里一次读8位
    if(!codestring.empty())   //结尾长度不够八位，用0补齐，记录剩余的长度将其放在文件末尾，在读取时转换为下标位置还原本来的字符串
    {
        unsigned long loc = codestring.size();
        std::bitset<8> bits(codestring,0,loc);
        output_file << static_cast<char>(bits.to_ulong());
        output_file << static_cast<char>(loc);
        cnt++; //DEBUG
    }
    std::cout << "在第" << cnt << "轮终止压缩" << std::endl;//DEBUG
    output_file.close();
    return true;
}

//解压文件
bool Fileunzipp(BinNode<HuffChar>* v){ //传入根节点
    std::fstream input_file("zipped.huf",std::ios::in | std::ios::binary);
    if (!input_file.is_open()) { //验证文件的打开是否成功
    std::cerr << "Could not open the file " <<  std::endl;
    return false;
    }

    char buf;
    std::bitset<8> bits;
    std::string codestring; //用于保存从文件中读取的01编码串
    int cnt = 0;//DEBUG:记录迭代轮次
    while(true){
        if(input_file.peek() == EOF) break;
        input_file.read(&buf,1);
        bits = buf; //字符赋值给bitset<8>可直接转化为二进制
        codestring += bits.to_string();
        cnt++; //记录迭代轮次 //DEBUG
    }
    std::cout << "在第" << cnt << "轮终止读取01串" << std::endl;
    input_file.close();

    //DEBUG:测试codestring是否为空
    //for(auto iter : codestring) std::cout << iter;
    
    //处理末尾不够八位的情况
    std::string end = codestring.substr(codestring.size()-16,16); 
    std::bitset<8> loc(end,8,16);  
    unsigned long add = loc.to_ulong();
    end = end.substr(8-add,add);
    codestring.erase(codestring.size()-16,codestring.size());
    codestring += end;

    std::ofstream output_file;
    output_file.open("unzipped.txt",std::ios::app);
    if (!output_file.is_open()) { //验证文件的打开是否成功
        std::cerr << "Could not open the file " << std::endl;
        return 0;
    }

    //沿huffman树路径，解码字符串
    BinNode<HuffChar>* p = v;
    for(auto i : codestring){
        if(i == '0') p = p->lc;
        if(i == '1') p = p->rc;
        if(!(p->lc) && !(p->rc)){
            output_file << p->data.ch;
            p = v;
        }
    }
    output_file.close();
    return true;
}

//测试
int main(){
    int* t = statistics();
    // for(int i=0;i<52;i++){
    //     std::cout << t[i] << std::endl ;
    // }

    HuffForest* x = initForest(t);
    //print(x);
    // HuffTree* y = MinChar(x);
    // HuffTree* z = MinChar(x,y);
    // std::cout << y->root()->data.ch << y->root()->data.weight << std::endl;
    // std::cout << z->root()->data.ch << z->root()->data.weight ;
    HuffTree* y = generateTree(x);
    std::bitset<1000>* code = new std::bitset<1000>;
    std::vector<std::pair<char,std::string>> *codeorder = new std::vector<std::pair<char,std::string>>;
    generateCT(0,code,y->root(),codeorder);
    Filezip(codeorder);
    Fileunzipp(y->root());

    delete t;
    delete x; 
    delete y;
    delete codeorder;
    return 0;
}