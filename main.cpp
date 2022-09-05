#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include "ArgumentManager.h"
#include <vector>
using namespace std;

class bNode {
  private:
    int *keys;
    int degree; 
    bNode **C;
    int n;
    bool leaf;

  public:
    bNode(int d);
    void insertNonFull(int k);
    void traverse(ofstream &output);
    void printLevel(bNode*temp,int level, ofstream &output);
    void height(bNode *temp, int height, int &result);
  friend class btree;
};


class btree{
  bNode *root;
  int degree; // minimun degree
  public:
    btree(int d) {
      root = nullptr;
      degree = d;
    }
    void traverse(ofstream &output){
      if(root!=nullptr)
        root->traverse(output);
    }
    void insert(int k);
    void addAtLeaf(bNode* parent, bNode* node, int v);
    void splitChild(bNode* parent,  bNode* node);
    void printLevel(int level, ofstream &output){
        root->printLevel(root,level, output);
    };
    int height(){
      int result;
      root -> height(root, 1, result);
      return result;
    };
    int getheight(bNode *temp);

};

bNode::bNode(int d){
  degree = d;
  leaf = true;        
  keys = new int[degree];
  C = new bNode *[degree+1];
  n = 0;
}

void bNode::traverse(ofstream &output){
  int i;
  for (i = 0; i < n; i++){
  if (leaf == false)
    C[i]->traverse(output);
  cout << keys[i] << " ";
  output << keys[i] << " ";
  }
  if (leaf == false)
    C[i]->traverse(output);
}

void btree::insert(int v){
  int i;
  if(root==nullptr){
    bNode * tmp = new bNode(degree);
    tmp -> keys[0] = v;
    tmp -> n = 1;
    root = tmp;
    return;
  }
  else
    addAtLeaf(nullptr, root, v);
}

void btree::addAtLeaf(bNode* parent, bNode* node, int v){
  if(node->leaf==true){ //find index to add
    int i;
    for (i = 0; i < node->n; i++){
      if (v > node->keys[i] && v < node->keys[i + 1]){
        i++;
        break;
      }
      else if (v < node->keys[0])
        break;
      else
        continue;
    }
    for(int j=node->n; j>i; j--)
      node->keys[j] = node->keys[j-1];
    node->keys[i] = v;
    node->n += 1;
    }
  else{
    int i;
    for (i = 0; i < node->n; i++){
      if (v > node->keys[i] && v < node->keys[i + 1]){
        i++;
        break;
      }
      else if (v < node->keys[0])
        break;
      else
        continue;
    }
    addAtLeaf(node, node->C[i], v);
    }
  if(node->n == degree){ // if leaf is full -> split it
    if(node == root){
      bNode *s = new bNode(degree);
      s->leaf = false;
      s->C[0] = node;
      splitChild(s, node);  
      root = s;
    }
    else
      splitChild(parent, node);
  }
  }       

void bNode::printLevel(bNode*temp, int level, ofstream &output){
  if(level>1 && leaf){
    return;
  }
  else if(level==1){
    for(int i=0;i<n;i++){
      output<<keys[i]<<" ";
      cout<<keys[i]<<" ";
    }
    return;
  }
  else{
    for(int i=0;i<=n;i++){
        C[i]->printLevel(temp,level-1,output);
    }
  }
}

void btree::splitChild(bNode* parent, bNode* node){
  int mid;
  bNode *r = new bNode(degree);
  bNode *y;
  int index=0;
  int rCIndex = 0;
  for (int j = (degree-1)/2+1; j < degree; j++){
    r -> keys[index] = node -> keys[j];
    node -> keys[j] = 0;
    r -> n++;
    node -> n--;
    index ++;    
  }
  if(node->leaf==false){
    for(int i=(degree+1)/2; i<=degree; i++) {
      r->C[rCIndex] = node->C[i];
      node->C[i]=nullptr;
      rCIndex++;
    }
  r->leaf = false;
  }
  
  int rIndex = degree-1;
  while(parent->C[rIndex]!=node){
    parent->C[rIndex+1]=parent->C[rIndex];
    rIndex--;
  }
  parent->C[rIndex+1] = r;

  int j=parent->n;
  int carry = node->keys[(degree-1)/2];
  while(parent-> keys[j-1]>carry && j!=0){
      parent->keys[j]=parent->keys[j-1];
      j--;
  }
  parent->keys[j]=carry;
  parent->n++;
  node->n--;
}

void bNode::height(bNode*temp, int height, int&result){
  if(leaf){
   result = height;
   return;
  }
  C[0] -> height(temp, height+1, result);
}

bool findDuplicateNumber(vector<int> numbers, int value, int size){
	int i;
	for(i = 0; i < size; i++)
    if(numbers[i] == value)
      return true;
  return false;
}

int main(int argc, char* argv[])
{
  ArgumentManager am(argc, argv);
  string infile = am.get("input");
  string outfile = am.get("output");
  string incommand = am.get("command");
  ifstream input(infile);
  ifstream command(incommand);
  ofstream output(outfile);
  
  string line, level, degree;
  int d, l;
  size_t cut;
  size_t start = 0;

  command >> line;
  if ((cut = line.find("Degree=",start)) != string::npos){
    degree = line.substr(cut+7,line.size()-cut-7);
    d = stoi(degree);
    cout << "Degree= " << d << endl;
  }
  btree b(d);
  vector<int> numbers;
  int n;
  while(input >> n){
    if(!findDuplicateNumber(numbers, n, numbers.size()))
      numbers.push_back(n);
  }
  for(int i=0; i<numbers.size(); i++){
    b.insert(numbers[i]);
  }
  int height = b.height();
  cout << "height = " << height << endl;
  
  while (getline(command, line)){
    if(line.length()==0 || line.length()==1)
      continue;
    if((cut = line.find("Level ",start)) != string::npos){
      string level = line.substr(cut+6,line.size()-cut-6);
      int l = stoi(level);
      cout << "Level:" << l << endl;
      if(l>height){
        output << "empty";
        cout << "empty";
      }
      else
        b.printLevel(l,output);
      cout<< endl;
      output << endl;
    }
    else if((cut = line.find("Inorder Traversal",start)) != string::npos){
      b.traverse(output);
      output << endl;
      cout << endl;
    }

  }
  
}
