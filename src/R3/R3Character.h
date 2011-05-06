

class R3Character{
public:
	R3Character(void);


	int health;
	int maxhealth;
	R3Item item;
	R3Vector velocity;
	R3Point position;
	int number_items[8];
	R3ItemType belt[8];
};