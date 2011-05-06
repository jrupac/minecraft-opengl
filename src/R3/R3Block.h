


typedef enum {
  DIRT_BLOCK,
  AIR_BLOCK,
	LEAF_BLOCK,
	BRANCH_BLOCK,
	WATER_BLOCK
} R3BlockType;

class R3Block {
 public:
  R3Block(const R3Box& box, int type);//, int x, int y, int z);
  
  R3Box& getBox();
  int getBlockType() const;
  int getHealth() const;
  void changeBlock(int newType);
	R3Block *getUpper() const;
	void setUpper(R3Block *up);
  //output
  void Draw(void) const;

 private:
  R3Box box;
  int blockType;
  int health;
  bool walkable;
  bool transparent;
  //int x, y, z; // chunk x, y, z coords
	R3Block *upper; //block above this one
};

//gets the box for this block
inline R3Box& R3Block::
getBox (void) 
{
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
