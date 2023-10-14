import numpy as np

def parallel_weight_Tin(weight_input, CHout, CHin, Ky, Kx, Tout, Tin):
    slice_of_CHin = (CHin + Tin - 1) // Tin
    max_slice_of_CHout = (CHout + Tout - 1) // Tout
    slice_of_Tin_div_Tout = (Tin + Tout - 1) // Tout

    # print("slice_of_CHin=", slice_of_CHin)
    # print("slice_of_CHout=", slice_of_CHout)
    # print("slice_of_Tin_div_Tout=", slice_of_Tin_div_Tout)
    print("Chout=", CHout)
    if CHout%Tout == 0:
        slice_of_CHout = (CHout + Tout - 1) // Tout
    else:
        if (CHout + Tout - 1) // Tout == 1:
            slice_of_CHout = CHout % Tout
        else:
            slice_of_CHout = CHout / Tout

    print("slice_of_CHout=",slice_of_CHout)

    weight_reorg = np.zeros((int(slice_of_CHout*slice_of_CHin*Ky*Kx*Tout*slice_of_Tin_div_Tout*Tout)))

    addr = 0
    for chout in range(max_slice_of_CHout):
        for l in range(slice_of_CHin):
            for ky in range(Ky):
                for kx in range(Kx):
                    for t_chout in range(Tout):
                        for kk in range(slice_of_Tin_div_Tout):
                            for tin_out in range(Tout):
                                if chout * Tout + t_chout < CHout:
                                    if kk*Tout + tin_out < Tin and l * Tin + kk*Tout + tin_out < CHin:
                                        # print("*******",l * Tin + kk*Tout + tin_out)
                                        tp1 = weight_input[chout * Tout + t_chout][l * Tin + kk*Tout + tin_out][ky][kx]
                                    else:
                                        tp1 = 0
                                    weight_reorg[addr] = tp1
                                    addr = addr + 1
                                else:
                                    addr = addr + 0
    # print(weight_reorg.flatten())
    return weight_reorg


def np_to_bin(dat_in, dw):

    count = dat_in.size

    if(dw==2):
        # print(dat_in)
        mod = count % 8
        if (mod):
            dat_out_0 = np.zeros((count + 8 - mod) // 8)
        else:
            dat_out_0 = np.zeros(count // 8)

        print("size of 16 bit:", dat_out_0.size)
        if (mod):
            print("txt行数不是8的倍数，补充", (8 - mod), "行0")
            for i in range(8 - mod):
                dat_in = np.append(dat_in, 0)
            # print(dat_in)

        for i in range(dat_out_0.size):
            a1 = int(dat_in[8 * i])
            if (a1 < 0):
                a1 = 4 + a1
            a1 = a1

            a2 = int(dat_in[8 * i + 1])
            if (a2 < 0):
                a2 = 4 + a2
            a2 = a2 * 4

            a3 = int(dat_in[8 * i + 2])
            if (a3 < 0):
                a3 = 4 + a3
            a3 = a3 * (4 ** 2)

            a4 = int(dat_in[8 * i + 3])
            if (a4 < 0):
                a4 = 4 + a4
            a4 = a4 * (4 ** 3)

            a5 = int(dat_in[8 * i + 4])
            if (a5 < 0):
                a5 = 4 + a5
            a5 = a5 * (4 ** 4)

            a6 = int(dat_in[8 * i + 5])
            if (a6 < 0):
                a6 = 4 + a6
            a6 = a6 * (4 ** 5)

            a7 = int(dat_in[8 * i + 6])
            if (a7 < 0):
                a7 = 4 + a7
            a7 = a7 * (4 ** 6)

            a8 = int(dat_in[8 * i + 7])
            if (a8 < 0):
                a8 = 4 + a8
            a8 = a8 * (4 ** 7)

            b = a1 + a2 + a3 + a4 + a5 + a6 + a7 + a8

            if ((b <= 32767) & (b >= -32768)):
                b = b
            else:
                b = b - 65536
            dat_out_0[i] = b

    elif(dw==4):
        mod = count%4
        if (mod):
            dat_out_0 = np.zeros((count + 4 - mod) // 4)
        else:
            dat_out_0 = np.zeros(count // 4)
        print("size of 16bit:", dat_out_0.size)
        if(mod):
            print("txt行数不是4的倍数，补充", (4-mod),"行0")
            for i in range(4-mod):
                dat_in = np.append(dat_in,0)
            #print(dat_in)

        for i in range(dat_out_0.size):
            a1 = int(dat_in[4 * i])
            if (a1 < 0):
                a1 = 16 + a1
            a1 = a1

            a2 = int(dat_in[4 * i + 1])
            if (a2 < 0):
                a2 = 16 + a2
            a2 = a2 * 16

            a3 = int(dat_in[4 * i + 2])
            if (a3 < 0):
                a3 = 16 + a3
            a3 = a3 * (16 ** 2)

            a4 = int(dat_in[4 * i + 3])
            if (a4 < 0):
                a4 = 16 + a4
            a4 = a4 * (16 ** 3)

            b = a1 + a2 + a3 + a4
            if ((b <= 32767 )& (b >= -32768)):
                b=b
            else:
                b = b - 65536
            dat_out_0[i] = b

    elif(dw==8):
        mod = count%2
        if (mod):
            dat_out_0 = np.zeros((count + 2 - mod) // 2)
        else:
            dat_out_0 = np.zeros(count // 2)
        print("size of 16bit:", dat_out_0.size)
        if(mod):
            print("txt行数不是2的倍数，补充", (2-mod),"行0")
            for i in range(2-mod):
                dat_in = np.append(dat_in,0)
            #print(dat_in)

        for i in range(dat_out_0.size):
            a1 = int(dat_in[2 * i])
            if (a1 < 0):
                a1 = 256 + a1
            a1 = a1

            a2 = int(dat_in[2 * i + 1])
            if (a2 < 0):
                a2 = 256 + a2
            a2 = a2 * 256


            b = a1 + a2
            if ((b <= 32767 )& (b >= -32768)):
                b=b
            else:
                b = b - 65536
            dat_out_0[i] = b
    else:
        print("wt width should be 2/4/8 bit")

    return dat_out_0

def HWIO2OIHW(weight_in_vgg):
    Ky, Kx, CHin, CHout = weight_in_vgg.shape
    new_weight_in_test = np.zeros((CHout, CHin, Ky, Kx))
    for chout in range(CHout):
        for chin in range(CHin):
            for ky in range(Ky):
                for kx in range(Kx):
                    new_weight_in_test[chout][chin][ky][kx] =weight_in_vgg[ky][kx][chin][chout]
    return new_weight_in_test
