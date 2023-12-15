#pragma once //改为头文件时注意带上此标志

#include "BinNode.h"
#include "..\..\release.h"
#include "Macro.h"
#include <algorithm> 

//二叉树类
template <typename T> class BinTree {
protected:
//数据成员
    int _size; //规模
    BinNode<T>* _root; //根节点

public:
//构造与析构
    BinTree(): _size(0),_root(nullptr){}
    ~BinTree() { if ( 0 < _size ) remove( _root ); } //析构函数

//操作接口
    bool empty() const { return !_root; } //判空
    int size() const { return _size; } //规模
    BinNode<T>* root() const { return _root; } //树根

    BinNode<T>* insert(T const& e); //插入根节点（仅空树）
    BinNode<T>* insert(T const& e,BinNode<T>* x); //e作为节点x的左孩子(原无)插入
    BinNode<T>* insert(BinNode<T>* x,T const& e); //e作为节点x的右孩子(原无)插入
    BinNode<T>* attach(BinTree<T>* ,BinNode<T>* ); //为节点x接入左子树(要求x->lc == NULL)(可能存在内存问题)
    BinNode<T>* attach(BinNode<T>*,BinTree<T>* ); //为节点x接入右子树(要求x->rc == NULL)(可能存在内存问题)
    int remove ( BinNode<T>*x ); //子树删除

    virtual int updateHeight(BinNode<T>* x); //更新节点x的高度
    virtual void updateHeightAbove(BinNode<T>* x); //更新节点x所有祖先的高度

    //遍历
    template <typename VST> //操作器
    void travPost( VST& visit ) { if ( _root ) _root->traversePost( visit ); } //后序遍历
    template <typename VST> //操作器
    void travPre( VST& visit ) { if ( _root ) _root->traversePre( visit ); } //先序遍历
    template <typename VST> //操作器
    void travIn( VST& visit ) { if ( _root ) _root->traverseIn( visit ); } //中序遍历
    template <typename VST> //操作器
    void teavLevel( VST& visit ){if( _root ) _root->traverseLevel( visit );} //层次遍历
};


//Mumber Functions Implementation
template<typename T> int BinTree<T>::updateHeight(BinNode<T>* x){
    if(!x) return -1; //递归基：空树高度为-1,单一节点高度为0
    //递归思路：树高=左右子树最大值+1
    int hl = updateHeight(x->lc); //递归计算左子树高度
    int hr = updateHeight(x->rc); //递归计算右子树高度
    x->height = (1 + std::max(hl,hr)); //更新树高
    return x->height; //返回树高
}

template<typename T> void BinTree<T>::updateHeightAbove(BinNode<T>* x){
    if(!x) return; //递归基
    int oldh = x->height;
    int newh = updateHeight(x); //更新当前节点高度
    if(oldh == newh && oldh != 0) return; //若更新前后高度不变，则后续祖先节点高度也不会改变，直接返回
    updateHeightAbove(x->parent); //递归更新祖先节点
}

template <typename T>BinNode<T>* BinTree<T>::insert(T const& e){//插入根节点（仅空树）
    _size = 1;
    return _root = new BinNode<T>(e);
}

template <typename T>BinNode<T>* BinTree<T>::insert(T const& e,BinNode<T>* x){ //为节点x插入左孩子
    _size++;
    x->insertAsLC(e);
    updateHeightAbove(x);
    return x->lc;
}

template <typename T>BinNode<T>* BinTree<T>::insert(BinNode<T>* x,T const& e){ //为节点x插入右孩子
    _size++;
    x->insertAsRC(e);
    updateHeightAbove(x);
    return x->rc;
}

template <typename T>BinNode<T>* BinTree<T>::attach(BinTree<T>* S,BinNode<T>* x){//将S当作节点x的左子树接入二叉树，S本身置空
   if ( x->lc = S->_root ) x->lc->parent = x; //接入
   _size += S->_size; updateHeightAbove( x ); //更新全树规模与x所有祖先的高度
   S->_root = NULL; S->_size = 0; S = NULL; return x; //释放原树，返回接入位置
}

template <typename T>BinNode<T>* BinTree<T>::attach(BinNode<T>* x,BinTree<T>* S){ //将S当作节点x的右子树接入二叉树，S本身置空
    if ( x->rc = S->_root ) x->rc->parent = x; //接入
    _size += S->_size; updateHeightAbove( x ); //更新全树规模与x所有祖先的高度
    S->_root = NULL; S->_size = 0;S = NULL; return x; //释放原树，返回接入位置
}

