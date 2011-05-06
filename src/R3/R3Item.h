

typedef enum{
	R3BLOCK_AIR,
	R3BLOCK_DIRT,
	R3BLOCK_BEDROCK,
	R3BLOCK_STONE,
	R3BLOCK_SAND,
	R3STICK,
	R3HATCHET,
	R3TORCH
} R3ItemType;


class R3Item{
public:
	R3Item(void);
	R3Item(R3ItemType itemtype);
	
	/* variables */
	R3ItemType itemtype;
};



