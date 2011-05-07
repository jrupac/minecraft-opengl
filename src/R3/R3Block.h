#ifndef __BLOCK_H_
#define __BLOCK_H_

typedef enum {
  DIRT_BLOCK,
  AIR_BLOCK,
	LEAF_BLOCK,
	BRANCH_BLOCK,
	WATER_BLOCK
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

 private:
  R3Box box;
  int blockType;
  int health;
  bool walkable;
  bool transparent;
	R3Block *upper; //block above this one
};

inline R3Box& R3Block::
getBox (void) 
{
  // Gets the box for this block
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
