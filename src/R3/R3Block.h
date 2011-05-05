


typedef enum {
  DIRT_BLOCK,
  AIR_BLOCK
} R3BlockType;

class R3Block {
 public:
  R3Block(const R3Box& box, int type);//, int x, int y, int z);
  
  R3Box& getBox();
  int getBlockType() const;
  int getHealth() const;
  void changeBlock(int newType);

  //output
  void Draw(void) const;

 private:
  R3Box box;
  int blockType;
  int health;
  bool walkable;
  bool transparent;
  //int x, y, z; // chunk x, y, z coords

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

