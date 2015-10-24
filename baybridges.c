//  baybridges
//  Created by Stephan Warren
//  Copyright (c) 2015 Stephan Warren. All rights reserved.
/*
 BAY BRIDGES
 
 https://www.codeeval.com/open_challenges/109/
 
 
 CHALLENGE DESCRIPTION:
 
 A new technological breakthrough has enabled us to build bridges that can withstand a 9.5 magnitude earthquake for a fraction of the cost. Instead of retrofitting existing bridges which would take decades and cost at least 3x the price we're drafting up a proposal rebuild all of the bay area's bridges more efficiently between strategic coordinates outlined below.
 
 You want to build the bridges as efficiently as possible and connect as many pairs of points as possible with bridges such that no two bridges cross. When connecting points, you can only connect point 1 with another point 1, point 2 with another point 2.
 
 At example given on the map we should connect all the points except points with number 4.
 
 INPUT SAMPLE:
 Your program should accept as its first argument a path to a filename. Input example is the following
 
1: ([37.788353, -122.387695], [37.829853, -122.294312])
2: ([37.429615, -122.087631], [37.487391, -122.018967])
3: ([37.474858, -122.131577], [37.529332, -122.056046])
4: ([37.532599,-122.218094], [37.615863,-122.097244])
5: ([37.516262,-122.198181], [37.653383,-122.151489])
6: ([37.504824,-122.181702], [37.633266,-122.121964])
 Each input line represents a pair of coordinates for each possible bridge.
 
 OUTPUT SAMPLE:
 You should output bridges in ascending order.
 
 1
 2
 3
 5
 6
 (Check lines on the map)

*/

// #define DEBUGPRT
#ifdef DEBUGPRT
#define debugprintf(...) do { fprintf(stderr, __VA_ARGS__); } while (0)
#else
#define debugprintf(...) do {} while (0)
#endif

#define MAXBRIDGES 16


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct { double x; double y; } coord_t;

typedef struct {
    char len;
    char val[MAXBRIDGES];
} array_t;



typedef struct {
    coord_t co[2];
    double slope;
    double offset;
    char vflag;
    char xref[MAXBRIDGES];
//    short id;
    char * name;
} bridge_t;

bridge_t bridge[MAXBRIDGES];
static char bcnt = 0;
static char MAXSOLUTION = 0;



//#if 0
//typedef struct {
//    bridge_t * b;
//    char intersects;
//    char conflicts[MAXBRIDGES];
//} bridge_list_t;
//#endif


#ifdef DEBUGPRT
#define JSCRIPT
#endif

#ifdef JSCRIPT

#define XS 800
#define YS XS
#define FACTOR ((double) XS)
#define XOFF 10
#define YOFF 10

double minx, maxx, miny, maxy;
FILE * js;

int calcx(double x)
{
    char flag = !0;
    static double factor = 0.0;
    
    if(flag) {
        factor = (maxx - minx);
        flag = 0;
    }
    int x1 = ((int) (((maxx - x) * FACTOR) / factor))+ XOFF;
    return(x1);
}
int calcy(double y)
{
    char flag = !0;
    static double factor = 0.0;
    
    if(flag) {
        factor = (maxy - miny);
        flag = 0;
    }
    int y1 = YOFF + YS - (int) (((maxy - y)  * FACTOR) / factor);
    return(y1);
}

