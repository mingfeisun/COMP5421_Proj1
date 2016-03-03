#ifndef TYPEDEF_H
#define TYPEDEF_H

typedef struct pixel{
    int col, row;
    int state;
    double cost[8];
    double t_cost;
    pixel* pre;
}pixel;

typedef struct seed{
    unsigned int x, y;
    pixel** px;
    seed* pre;
}seed;

typedef struct coord{
    short x;
    short y;
}coord;

#endif // TYPEDEF_H
