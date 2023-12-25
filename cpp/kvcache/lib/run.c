#include <stdio.h>

int run(short* data, short* weight, short* result, int hin, int win, int chin, int chout, int tout, int tinf, int tino) {

    int OH = hin;
    int OW = win;

    int SO_Tino = (chout + tino - 1) / tino;
    int SP_Tout = (OH*OW + tout - 1) / tout;
    int SC_Tinf = ( chin + tinf - 1) / tinf;
    int ST_Tout = ( tino + tout - 1) / tout;
    printf("%d, %d, %d, %d\n", SO_Tino, SP_Tout, SC_Tinf, ST_Tout);

    for (int so = 0; so < SO_Tino; so++)
        for (int sp = 0; sp < SP_Tout; sp++)
            for (int st = 0; st < ST_Tout; st++) {
                int sum[tout][tout];
                for (int i = 0; i < tout; i++)
                    for (int j = 0; j < tout; j++)
                        sum[i][j] = 0;
                for (int sc = 0; sc < SC_Tinf; sc++) 
                    for (int tp = 0; tp < tout; tp++) {
                        int oh = (sp*tout+tp) / OW;
                        int ow = (sp*tout+tp) % OW;
                        for (int to = 0; to < tout; to++) {
                            for (int ti = 0; ti < tinf; ti++) {
                                int tp_dt, tp_wt;
                                int h = oh;
                                int w = ow;
                                int c = sc*tinf+ti;
                                int o = so*ST_Tout*tout+st*tout;
                                int index_dt = sc*hin*win*tinf + h*win*tinf + w*tinf + ti;
                                int index_wt = c*chout + o;
                                if (h < 0 || h >= hin || w < 0 || w >= win)
                                    tp_dt = 0;
                                else
                                    tp_dt = data[index_dt];
                                tp_wt = weight[index_wt];
                                sum[tp][to] += tp_dt * tp_wt;
                            }
                        }
                    }
                for (int tp = 0; tp < tout; tp++) {
                    if (sp*tout+tp < OH*OW) {
                        int oh = (sp*tout+tp) / OW;
                        int ow = (sp*tout+tp) % OW;
                        for (int to = 0; to < tout; to++) {
                            int index_rt = so*OH*OW*tino + oh*OW*tino + ow*tino + st*tout+to;
                            result[index_rt] = sum[tp][to];
                        }
                    }
                }
            }
    return 0;
}

void CONV(short* data, short* weight, short* result, int H, int W, int C, int O, int OH, int OW, int K, int P, int S) {
    for (int o = 0; o < O; o++)
        for (int oh = 0; oh < OH; oh++)
            for (int ow = 0; ow < OW; ow++) {
                result[oh*OW*O+ow*O+o] = 0;
                for (int c = 0; c < C; c++) 
                    for (int y = 0; y < K; y++) 
                        for (int x = 0; x < K; x++) {
                            if (oh*S-P+y < 0 || oh*S-P+y > H-1) {
                                result[oh*OW*O+ow*O+o] += 0;
                            } else if (ow*S-P+x < 0 || ow*S-P+x > W-1) {
                                result[oh*OW*O+ow*O+o] += 0;
                            } else {
                                int index_dt = (oh*S-P+y)*W*C + (ow*S-P+x)*C + c;
                                int index_wt = y*K*C*O + x*C*O + c*O + o;
                                result[oh*OW*O+ow*O+o] += data[index_dt]*weight[index_wt];
                            }
                        }
            }
}

void FC(short* data, short* weight, short* result, int H, int C, int O) {
    for (int h = 0; h < H; h++) {
        for (int o = 0; o < O; o++) {
            result[h*O+o] = 0;
            for (int c = 0; c < C; c++) {
                result[h*O+o] += data[h*C+c] * weight[c*O+o];
            }
        }
    }
}
