#pragma once

#include <queue>
#include "Macro.h"

//二叉树节点类
template <typename T> class BinNode {
public:
//构造函数
    BinNode() : parent(nullptr),lc(nullptr),rc(nullptr),height(0){}
    BinNode(T const& e,BinNode<T>* p = nullptr,BinNode<T>* lc = nullptr,BinNode<T>* rc = nullptr,int h = 0)
        :data(e),parent(p),lc(lc),rc(rc),height(h) {}

//数据成员
    T data; //数据
    BinNode<T>* parent; //父节点
    BinNode<T>* lc; //左孩子
    BinNode<T>* rc; //右孩子
    int height; //高度

//操作接口
    int size(); //统计以当前节点为根的子树的规模
    BinNode<T>* insertAsLC(T const& e); //作为当前节点的左孩子插入二叉树
    BinNode<T>* insertAsRC(T const& e); //作为当前节点的左孩子插入二叉树

//VST为函数对象
    template<typename VST> void traversePre(VST & visit); //先序遍历
    template<typename VST> void traverseIn(VST & visit); //中序遍历
    template<typename VST> void traversePost(VST & visit); //后序遍历
    template<typename VST> void traverseLevel(VST & visit); //层次遍历
};

//implement
//size():统计以当前节点为根的子树的规模
template <typename T>int BinNode<T>::size() {
    int s = 1; //计入自身,同时作为递归基
    if( lc ) s += lc->size(); //递归计数左子树
    if( rc ) s += rc->size(); //递归计数右子树
    return s;
}


//insert:插入左/右节点
template <typename T>BinNode<T>* BinNode<T>::insertAsLC(T const& e)
{ return lc = new BinNode<T>(e,this);} //作为当前节点的左孩子插入二叉树

template <typename T>BinNode<T>* BinNode<T>::insertAsRC(T const& e)
{ return rc = new BinNode<T>(e,this);} //作为当前节点的右孩子插入二叉树


//traverse:遍历
template <typename T>
template<typename VST> 
void BinNode<T>::traversePre(VST& visit) {
    if (!this) return;
    visit(data);
    lc->traversePre(visit);
    rc->traversePre(visit);
}

template <typename T>
template<typename VST> 
void BinNode<T>::traverseIn(VST& visit) {
    if (!this) return;
    lc->traverseIn(visit);
    visit(data);
    rc->traverseIn(visit);
}

template <typename T>
template<typename VST> 
void BinNode<T>::traversePost(VST& visit) {
    if (!this) return;
    lc->traversePost(visit);
    rc->traversePost(visit);
    visit(data);
}

template <typename T>
template<typename VST> 
void BinNode<T>::traverseLevel(VST & visit){
    std::queue<BinNode<T>*> q;
    q.push(this);
    while(!q.empty()){
        BinNode<T>* x = q.front();
        q.pop();
        visit(x->data);
        if(HasLChild(*x)) q.push(x->lc);
        if(HasRChild(*x)) q.push(x->rc);
    }
}

// //测试部分
// #include <iostream>

// template<typename T>
// void PrintToCout(T data) {
//     std::cout << data << " ";
// }

// int main(){
//     // 创建一个二叉树
//     BinNode<int> root(1);
//     BinNode<int>* node2 = root.insertAsLC(2);
//     BinNode<int>* node3 = root.insertAsRC(3);
//     BinNode<int>* node4 = node2->insertAsLC(4);
//     BinNode<int>* node5 = node2->insertAsRC(5);

//     root.traversePre(PrintToCout<int>);
//     root.traverseIn(PrintToCout<int>);
//     root.traversePost(PrintToCout<int>);
//     root.traverseLevel(PrintToCout<int>);

//     return 0;
// }