///////////////////////////////////////////////////////////////////////////////////////////  drawline
void drawline(char * str, coord_t p1, coord_t p2)
{
    debugprintf("brigdename = %s\n", str);
    debugprintf("JS minx = %lf, maxx = %lf, miny = %lf, maxy = %lf\n", minx, maxx, miny, maxy);
    debugprintf("JS p1.x = %lf, p2.x = %lf, p1.y = %lf, p2.y = %lf\n", p1.x, p2.x, p1.y, p2.y);
    
    int x1 = calcx(p1.x);
    int x2 = calcx(p2.x);
    int y1 = calcy(p1.y);
    int y2 = calcy(p2.y);
    
    debugprintf("JS from (%d, %d)\n", x1, y1);
    debugprintf("JS to   (%d, %d)\n", x2, y2);
    
    fprintf(js, "    context.moveTo(%d, %d);\n", x1, y1);
    fprintf(js, "    context.lineTo(%d, %d);\n", x2, y2);
    fprintf(js, "    context.stroke();\n");
    fprintf(js, "    context.font = \'italic 9pt Calibri\';\n");
    fprintf(js, "    context.fillText(\'%s\', %d, %d);\n", str, (x1 + x2) / 2 + XOFF, (y1 + y2) / 2 + YOFF);
    fprintf(js, "    context.fillText(\'(%lf,%lf)\', %d, %d);\n", p1.x, p1.y, x1 + XOFF, y1 + YOFF);
    fprintf(js, "    context.fillText(\'(%lf,%lf)\', %d, %d);\n", p2.x, p2.y, x2 + XOFF, y2 + YOFF);
}

////////////////////////////////////////////////////////////////////////////////////////////   drawpoint
void drawpoint(coord_t p)
{
    char buf[256];
    sprintf(buf,"    context.fillText(\'(%lf, %lf)\', \%s, \%s);\n", p.x, p.y, "%d", "%d" );
  
    int x = calcx(p.x);
    int y = calcy(p.y);
  
    fprintf(js, "    context.moveTo(%d, %d);\n", x+2, y+2);
    fprintf(js, "    context.lineTo(%d, %d);\n", x-2, y-2);
    fprintf(js, "    context.stroke();\n");
    fprintf(js, "    context.moveTo(%d, %d);\n", x-2, y+2);
    fprintf(js, "    context.lineTo(%d, %d);\n", x+2, y-2);
    fprintf(js, "    context.stroke();\n");
    fprintf(js, "    context.font = \'italic 9pt Calibri\';\n");
    fprintf(js, buf, x + XOFF, y + YOFF);
}


#endif


char isIntersect(bridge_t * b1, bridge_t * b2)
{
    if(b1->slope == b2->slope) return(0);
    // find intersection
   
    coord_t result;
    if(b1->vflag) {
        result.x = b1->co[0].x;
        result.y = b2->slope * result.x + b2->offset;
    }
    else if(b2->vflag) {
        result.x = b2->co[0].x;
        result.y = b1->slope * result.x + b1->offset;
    }
    else {
        double deltaSlope = b2->slope - b1->slope;
        result.x = (b1->offset - b2->offset) / (deltaSlope);
        result.y = ((b2->slope * b1->offset) - (b1->slope * b2->offset)) / (deltaSlope);
    }

    debugprintf("pointset1: (%lf, %lf), (%lf, %lf)\n", b1->co[0].x, b1->co[0].y, b1->co[1].x, b1->co[1].y);
    debugprintf("pointset2: (%lf, %lf), (%lf, %lf)\n", b2->co[0].x, b2->co[0].y, b2->co[1].x, b2->co[1].y);
    debugprintf("intersection = (%lf, %lf)\n", result.x, result.y);
    
    // is intersection between points
    if( (((b1->co[0].x <= result.x) && (result.x <= b1->co[1].x)) ||
         ((b1->co[1].x <= result.x) && (result.x <= b1->co[0].x))) &&
        (((b1->co[0].y <= result.y) && (result.y <= b1->co[1].y)) ||
         ((b1->co[1].y <= result.y) && (result.y <= b1->co[0].y))) &&
        (((b2->co[0].x <= result.x) && (result.x <= b2->co[1].x)) ||
         ((b2->co[1].x <= result.x) && (result.x <= b2->co[0].x))) &&
        (((b2->co[0].y <= result.y) && (result.y <= b2->co[1].y)) ||
         ((b2->co[1].y <= result.y) && (result.y <= b2->co[0].y))))
    {
#ifdef JSCRIPT
//        drawpoint(result);
#endif
        return(1);
    }
    return(0);
}


