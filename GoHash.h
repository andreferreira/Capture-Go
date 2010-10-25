typedef unsigned long long Board;
typedef int node;

#define NOT_FOUND 0xCAFEBABE
#define ASSOCIATIVITY 10
#include "ia.h"

struct GoHashNode{
	Board key1_;
	Board key2_;
	node data_;
	char level_;

	GoHashNode(){ key1_ = key2_ = 0 ; data_ = 0; level_ = -1;}
	GoHashNode(Board max,Board min, int data, int level){ key1_ = max; key2_ = min; data_ = data; level_ = level;}
	//void set(Board max,Board min, int data){ key1_ = max; key2_ = min; data_ = data;  empty_ = false;}
	bool operator==(GoHashNode g2) { return this->key1_ == g2.key1_ && this->key2_ == g2.key2_ && this->level_ == g2.level_; }
	//void operator=(GoHashNode g2) { this->key1_ = g2.key1_ ; this->level_ = g2.level_; this->key2_ = g2.key2_; this->data_ = g2.data_; this->used_ = g2.used_;}
};
class GoHash {
public:
	GoHash(int maxSize = 66000);
	~GoHash();
	void insert(Board max,Board min,node value,int level);
	int get(Board max,Board min,int level);
	//void remove(Board key1,Board key2);
private:
	int key(Board max,Board min);
	int maxSize_;
	GoHashNode(* elements_)[ASSOCIATIVITY];
};

void inicializaRandombits();