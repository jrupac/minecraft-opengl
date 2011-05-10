#ifndef __R3ITEM_H__
#define __R3ITEM_H__

typedef enum {
	R3BLOCK_DIRT,
	R3BLOCK_STONE,
	R3BLOCK_WOOD,
	R3BLOCK_SAND,
	R3BLOCK_OBSIDIAN,
	R3HATCHET,
	R3FOOD,
	R3BLOCK_AIR
} R3ItemType;


class R3Item {
  public:
    R3Item(void);
    R3Item(R3ItemType itemtype);
	
  public:
    R3ItemType itemtype;
};

#endif
