class CoordsData
{
public:
    int height;
    int width;
    unsigned int nbTree;

    CoordsData(int h, int w, unsigned int t)
    {
        height = h;
        width = w;
        nbTree = t;
    }
};

class TmnData
{
public:
    int nbupdate; //number of the update (to prevent infinite loops)
    int rotation;
    int position;
    int color;
    int nbReinit;
    int nbFree;

    TmnData(int upd, int r, int p, int c, int reinit, int nbf)
    {
        nbupdate = upd;
        rotation = r;
        position = p;
        color = c;
        nbReinit = reinit;
        nbFree = nbf;
    }
};

class ReinitData
{
public:
    int id;
    int tmn;
    int movement;

    ReinitData(int nbReinit, int t, int mvt)
    {
        id = nbReinit;
        tmn = t;
        movement = mvt;
    }
};

class isFreeData
{
public:
    int id;
    int position;
    int direction;
    int answer; 

    isFreeData(int i, int p, int d)
    {
        id = i;
        position = p;
        direction = d;
        answer = 0; // = NO_ANSWER
    }
    isFreeData(int i, int p, int d, int a)
    {
        id = i;
        position = p;
        direction = d;
        answer = a;
    }
};

class freeAnswer
{
public:
    int direction; //asked direction and position
    int position;

    freeAnswer(int p, int d)
    {
        direction = d;
        position = p;
    }
};

/*
stringstream strstm;
strstm << "Role is BL corner";
scheduler->trace(strstm.str(), module->blockId, GREEN);
*/