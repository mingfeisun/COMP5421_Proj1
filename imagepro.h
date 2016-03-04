#ifndef IMAGEPRO_H
#define IMAGEPRO_H
#include <stdlib.h>
#include <string.h>
#include <queue>
#include <QPixmap>
#include <math.h>

#include "typedef.h"
#include "fibheap.h"
#include "fibheapnode.h"

class ImgPro{

private:
    const double max_cost = 1e8;
    int width;
    int height;
    double maxD=0;
    uint depth;
    uint*** im;
    int** cont;
    int** cont_temp;
    int** mask;
    const int lin_x[8]={+1, +1,  0, -1, -1, -1,  0, +1};
    const int lin_y[8]={ 0, -1, -1, -1,  0, +1, +1, +1};

    pixel** px;
    seed* curr_seed;
    seed* first_seed;

    void mapFlood(int x, int y);
    void genPathTree(int init_x, int init_y);
    void mapCost(uint x, uint y, double* cost);
    void genPixelMapCost();
    void traversePath(pixel** curr_px, int x, int y, int** cont_c, bool clear);

public:

    int curr_mode = 0;

    ImgPro(QPixmap* pixmap);
    void dropLastSeed();
    void drawSeed(int i, int j);
    void currPos(int i, int j);
    void closureSeed(int i, int j);
    QPixmap toPixmap();
    QPixmap toMaskPixmap();
    QPixmap toPixelNodePixmap();
    QPixmap toCostGraphPixmap();
    QPixmap toPathTreePixmap();
    QPixmap toMinPathPixmap();
    QPixmap zoomOut();
    QPixmap zoomIn();

};

#endif // IMAGEPRO_H

void ImgPro::mapFlood(int x, int y)
{
    std::queue<coord> wait_que;
    coord node; node.x = x; node.y = y;
    if(x<=0 || y<=0 || x>=width-1 || y>=height-1){
        return;
    }
    wait_que.push(node);
    mask[y][x] = 3;
    while(!wait_que.empty()){
        coord curr_node = wait_que.front();
        wait_que.pop();
        short c_x = curr_node.x; short c_y = curr_node.y;
        mask[c_y][c_x] = 1;

        if(c_x+1 < width && mask[c_y][c_x+1] == 0
                && cont[c_y][c_x+1] == cont[c_y][c_x]){
            coord r_node; r_node.x = c_x+1; r_node.y = c_y;
            wait_que.push(r_node);
            mask[c_y][c_x+1] = 3;
        }
        if(c_y-1 >= 0 && mask[c_y-1][c_x] == 0
                && cont[c_y-1][c_x] == cont[c_y][c_x]){
            coord u_node; u_node.x = c_x; u_node.y = c_y-1;
            wait_que.push(u_node);
            mask[c_y-1][c_x] = 3;
        }
        if(c_x-1 >= 0 && mask[c_y][c_x-1] == 0
                && cont[c_y][c_x-1] == cont[c_y][c_x]){
            coord l_node; l_node.x = c_x-1; l_node.y = c_y;
            wait_que.push(l_node);
            mask[c_y][c_x-1] = 3;
        }
        if(c_y+1 < height && mask[c_y+1][c_x] == 0
                && cont[c_y+1][c_x] == cont[c_y][c_x]){
            coord b_node; b_node.x = c_x; b_node.y = c_y+1;
            wait_que.push(b_node);
            mask[c_y+1][c_x] = 3;
        }
    }
}

ImgPro::ImgPro(QPixmap *pixmap){
    width=pixmap->width();
    height=pixmap->height();
    depth=pixmap->depth();

    im=(uint***)malloc(3*sizeof(uint**));
    im[0]=(uint**)malloc(height*sizeof(uint*));
    im[1]=(uint**)malloc(height*sizeof(uint*));
    im[2]=(uint**)malloc(height*sizeof(uint*));
    px=(pixel**)malloc(height*sizeof(pixel*));
    cont=(int**)malloc(height*sizeof(int*));
    mask=(int**)malloc(height*sizeof(int*));
    cont_temp=(int**)malloc(height*sizeof(int*));

    for(int i=0; i<height; i++){
        im[0][i]=(uint*)malloc(width*sizeof(uint));
        im[1][i]=(uint*)malloc(width*sizeof(uint));
        im[2][i]=(uint*)malloc(width*sizeof(uint));
        cont[i]=(int*)malloc(width*sizeof(int));
        cont_temp[i]=(int*)malloc(width*sizeof(int));
        mask[i] = (int*)malloc(width*sizeof(int));
        memset(mask[i], 0, width*sizeof(int));
        memset(cont[i], 0, width*sizeof(int));
        memset(cont_temp[i], 0, width*sizeof(int));
        px[i]=(pixel*)malloc(width*sizeof(pixel));
    }
    QImage img=pixmap->toImage();

    for(int i=0; i<height; i++){
        for(int j=0; j<width; j++){
            QColor color=img.pixel(j,i);
            im[0][i][j]=color.red();
            im[1][i][j]=color.green();
            im[2][i][j]=color.blue();
        }
    }
    genPixelMapCost();
    curr_seed = (seed*)malloc(sizeof(seed));
    curr_seed->pre=NULL;
    first_seed=curr_seed;
    curr_mode = 0;
}