template <typename T> //删除二叉树中位置x处的节点及其后代，返回被删除节点的数值
static int removeAt( BinNode<T>* x ) { // assert: x为二叉树中的合法位置
   if ( !x ) return 0; //递归基：空树
   int n = 1 + removeAt( x->lc ) + removeAt( x->rc ); //递归释放左、右子树
   release( x->data ); release( x ); return n; //释放被摘除节点，并返回删除节点总数
} // release()负责释放复杂结构，与算法无直接关系

template <typename T> //删除二叉树中位置x处的节点及其后代，返回被删除节点的数值
int BinTree<T>::remove( BinNode<T>* x ) { // assert: x为二叉树中的合法位置
   FromParentTo( *x ) = NULL; //切断来自父节点的指针
   updateHeightAbove( x->parent ); //更新祖先高度
   int n = removeAt( x ); _size -= n; return n; //删除子树x，更新规模，返回删除节点总数
}





//整体测试部分
// #include <iostream>

// int main() {
//     // 创建一个二叉树对象
//     BinTree<int> tree;

//     // 插入根节点
//     BinNode<int>* root = tree.insert(1);

//     // 插入左孩子和右孩子
//     BinNode<int>* node2 = tree.insert(2, root); // 左孩子
//     BinNode<int>* node3 = tree.insert(root, 3); // 右孩子

//     // 更新节点高度
//     tree.updateHeightAbove(node3);

//     // 打印节点高度以进行验证
//     std::cout << "Node 3 Height: " << node3->height << std::endl; // 应该为 0
//     std::cout << "Node 2 Height: " << node2->height << std::endl; // 应该为 0
//     std::cout << "Root Height: " << root->height << std::endl;     // 应该为 1

//     // 创建一个新的二叉树对象
//     BinTree<int> tree2;

//     // 插入节点到新树
//     BinNode<int>* node4 = tree2.insert(4);
//     BinNode<int>* node5 = tree2.insert(5, node4);

//     // 将 tree2 当作 node2 的左子树接入 tree
//     tree.attach(&tree2, node2);

//     // 更新节点高度
//     tree.updateHeightAbove(node4);

//     // 打印节点高度以进行验证
//     std::cout << "Node 4 Height after attach: " << node4->height << std::endl; // 应该为 1
//     std::cout << "Root Height after attach: " << root->height << std::endl;     // 应该为 3

//     // 删除节点 node5
//     tree.remove(node5);

//     // 更新节点高度
//     tree.updateHeightAbove(node4);

//     // 打印节点高度以进行验证
//     std::cout << "Node 4 Height after removal: " << node4->height << std::endl; // 应该为 0
//     std::cout << "Root Height after removal: " << root->height << std::endl;     // 应该为 2

//     return 0;
// }













//测试updateHeightAbove方法（测试完毕无bug）
// #include <iostream>
// int main() {
//     // 创建一个二叉树对象
//     BinTree<int> tree;

//     // 插入一些节点
//     BinNode<int>* root = tree._root = new BinNode<int>(1); // 根节点
//     BinNode<int>* node2 = root->insertAsLC(2); // 左孩子
//     BinNode<int>* node3 = root->insertAsRC(3); // 右孩子
//     BinNode<int>* node4 = node2->insertAsLC(4); // 左孩子的左孩子
//     BinNode<int>* node5 = node2->insertAsRC(5); // 左孩子的右孩子

//     // 更新节点高度
//     tree.updateHeightAbove(node4);

//     // 打印节点高度以进行验证
//     std::cout << "Node 4 Height: " << node4->height << std::endl; // 应该为 1
//     std::cout << "Node 2 Height: " << node2->height << std::endl; // 应该为 2
//     std::cout << "Root Height: " << root->height << std::endl;     // 应该为 3

//     // 可以继续插入更多节点，然后再次调用 updateHeightAbove 进行测试
//     BinNode<int>* node7 = node5->insertAsRC(7); 
//     BinNode<int>* node8 = node7->insertAsRC(8); 
//     tree.updateHeightAbove(node8);
//     BinNode<int>* node6 = node4->insertAsLC(6); 

//     //打印高度节点验证
//     tree.updateHeightAbove(node6);
//     std::cout << "Node 4 Heiht: " << node4->height << std::endl; // 应该为1
//     std::cout << "Node 2 Heiht: " << node2->height << std::endl; // 应该为3

//     return 0;
// }
