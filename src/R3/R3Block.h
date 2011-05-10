#ifndef __BLOCK_H_
#define __BLOCK_H_

typedef enum {
  DIRT_BLOCK, //0
  AIR_BLOCK, //1
  LEAF_BLOCK, //2
  WOOD_BLOCK, //3
  STONE_BLOCK, //4
  SAND_BLOCK, //5
	OBSIDIAN_BLOCK, //6
	GOLD_BLOCK //7
} R3BlockType;

#define DIRT_HEALTH 3
#define AIR_HEALTH -1
#define LEAF_HEALTH 1
#define WOOD_HEALTH 2
#define STONE_HEALTH 5
#define SAND_HEALTH 1
#define OBSIDIAN_HEALTH 10
#define GOLD_HEALTH 12

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
  void Draw(int face, bool isSelected) const;
  void Draw(int face, bool isSelected, double ratio) const;

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