void ImgPro::mapCost(uint x, uint y, double *cost){
    double temp;
    for(int i=0; i<3; i++){
        temp=1.0*abs(im[i][x-1][y]+im[i][x-1][y+1]-im[i][x+1][y]-im[i][x+1][y+1])/4;
        cost[0]+=temp*temp;
        temp=1.0*abs(im[i][x-1][y-1]+im[i][x][y-1]-im[i][x-1][y+1]-im[i][x][y+1])/4;
        cost[2]+=temp*temp;
        temp=1.0*abs(im[i][x-1][y-1]+im[i][x-1][y]-im[i][x+1][y-1]-im[i][x+1][y])/4;
        cost[4]+=temp*temp;
        temp=1.0*abs(im[i][x][y-1]+im[i][x+1][y-1]-im[i][x][y+1]-im[i][x+1][y+1])/4;
        cost[6]+=temp*temp;
        temp=abs(im[i][x-1][y]-im[i][x][y+1])/sqrt(2.0);
        cost[1]+=temp*temp;
        temp=abs(im[i][x-1][y]-im[i][x][y-1])/sqrt(2.0);
        cost[3]+=temp*temp;
        temp=abs(im[i][x][y-1]-im[i][x+1][y])/sqrt(2.0);
        cost[5]+=temp*temp;
        temp=abs(im[i][x+1][y]-im[i][x][y+1])/sqrt(2.0);
        cost[7]+=temp*temp;
    }
    temp=0;
    for(int i=0; i<8; i++){
        cost[i]=sqrt(cost[i]/3);
        if(maxD<cost[i]){
            maxD=cost[i];
        }
    }
}

void ImgPro::genPixelMapCost(){
    for(int i=1; i<height-1; i++){
        for(int j=1; j<width-1; j++){
            pixel* curr = &px[i][j];
            curr->col=j; curr->row=i;
            curr->steps = 0;
            curr->pre=NULL; curr->state=0;
            curr->t_cost=max_cost;
            memset(curr->cost, 0, sizeof(curr->cost));
            mapCost(i, j, curr->cost);
        }
    }
    for(int i=1; i<height-1; i++){
        for(int j=1; j<width-1; j++){
            pixel* curr = &px[i][j];
            for(int k=0; k<8; k++){
                curr->cost[k] = (maxD-curr->cost[k])
                        *(k%2==0?1:sqrt(2.0));
            }
        }
    }
    for(int i=0; i<height; i++){
        pixel* curr = &px[i][0];
        curr->col=0; curr->row=i;
        curr->steps=0;
        curr->pre=&px[i][1]; curr->state=0;
        curr->t_cost=max_cost;
        for(uint j=0; j<8; j++){
            curr->cost[j]=max_cost;
        }
        curr = &px[i][width-1];
        curr->col=width-1; curr->row=i;
        curr->pre=&px[i][width-2]; curr->state=0;
        curr->t_cost=max_cost;
        for(uint j=0; j<8; j++){
            curr->cost[j]=max_cost;
        }
    }
    for(int i=0; i<width; i++){
        pixel* curr = &px[0][i];
        curr->col=i; curr->row=0;
        curr->steps=0;
        curr->pre=&px[1][i]; curr->state=0;
        curr->t_cost=max_cost;
        for(uint j=0; j<8; j++){
            curr->cost[j]=max_cost;
        }
        curr = &px[height-1][i];
        curr->col=i; curr->row=height-1;
        curr->steps=0;
        curr->pre=&px[height-2][i]; curr->state=0;
        curr->t_cost=max_cost;
        for(uint j=0; j<8; j++){
            curr->cost[j]=max_cost;
        }
    }
}

