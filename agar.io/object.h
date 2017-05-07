enum IDS{FOOD, PLAYER, ENEMY};

struct MyFood{

    int ID;
    int x;
    int y;
    bool live;

};
struct MyAgar
{
	int ID;
	int x;
	int y;
	bool lives;
	double diameter;
	int score;
    double speed;
    int ForSpeedAgar;
};
struct BadEnemy
{
	int ID;
	int x;
	int y;
	bool live;
	double speed;
	double Ediameter;
	int enemy_dir;
	int targetX;
    int targetY;
    int ForSpeedEnemy;
};