array_t recurseBridges(array_t b, char c)
{
#ifdef DEBUGPRT
    debugprintf("enter recurse: cnt = %d, blen = %d\n", c, b.len);
    for(char i = 0; i < b.len; i++) {
        debugprintf("    level[%d] = %d\n", i, b.val[i]);
    }
#endif
    // if the cnt is the number of bridges - we are at the end of recursing
    if (c == bcnt) {
        // if the last item makes the number of good bridges larger add to good list
        if(b.len > MAXSOLUTION) {
            debugprintf("Max = %d\n", MAXSOLUTION);
            MAXSOLUTION = b.len;
        }
        // return good list as it
        debugprintf("ret = b\n");
        return b;
    }
    
    array_t res1;
    res1.len = 0;
    // we can return an empty res1 array if
    if(MAXSOLUTION < bcnt + b.len - c) {
        // bump cnt in recursive call
        res1 = recurseBridges(b, c + 1);

#ifdef DEBUGPRT
        debugprintf("after recurse: res = %d\n", res1.len);
        for(char i = 0; i < res1.len; i++) {
            debugprintf("    res1[%d] = %d\n", i, res1.val[i]);
        }
#endif
        // if any of the used bridges cross each other ... ret res1
        for(char i = 0; i < b.len; i++) {
            debugprintf("bcnt = %d, ix = x, i = %d, c = %d\n", bcnt, i, c);
            if(bridge[c].xref[b.val[i]]) {
//                debugprintf("ret = x res1\n");
                return res1;
            }
        }
        
        // if intersection not in unsolved list
        // append c to b array and recurse
        b.val[b.len++] = c;
        array_t res2 = recurseBridges(b, c + 1);
#ifdef DEBUGPRT
        debugprintf("after recurse: res = %d\n", res2.len);
        for(char i = 0; i < res2.len; i++) {
            debugprintf("    res2[%d] = %d\n", i, res2.val[i]);
        }
#endif

        b.len--; // stacked value pops off as junk
        if(res2.len > res1.len) {
//            debugprintf("ret = res2\n");
            return res2;
        }
//        else {
//            debugprintf("ret = res1\n");
//            // return res1
//        }
    }
//    else {
//        debugprintf("ret = (empty) res1\n");
//    }
    
    return res1;
}