void ImgPro::genPathTree(int x, int y){
    FibHeap pq;

    FibHeapNode*** fib_nodes = (FibHeapNode***)malloc(height*sizeof(FibHeapNode**));
    for(int i=0; i<height; i++){
        fib_nodes[i] = (FibHeapNode**)malloc(width*sizeof(FibHeapNode*));
    }
    for(int i=0; i<height; i++){
        for(int j=0; j<width; j++){
            fib_nodes[i][j] = new FibHeapNode(&px[i][j]);
        }
    }
    fib_nodes[y][x]->pix->pre = NULL; fib_nodes[y][x]->pix->t_cost = 0;
    pq.Insert(fib_nodes[y][x]);

    long len = pq.GetNumNodes();
    while(len > 0){
        FibHeapNode* min_node = pq.ExtractMin();
        pixel* curr = min_node->pix;
        curr->state=2;
        for(int i=0; i<8; i++){
            if(curr->row + lin_y[i] < 0){
                curr->t_cost = max_cost;
                curr->pre = &px[1][curr->col];
                curr->steps = curr->pre->steps+1;
                continue;
            }
            if(curr->row + lin_y[i] > height-1){
                curr->t_cost = max_cost;
                curr->pre = &px[height-2][curr->col];
                curr->steps = curr->pre->steps+1;
                continue;
            }
            if(curr->col + lin_x[i] < 0){
                curr->t_cost = max_cost;
                curr->pre = &px[curr->row][1];
                curr->steps = curr->pre->steps+1;
                continue;
            }
            if(curr->col + lin_x[i] > width-1){
                curr->t_cost = max_cost;
                curr->pre = &px[curr->row][width-2];
                curr->steps = curr->pre->steps+1;
                continue;
            }

            FibHeapNode* neib_node = fib_nodes[curr->row + lin_y[i]][curr->col + lin_x[i]];
            pixel* neib = neib_node->pix;

            if(neib->state != 2){
                if(neib->state == 0){
                    neib->pre=curr;
                    neib->t_cost = curr->t_cost + curr->cost[i];
                    neib->state = 1;
                    neib->steps=curr->steps+1;
                    pq.Insert(neib_node);
                }
                else{
                    double sum_cost=curr->t_cost + curr->cost[i];
                    if(sum_cost < neib->t_cost){
                        FibHeapNode new_neib_node = *neib_node;
                        new_neib_node.pix->t_cost = sum_cost;
                        new_neib_node.pix->pre = curr;
                        new_neib_node.pix->steps = curr->steps+1;
                        pq.DecreaseKey(neib_node, new_neib_node);
                    }
                }
            }
        }
        len=pq.GetNumNodes();
    }
}

void ImgPro::dropLastSeed(){
    if(curr_seed->pre == NULL){
        curr_seed = (seed*)malloc(sizeof(seed));
        curr_seed->pre=NULL;
        first_seed=curr_seed;
        return;
    }
    curr_seed = curr_seed->pre;
    if(curr_seed->pre == NULL){
        for(int i=0; i<height; i++){
            memset(cont[i], 0, width*sizeof(int));
        }
    }
    else{
        traversePath(curr_seed->pre->px, curr_seed->x, curr_seed->y, cont, true);
    }
}

void ImgPro::drawSeed(int x, int y){
    //x = 110; y=89;
    if(x<0 || y<0 || x>width-1 || y>height-1)
        return;

    curr_seed->x = x;
    curr_seed->y = y;
    curr_seed->px=px;

    genPathTree(x, y);

    seed* new_seed = (seed*)malloc(sizeof(seed));
    new_seed->pre = curr_seed;

    px=(pixel**)malloc(height*sizeof(pixel*));
    for(int i=0; i<height; i++){
        px[i]=(pixel*)malloc(width*sizeof(pixel));
    }
    for(int i=0; i<height; i++){
        for(int j=0; j<width; j++){
            px[i][j].col = j; px[i][j].row = i;
            px[i][j].t_cost = max_cost;
            px[i][j].state = 0;
            for(int k=0; k<8; k++){
                px[i][j].cost[k] = curr_seed->px[i][j].cost[k];
            }
        }
    }
    new_seed->px = px;
    seed* trav_seed = curr_seed;
    curr_seed=new_seed;

    for(int i=0; i<height; i++){
        memset(cont[i], 0, width*sizeof(int));
    }

    while(trav_seed->pre != NULL){
        traversePath(trav_seed->pre->px, trav_seed->x, trav_seed->y, cont, false);
        trav_seed = trav_seed->pre;
    }

}

