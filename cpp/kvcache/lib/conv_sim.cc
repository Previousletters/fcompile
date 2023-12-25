#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int H = 16;
int W = 16;
int C = 16;
int O = 16;

int K = 3;
int P = 1;
int S = 1;

int Tout = 8;
int Tinf = Tout*1;
int Tino = Tout*4;

int OH = (H-K+2*P)/S+1;
int OW = (W-K+2*P)/S+1;

int SO_Tino = (    O + Tino - 1) / Tino;
int SP_Tout = (OH*OW + Tout - 1) / Tout;
int SC_Tinf = (    C + Tinf - 1) / Tinf;
int ST_Tout = ( Tino + Tout - 1) / Tout;

int randomInt(int a, int b) {
    return rand() % (b-a+1) + a;
}

int main(){
    printf("H: %d, W: %d, C: %d, O: %d\n", H, W, C, O);
    printf("K: %d, P: %d, S: %d\n", K, P, S);
    printf("Tinf: %d, Tino: %d, Tout: %d\n", Tinf, Tino, Tout);
    printf("SO_Tino: %d, SP_Tout: %d, SC_Tinf: %d, ST_Tout: %d\n", SO_Tino, SP_Tout, SC_Tinf, ST_Tout);
    printf("OH: %d, OW: %d\n", OH, OW);
    int data[H][W][C];
    int weight[K][K][C][O];
    int output[OH][OW][O];
    int hard_o[OH][OW][O];

    std::cout << "Start random the input datas" << std::endl;
    srand(time(0));
    for (int h = 0; h < H; h++) 
        for (int w = 0; w < W; w++) 
            for (int c = 0; c < C; c++) 
                data[h][w][c] = randomInt(-5, 5);

    for (int o = 0; o < O; o++) 
        for (int c = 0; c < C; c++) 
            for (int y = 0; y < K; y++) 
                for (int x = 0; x < K; x++) 
                    weight[y][x][c][o] =randomInt(-5, 5); 

    std::cout << "Start calculate the soft result" << std::endl;
    for (int o = 0; o < O; o++)
        for (int oh = 0; oh < OH; oh++)
            for (int ow = 0; ow < OW; ow++) {
                output[oh][ow][o] = 0;
                for (int c = 0; c < C; c++) 
                    for (int y = 0; y < K; y++) 
                        for (int x = 0; x < K; x++) {
                            if (oh*S-P+y < 0 || oh*S-P+y > H-1) {
                                output[oh][ow][o] += 0;
                            } else if (ow*S-P+x < 0 || ow*S-P+x > W-1) {
                                output[oh][ow][o] += 0;
                            } else {
                                output[oh][ow][o] += data[oh*S-P+y][ow*S-P+x][c]*weight[y][x][c][o];
                            }
                        }
            }

    std::cout << "Start calculate the hard result" << std::endl;
    for (int so = 0; so < SO_Tino; so++)
    	for (int sp = 0; sp < SP_Tout; sp++)
        	for (int st = 0; st < ST_Tout; st++) {
                int sum[Tout][Tout];
                for (int i = 0; i < Tout; i++)
                    for (int j = 0; j < Tout; j++)
                        sum[i][j] = 0;
                for (int sc = 0; sc < SC_Tinf; sc++) 
                    for (int y = 0; y < K; y++) 
                        for (int x = 0; x < K; x++) 
                            for (int tp = 0; tp < Tout; tp++) {
                                int oh = (sp*Tout+tp) / OW;
                                int ow = (sp*Tout+tp) % OW;
                                for (int to = 0; to < Tout; to++) {
                                    for (int ti = 0; ti < Tinf; ti++) {
                                        int tp_dt, tp_wt;
                                        int c = sc*Tinf+ti;
                                        int o = so*ST_Tout*Tout+st*Tout+to;
                                        if (o < O && c < C) {
                                            int h = oh*S-P+y;
                                            int w = ow*S-P+x;
                                            if (h < 0 || h >= OH || w < 0 || w >= OW) {
                                                tp_dt = 0;
                                            } else {
                                                tp_dt = data[h][w][c];
                                            }
                                            tp_wt = weight[y][x][c][o];
                                        } else {
                                            tp_dt = 0;
                                            tp_wt = 0;
                                        }
                                        sum[tp][to] += tp_dt * tp_wt;
                                    }
                                }
                            }
                for (int tp = 0; tp < Tout; tp++) {
                    if (sp*Tout+tp < OH*OW) {
                        int oh = (sp*Tout+tp) / OW;
                        int ow = (sp*Tout+tp) % OW;
                        for (int to = 0; to < Tout; to++) {
                            if (so*ST_Tout*Tout+st*Tout+to < O)
                                hard_o[oh][ow][so*ST_Tout*Tout+st*Tout+to] = sum[tp][to];
                        }
                    }
                }
            }

    int reshape_dt[SC_Tinf][H][W][Tinf];
    int reshape_wt[SO_Tino][SC_Tinf][K][K][ST_Tout][Tout][Tinf];
    int reshape_rt[SO_Tino][OH][OW][Tino];

    for (int sc = 0; sc < SC_Tinf; sc++)
        for (int h = 0; h < H; h++)
            for (int w = 0; w < W; w++)
                for (int ti = 0; ti < Tinf; ti++) {
                    if (sc*Tinf+ti < C)
                        reshape_dt[sc][h][w][ti] = data[h][w][sc*Tinf+ti];
                    else
                        reshape_dt[sc][h][w][ti] = 0;
                }
    for (int so = 0; so < SO_Tino; so++)
        for (int sc = 0; sc < SC_Tinf; sc++) 
            for (int y = 0; y < K; y++)
                for (int x = 0; x < K; x++)
                    for (int st = 0; st < ST_Tout; st++)
                        for (int to = 0; to < Tout; to++)
                            for (int ti = 0; ti < Tinf; ti++){
                                if (so*ST_Tout*Tout + st*Tout + to < O && sc*Tinf + ti < C)
                                    reshape_wt[so][sc][y][x][st][to][ti] = weight[y][x][sc*Tinf + ti][so*ST_Tout*Tout + st*Tout + to];
                                else
                                    reshape_wt[so][sc][y][x][st][to][ti] = 0;
                            }

    std::cout << "Start calculate the reshaped hard result" << std::endl;
    for (int so = 0; so < SO_Tino; so++)
        for (int sp = 0; sp < SP_Tout; sp++)
            for (int st = 0; st < ST_Tout; st++) {
                int sum[Tout][Tout];
                for (int i = 0; i < Tout; i++)
                    for (int j = 0; j < Tout; j++)
                        sum[i][j] = 0;
                for (int sc = 0; sc < SC_Tinf; sc++) 
                    for (int y = 0; y < K; y++) 
                        for (int x = 0; x < K; x++) 
                            for (int tp = 0; tp < Tout; tp++) {
                                int oh = (sp*Tout+tp) / OW;
                                int ow = (sp*Tout+tp) % OW;
                                for (int to = 0; to < Tout; to++) {
                                    for (int ti = 0; ti < Tinf; ti++) {
                                        int tp_dt, tp_wt;
                                        int h = oh*S-P+y;
                                        int w = ow*S-P+x;
                                        if (h < 0 || h >= OH || w < 0 || w >= OW) {
                                            tp_dt = 0;
                                        } else {
                                            tp_dt = reshape_dt[sc][h][w][ti];
                                        }
                                        tp_wt = reshape_wt[so][sc][y][x][st][to][ti];
                                        sum[tp][to] += tp_dt * tp_wt;
                                    }
                                }
                            }
                for (int tp = 0; tp < Tout; tp++) {
                    if (sp*Tout+tp < OH*OW) {
                        int oh = (sp*Tout+tp) / OW;
                        int ow = (sp*Tout+tp) % OW;
                        for (int to = 0; to < Tout; to++) {
                            reshape_rt[so][oh][ow][st*Tout+to] = sum[tp][to];
                        }
                    }
                }
            }

    std::cout << "Start check the results: Soft & Hard" << std::endl;
    for (int oh = 0; oh < OH; oh++) 
        for (int ow = 0; ow < OW; ow++) 
            for (int o = 0; o < O; o++)  {
                if (hard_o[oh][ow][o] != output[oh][ow][o]) {
                    std::cout << "*Mismatch* : Index [" << oh << ", " << ow << ", " << o << "]; ";
                    std::cout << "Hard: " << hard_o[oh][ow][o] << "; ";
                    std::cout << "Soft: " << output[oh][ow][o] << ";\n";
                }
            }

    std::cout << "Start check the results: Soft & Reshape" << std::endl;
    for (int so = 0; so < SO_Tino; so++)
        for (int oh = 0; oh < OH; oh++)
            for (int ow = 0; ow < OW; ow++)
                for (int ti = 0; ti < Tino; ti++) {
                    if (so*Tino+ti < O) {
                        if (reshape_rt[so][oh][ow][ti] != output[oh][ow][so*Tino+ti])
                            std::cout << "*Mismatch* : Index [" << so << ", " << oh << ", " << ow << ", " << ti << "];\n";
                    } else if (so*Tino+ti >= O) {
                        if (reshape_rt[so][oh][ow][ti] != 0)
                            std::cout << "*Mismatch* : Index [" << so << ", " << oh << ", " << ow << ", " << ti << "];\n";
                    } else {
                        std::cout << "*Error* : Index [" << so << ", " << oh << ", " << ow << ", " << ti << "];\n";
                    }
                }

    std::cout << "Finish simulation!" << std::endl;

    return 0;
}