int main(int argc, const char * argv[]) {

#ifdef JSCRIPT
    js = fopen("bridgeplot.html", "w");
    fprintf(js, "<!DOCTYPE HTML>\n <html>\n  <head>\n   <style>\n    body {\n   margin: 0px;\n");
    fprintf(js, "      padding: 0px;\n    }\n   </style>\n  </head>   \n  <body>\n");
    fprintf(js, "   <canvas id=\"myCanvas\" width=\"%d\" height=\"%d\"></canvas>\n", XS+200, YS+200);
    fprintf(js, "   <script>\n    var canvas = document.getElementById('myCanvas');\n");
    fprintf(js, "    var context = canvas.getContext('2d');\n");
    fprintf(js, "    context.beginPath();\n");
#endif

    FILE *file = fopen(argv[1], "r");
 
    char line[1024];
    char * ptr;
    MAXSOLUTION = 0;
    bcnt = 0;
    // example:  1: ([37.788353, -122.387695], [37.829853, -122.294312])
    while(fgets(line, 1024, file)) {
        debugprintf("%s", line);
        ptr = line - 1;
        while(*(++ptr) != ':') debugprintf("%p = %c\n", ptr, *ptr);
        *(ptr++) = '\0';
     //   debugprintf("%p = >%s<\n", ptr, ptr);
        bridge[bcnt].name = strdup(line);
        sscanf(ptr, " ([%lf, %lf], [%lf, %lf])",
                &bridge[bcnt].co[0].x, &bridge[bcnt].co[0].y,
                &bridge[bcnt].co[1].x, &bridge[bcnt].co[1].y );

        debugprintf("bridge name = %s, (%lf, %lf), (%lf, %lf)\n", bridge[bcnt].name,
                bridge[bcnt].co[0].x, bridge[bcnt].co[0].y,
                bridge[bcnt].co[1].x, bridge[bcnt].co[1].y
                );
#ifdef JSCRIPT
        if(bcnt == 0) {
            minx = maxx = bridge[bcnt].co[0].x;
            miny = maxy = bridge[bcnt].co[0].y;
        }
        for(short i = 0; i < 2; i++) {
            if(minx > bridge[bcnt].co[i].x) minx = bridge[bcnt].co[i].x;
            if(miny > bridge[bcnt].co[i].y) miny = bridge[bcnt].co[i].y;
            if(maxx < bridge[bcnt].co[i].x) maxx = bridge[bcnt].co[i].x;
            if(maxy < bridge[bcnt].co[i].y) maxy = bridge[bcnt].co[i].y;
        }
#endif
       // bridge[bcnt].id = bcnt;
        if(bridge[bcnt].co[1].x == bridge[bcnt].co[0].x) bridge[bcnt].vflag = 1;
        else {
            bridge[bcnt].slope =
                  (bridge[bcnt].co[1].y - bridge[bcnt].co[0].y) /
                    (bridge[bcnt].co[1].x - bridge[bcnt].co[0].x);
            bridge[bcnt].offset = bridge[bcnt].co[0].y - bridge[bcnt].slope * bridge[bcnt].co[0].x;
            bridge[bcnt].vflag = 0;
        }
        bcnt++;
    } //end of while fgets
    
    debugprintf("bcnt = %d\n", bcnt);
    for(short i = 0; i < bcnt; i++) {
//        bridge[i].xref = (char *) malloc(sizeof(char) * bcnt);
        bridge[i].xref[i] = 0;
    }
    for(short i = 0; i < bcnt - 1; i++) {
        for(short j = i + 1 ; j < bcnt; j++) {
            bridge[i].xref[j] = bridge[j].xref[i] = isIntersect(&bridge[i], &bridge[j]);
            debugprintf("bridge #%d, bridge# %d = %s\n", i, j, bridge[i].xref[j] ? "Yes" : "No");
        }
    }
    
#ifdef DEBUGPRT
    for(short i = 0; i < bcnt; i++) {
        debugprintf("bridge(%s): ", bridge[i].name);
        for(short j = 0 ; j < bcnt; j++) {
            debugprintf("%c", bridge[i].xref[j] + '0');
        }
        debugprintf("%c", '\n');
#ifdef JSCRIPT
        drawline(bridge[i].name, bridge[i].co[0], bridge[i].co[1]);
#endif
    }
#endif
    
#ifdef JSCRIPT
    fprintf(js, "    context.stroke();\n");
    fprintf(js, "   </script>\n  </body>\n </html>\n");
    fclose(js);
#endif
    
//    bridge_list_t * unknown = NULL;
//
//    unknown = (bridge_list_t *) malloc(bcnt * sizeof(bridge_list_t));
//    for(short i = 0; i < bcnt; i++) {
//        unknown[i].b = &bridge[i]; // bridge;
//        unknown[i].intersects = 0;
//        for(short j = 0; j < bcnt; j++)
//            unknown[i].intersects |= bridge[i].xref[j];
//    }
//    
    
    //  remove, good , good cnt, unknown, ucnt

    array_t empty;
    empty.len = 0;
    array_t result = recurseBridges(empty, 0);
    
    
#ifdef DEBUGPRT
   
    debugprintf("Solution:\n");
#endif
    for(char i = 0; i < result.len; i++) {
        debugprintf("Sol %d = %s\n", i, bridge[result.val[i]].name);
        printf("%s\n", bridge[result.val[i]].name);
    }

    return 0;
}