void ImgPro::traversePath(pixel **curr_px, int x, int y, int** cont_c, bool clear){
    pixel* now_pixel = &curr_px[y][x];
    cont_c[now_pixel->row][now_pixel->col] = clear?0:255;
    while(now_pixel->pre != NULL){
        now_pixel=now_pixel->pre;
        cont_c[now_pixel->row][now_pixel->col] = clear?0:255;
    }
}

void ImgPro::currPos(int x, int y){
    if(x >= width || y >= height || x<=2 || y<=2){
        for(int i=0; i<height; i++){
            memset(cont_temp[i], 0, width*sizeof(int));
        }
        return;
    }
    if(curr_seed->pre == NULL){
        return;
    }
    for(int i=0; i<height; i++){
        memset(cont_temp[i], 0, width*sizeof(int));
    }
   traversePath(curr_seed->pre->px, x, y, cont_temp, false);
}

void ImgPro::closureSeed(int x, int y){
    dropLastSeed();
    drawSeed(x, y);
    drawSeed(first_seed->x, first_seed->y);
}

QPixmap ImgPro::toPixmap(){
    QImage img(width, height, QImage::Format_RGB32);
    for(int i=0; i<height; i++){
        for(int j=0; j<width; j++){
            QColor color(im[0][i][j],im[1][i][j],im[2][i][j]);
            if(cont[i][j] != 0 || cont_temp[i][j] != 0){
                color.setRed(255); color.setGreen(0); color.setBlue(0);
            }
            QPoint p(j,i);
            img.setPixel(p, color.rgb());
        }
    }
    return QPixmap::fromImage(img);
}

QPixmap ImgPro::toMaskPixmap()
{
    curr_mode = 1;
    for(int i=0; i<height; i++){
        memset(mask[i], 0, width*sizeof(int));
    }
    mapFlood(1, 1);
    QImage img(width, height, QImage::Format_RGB32);
    for(int i=0; i<height; i++){
        for(int j=0; j<width; j++){
            QColor color(im[0][i][j],im[1][i][j],im[2][i][j]);
            if(mask[i][j] == 1){
                color.setRed(0); color.setGreen(0); color.setBlue(0);
            }
            QPoint p(j,i);
            img.setPixel(p, color.rgb());
        }
    }
    return QPixmap::fromImage(img);
}

QPixmap ImgPro::toPixelNodePixmap()
{
    QImage img(3*width, 3*height, QImage::Format_RGB32);
    for(int i=0; i<height; i++){
        for(int j=0; j<width; j++){
            QColor color(im[0][i][j],im[1][i][j],im[2][i][j]);
            QPoint p(3*j+1,3*i+1);
            img.setPixel(p, color.rgb());
            for(int k=0; k<8; k++){
                p.setX(3*j+1+lin_x[k]);
                p.setY(3*i+1+lin_y[k]);
                color.setRed(0); color.setGreen(0); color.setBlue(0);
                img.setPixel(p, color.rgb());
            }
        }
    }
    return QPixmap::fromImage(img);
}

QPixmap ImgPro::toCostGraphPixmap()
{
    QImage img(3*width, 3*height, QImage::Format_RGB32);
    for(int i=0; i<height; i++){
        for(int j=0; j<width; j++){
            QColor color(im[0][i][j],im[1][i][j],im[2][i][j]);
            QPoint p(3*j+1,3*i+1);
            img.setPixel(p, color.rgb());
            for(int k=0; k<8; k++){
                p.setX(3*j+1+lin_x[k]);
                p.setY(3*i+1+lin_y[k]);
                double temp = px[i][j].cost[k] == max_cost ? 255 : px[i][j].cost[k];
                color.setRed(temp);
                color.setGreen(temp);
                color.setBlue(temp);
                img.setPixel(p, color.rgb());
            }
        }
    }
    return QPixmap::fromImage(img);
}

