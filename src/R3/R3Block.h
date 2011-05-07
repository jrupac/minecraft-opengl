#ifndef __BLOCK_H_
#define __BLOCK_H_

typedef enum {
  DIRT_BLOCK, //0
  AIR_BLOCK, //1
	LEAF_BLOCK, //2
	BRANCH_BLOCK, //3
	STONE_BLOCK, //4
		WATER_BLOCK, //5
} R3BlockType;

class R3Block {
 public:
  R3Block(const R3Box& box, int type);
  
  R3Box& getBox();
  int getBlockType() const;
  int getHealth() const;
  void changeBlock(int newType);
	R3Block *getUpper() const;
	void setUpper(R3Block *up);
  void Draw(void) const;

 public:
  R3Box box;
  int blockType;
  int health;
  bool walkable;
  bool transparent;
	bool gravity;
	R3Block *upper;
	int dx; int dy; int dz;
};

inline R3Box& R3Block::
getBox (void) 
{
  // Get the box for this block
  return box;
}

inline int R3Block::
getBlockType (void) const
{
  return blockType;
}

inline int R3Block::
getHealth (void) const
{
  return health;
}

inline R3Block *R3Block::
getUpper (void) const 
{
	return upper;
}

inline void R3Block::
setUpper (R3Block *up)
{
	upper = up;
}

#endif