QPixmap ImgPro::toPathTreePixmap()
{
    if(curr_seed->pre == NULL){
        return QPixmap();
    }
    pixel** temp_px = curr_seed->pre->px;
    int seed_x = curr_seed->pre->x;
    int seed_y = curr_seed->pre->y;
    QImage img(3*width, 3*height, QImage::Format_RGB32);
    for(int i=1; i<height-1; i++){
        for(int j=1; j<width-1; j++){
            if(i==seed_y && j==seed_x){
                continue;
            }
            //QColor color(im[0][i][j],im[1][i][j],im[2][i][j]);
            QColor color(255,0,0);
            QPoint p(3*j+1,3*i+1);
            img.setPixel(p, color.rgb());
            double temp = 255;
            for(int k=0; k<8; k++){
                p.setX(3*j+1+lin_x[k]);
                p.setY(3*i+1+lin_y[k]);
                //double temp = px[i][j].cost[k] == max_cost ? 255 : px[i][j].cost[k];
                color.setRed(temp);
                color.setGreen(temp);
                color.setBlue(temp);
                img.setPixel(p, color.rgb());
            }
            int diff_x = temp_px[i][j].pre->col - j;
            int diff_y = temp_px[i][j].pre->row - i;
            color.setRed(255);
            color.setGreen(0);
            color.setBlue(0);
            //p.setX(3*j+1); p.setY(3*i+1);
            //img.setPixel(p, color.rgb());
            p.setX(3*j+1+diff_x); p.setY(3*i+1+diff_y);
            img.setPixel(p, color.rgb());
            p.setX(3*j+1+2*diff_x); p.setY(3*i+1+2*diff_y);
            img.setPixel(p, color.rgb());
            //p.setX(3*j+1+3*diff_x); p.setY(3*i+1+3*diff_y);
            //img.setPixel(p, color.rgb());
        }
    }
    return QPixmap::fromImage(img);
}

QPixmap ImgPro::toMinPathPixmap()
{
    if(curr_seed->pre == NULL){
        return QPixmap();
    }
    pixel** temp_px = NULL;
    temp_px = curr_seed->pre->px;
    int seed_x = curr_seed->pre->x;
    int seed_y = curr_seed->pre->y;
    QImage img(3*width, 3*height, QImage::Format_RGB32);
    for(int i=1; i<height-1; i++){
        for(int j=1; j<width-1; j++){
            if(i==seed_y && j==seed_x){
                continue;
            }
            //QColor color(im[0][i][j],im[1][i][j],im[2][i][j]);
            QColor color(255,0,0);
            QPoint p(3*j+1,3*i+1);
            img.setPixel(p, color.rgb());
            double temp = 255;
            for(int k=0; k<8; k++){
                p.setX(3*j+1+lin_x[k]);
                p.setY(3*i+1+lin_y[k]);
                //double temp = px[i][j].cost[k] == max_cost ? 255 : px[i][j].cost[k];
                color.setRed(temp);
                color.setGreen(temp);
                color.setBlue(temp);
                img.setPixel(p, color.rgb());
            }
            int diff_x = temp_px[i][j].pre->col - j;
            int diff_y = temp_px[i][j].pre->row - i;
            color.setRed(255);
            color.setGreen(0);
            color.setBlue(0);
            p.setX(3*j+1+diff_x); p.setY(3*i+1+diff_y);
            img.setPixel(p, color.rgb());
            p.setX(3*j+1+2*diff_x); p.setY(3*i+1+2*diff_y);
            img.setPixel(p, color.rgb());
            if( cont_temp[i][j] != 0){
                color.setRed(0); color.setGreen(0); color.setBlue(0);
                p.setX(3*j); p.setY(3*i);
                img.setPixel(p, color.rgb());
                p.setX(3*j+1); p.setY(3*i+1);
                img.setPixel(p, color.rgb());
                p.setX(3*j+1+diff_x); p.setY(3*i+1+diff_y);
                img.setPixel(p, color.rgb());
                p.setX(3*j+1+2*diff_x); p.setY(3*i+1+2*diff_y);
            }
        }
    }
    return QPixmap::fromImage(img);

}

QPixmap ImgPro::zoomOut()
{
    QImage img(width/2, height/2, QImage::Format_RGB32);
    for(int i=0; i<height/2; i++){
        for(int j=0; j<width/2; j++){
            QColor color(im[0][2*i][2*j],im[1][2*i][2*j],im[2][2*i][2*j]);
            QPoint p(j,i);
            img.setPixel(p, color.rgb());
        }
    }
    return QPixmap::fromImage(img);
}

QPixmap ImgPro::zoomIn()
{
    QImage img(2*width, 2*height, QImage::Format_RGB32);
    for(int i=0; i<2*height; i++){
        for(int j=0; j<2*width; j++){
            QColor color(im[0][i/2][j/2],im[1][i/2][j/2],im[2][i/2][j/2]);
            QPoint p(j,i);
            img.setPixel(p, color.rgb());
        }
    }
    return QPixmap::fromImage(img);
}
