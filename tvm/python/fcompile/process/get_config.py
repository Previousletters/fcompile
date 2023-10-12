pool_reg_bias = 128
POOL_1D_DAT_IN_DEP = 16
POOL_2D_DAT_IN_DEP = 32
MAX_WT_DW = 8
BRAM_NUM = 16
Total_BUF = 2 ** 24
mp_mode = 0  # mode0 = Booth; mode1 = HPS; mode2 = BSC; mode3 = LPC


def get_conv_cfg(str, Hin, Win, CHin, CHout, Kx, Ky, Sx, Sy, Px, Py, data_width, Tin, Tout):
    print("*********************************************")
    print("start %s" % str)
    if mp_mode == 0 or mp_mode == 1:
        BRAM_WIDTH = Tin * MAX_WT_DW
        BRAM_DEPTH = int((Total_BUF) / BRAM_WIDTH / BRAM_NUM)
    elif mp_mode == 2:  # BSC
        BRAM_WIDTH = Tin * MAX_WT_DW * 2
        BRAM_DEPTH = int((Total_BUF) / BRAM_WIDTH / BRAM_NUM)
    else:  # LPC
        BRAM_WIDTH = Tin * MAX_WT_DW * 4
        BRAM_DEPTH = int((Total_BUF) / BRAM_WIDTH / BRAM_NUM)
    conv_cfg = {}

    best_dat_banks = 0
    best_wt_banks = 0
    best_CHout_Split_Times = 0
    best_out_ch_slice = 0
    best_out_ch_slice_last = 0
    best_Hout_Split_Times = 0
    best_in_height_first = 0
    best_in_height_middle = 0
    best_in_height_last = 0
    best_out_height_first = 0
    best_out_height_middle = 0
    best_out_height_last = 0
    best_total_bw_if_reuse_wt = 0
    best_total_bw_if_reuse_dat = 0
    best_method = 0
    mininum_bw = 0

    wt_factor = int(MAX_WT_DW / data_width)  # 8bit: wt_factor = 1, 2bit: wt_factor = 4

    if mp_mode == 0 or mp_mode == 1:
        Tin_L = Tin * MAX_WT_DW / data_width
    elif mp_mode == 2:
        if data_width == 8:
            Tin_L = Tin
        else:
            Tin_L = Tin * 2 * MAX_WT_DW / data_width
    else:
        Tin_L = Tin * int(pow(2, (MAX_WT_DW / data_width)))

    out_width = ((Win + 2 * Px - Kx) // Sx + 1)
    out_height = ((Hin + 2 * Py - Ky) // Sy + 1)
    overlap = Ky - Sy
    dat_num_per_row = Win * ((CHin + Tin_L - 1) // Tin_L)

    print("dat_num_per_row = ", dat_num_per_row)

    dat_banks_min = (dat_num_per_row * Ky + BRAM_DEPTH - 1) // BRAM_DEPTH
    wt_banks_min = (Kx * Ky * Tout * ((CHin + Tin_L - 1) // Tin_L) + BRAM_DEPTH - 1) // BRAM_DEPTH
    print("dat = ", dat_num_per_row * Ky)
    print("dat_banks_min = ", dat_banks_min)
    print("wt = ", Kx * Ky * Tout * ((CHin + Tin_L - 1) // Tin_L))
    print("wt_banks_min = ", wt_banks_min)

    if (dat_banks_min + wt_banks_min) > BRAM_NUM:
        print("--------------------------")
        print("Error:  BUF  not enough!!!")
        print("--------------------------")
        return "error"

    for dat_buf_num in range(int(dat_banks_min), int(BRAM_NUM - wt_banks_min + 1)):

        wt_banks = BRAM_NUM - dat_buf_num
        out_ch_slice = ((BRAM_DEPTH * wt_banks) // (Kx * Ky * Tout * ((CHin + Tin_L - 1) // Tin_L))) * Tout

        print("dat_buf_num = ", dat_buf_num)
        print("wt_banks = ", wt_banks)
        print("out_ch_slice = ", out_ch_slice)

        if (out_ch_slice >= CHout):
            out_ch_slice = CHout
            CHout_Split_Times = 1
        else:
            CHout_Split_Times = (CHout + out_ch_slice - 1) // out_ch_slice

        if (CHout % out_ch_slice == 0):
            out_ch_slice_last = out_ch_slice
        else:
            out_ch_slice_last = CHout % out_ch_slice

        out_height_first = ((BRAM_DEPTH * dat_buf_num) // dat_num_per_row + Py - Ky) // Sy + 1
        in_height_first = (out_height_first - 1) * Sy + Ky - Py

        out_height_middle = ((BRAM_DEPTH * dat_buf_num) // dat_num_per_row - Ky) // Sy + 1
        in_height_middle = (out_height_middle - 1) * Sy + Ky

        if (out_height_first >= out_height):
            out_height_first = out_height
            in_height_first = Hin

        if ((out_height - out_height_first) % out_height_middle == 0):
            Hout_Split_Times = (out_height - out_height_first) // out_height_middle + 1
            out_height_last = out_height_middle

        else:
            Hout_Split_Times = (out_height - out_height_first) // out_height_middle + 2
            out_height_last = (out_height - out_height_first) % out_height_middle

        in_height_last = Hin - in_height_first + overlap - (Hout_Split_Times - 2) * (in_height_middle - overlap)

        print("CHout_Split_Times = ", CHout_Split_Times)
        print("out_ch_slice = ", out_ch_slice)
        print("out_ch_slice_last = ", out_ch_slice_last)

        print("Hout_Split_Times = ", Hout_Split_Times)
        print("in_height_first = ", in_height_first)
        print("in_height_middle = ", in_height_middle)
        print("in_height_last = ", in_height_last)
        print("out_height_first = ", out_height_first)
        print("out_height_middle = ", out_height_middle)
        print("out_height_last = ", out_height_last)

        total_bw_if_reuse_wt = (dat_num_per_row * Hin + dat_num_per_row * overlap * (
                Hout_Split_Times - 1)) * CHout_Split_Times + Kx * Ky * CHout * (
                                       (CHin + Tin_L - 1) // Tin_L) / wt_factor
        total_bw_if_reuse_dat = Hout_Split_Times * Kx * Ky * CHout * (
                (CHin + Tin_L - 1) // Tin_L) / wt_factor + dat_num_per_row * Hin + dat_num_per_row * overlap * (
                                        Hout_Split_Times - 1)
        print("               total_bw_if_reuse_wt = ", total_bw_if_reuse_wt)
        print("               total_bw_if_reuse_dat = ", total_bw_if_reuse_dat)


        if ((mininum_bw == 0) or (total_bw_if_reuse_wt < mininum_bw) or (total_bw_if_reuse_dat < mininum_bw)):
            best_CHout_Split_Times = CHout_Split_Times
            best_out_ch_slice = out_ch_slice
            best_out_ch_slice_last = out_ch_slice_last
            best_Hout_Split_Times = Hout_Split_Times
            best_in_height_first = in_height_first
            best_in_height_middle = in_height_middle
            best_in_height_last = in_height_last
            best_out_height_first = out_height_first
            best_out_height_middle = out_height_middle
            best_out_height_last = out_height_last
            best_total_bw_if_reuse_wt = total_bw_if_reuse_wt
            best_total_bw_if_reuse_dat = total_bw_if_reuse_dat

        if (mininum_bw == 0):
            if total_bw_if_reuse_wt < total_bw_if_reuse_dat:
                best_dat_banks = dat_buf_num
                best_wt_banks = BRAM_NUM - dat_buf_num
                mininum_bw = total_bw_if_reuse_wt
                best_method = 0
            else:
                best_dat_banks = dat_buf_num
                best_wt_banks = BRAM_NUM - dat_buf_num
                mininum_bw = total_bw_if_reuse_dat
                best_method = 1

        else:
            if total_bw_if_reuse_wt < mininum_bw:
                best_dat_banks = dat_buf_num
                best_wt_banks = BRAM_NUM - dat_buf_num
                mininum_bw = total_bw_if_reuse_wt
                best_method = 0
            if total_bw_if_reuse_dat < mininum_bw :
                best_dat_banks = dat_buf_num
                best_wt_banks = BRAM_NUM - dat_buf_num
                mininum_bw = total_bw_if_reuse_dat
                best_method = 1
        print(" -------------------")


    print("**** final result *****")
    print("conclusion of %s" % str)
    print("   ")
    print("wt_banks = ", best_wt_banks)
    print("dat_buf_num = ", best_dat_banks)
    print("CHout_Split_Times = ", best_CHout_Split_Times)
    print("out_ch_slice = ", best_out_ch_slice)
    print("out_ch_slice_last = ", best_out_ch_slice_last)

    print("Hout_Split_Times = ", best_Hout_Split_Times)
    print("in_height_first = ", best_in_height_first)
    print("in_height_middle = ", best_in_height_middle)
    print("in_height_last = ", best_in_height_last)
    print("out_height_first = ", best_out_height_first)
    print("out_height_middle = ", best_out_height_middle)
    print("out_height_last = ", best_out_height_last)
    print("total_bw_if_reuse_wt = ", best_total_bw_if_reuse_wt)
    print("total_bw_if_reuse_dat = ", best_total_bw_if_reuse_dat)

    print("best_method = ", best_method)
    if best_method == 0:
        total_bw = best_total_bw_if_reuse_wt
    else:
        total_bw = best_total_bw_if_reuse_dat

    # conv_cfg = {'CHin': 0, 'Win': 0, 'CHout': 0, 'overlap': 0,
    #             'Kx': 0, 'Ky': 0, 'Sx': 0, 'Sy': 0, 'pad_x': 0, 'pad_y': 0,
    #             'dat_banks': 0, 'method': 0, 'out_width': 0, 'out_height': 0, 'dat_num_per_row': 0, 'wt_size': 0,
    #             'Hout_Split_Times': 0, 'in_height_first': 0, 'in_height_middle': 0, 'in_height_last': 0,
    #             'out_height_first': 0, 'out_height_middle': 0, 'out_height_last': 0,
    #             'CHout_Split_Times': 0, 'out_ch_slice': 0, 'out_ch_slice_last': 0}
    # conv_cfg['mininum_bw'] = mininum_bw
    conv_cfg['CHin'] = int(CHin)
    conv_cfg['Tin_L'] = int(Tin_L)
    conv_cfg['Win'] = int(Win)
    conv_cfg['CHout'] = int(CHout)
    conv_cfg['overlap'] = int(overlap)
    conv_cfg['Kx'] = int(Kx)
    conv_cfg['Ky'] = int(Ky)
    conv_cfg['Sx'] = int(Sx)
    conv_cfg['Sy'] = int(Sy)
    conv_cfg['pad_x'] = int(Px)
    conv_cfg['pad_y'] = int(Py)
    conv_cfg['dat_banks'] = int(best_dat_banks)
    conv_cfg['method'] = int(best_method)
    conv_cfg['out_width'] = int(out_width)
    conv_cfg['out_height'] = int(out_height)
    conv_cfg['dat_num_per_row'] = int(dat_num_per_row)
    conv_cfg['wt_size_in_bytes'] = int((Tin_L * data_width / 8) * Kx * Ky * CHout * ((CHin + Tin_L - 1) // Tin_L))
    conv_cfg['Hout_Split_Times'] = int(best_Hout_Split_Times)
    conv_cfg['in_height_first'] = int(best_in_height_first)
    conv_cfg['in_height_middle'] = int(best_in_height_middle)
    conv_cfg['in_height_last'] = int(best_in_height_last)
    conv_cfg['out_height_first'] = int(best_out_height_first)
    conv_cfg['out_height_middle'] = int(best_out_height_middle)
    conv_cfg['out_height_last'] = int(best_out_height_last)
    conv_cfg['CHout_Split_Times'] = int(best_CHout_Split_Times)
    conv_cfg['out_ch_slice'] = int(best_out_ch_slice)
    conv_cfg['out_ch_slice_last'] = int(best_out_ch_slice_last)

    return list(conv_cfg.values())
    # return best_method, total_bw


def get_pool_cfg(layer_name, Win, Hin, CHin, Kx, Ky, Sx, Sy, Px, Py, in_bit_width):
    print("*********************************************")
    print("start %s" % layer_name)
    pool_cfg = {}
    out_ch_slice = (CHin + Tout - 1) // Tout
    Tin_factor = (8 / in_bit_width)
    One_div_KxKy = 256 / (Kx * Ky)  # {1'b1,{(8){1'b0}}}/(Kx*Ky)
    P_left = Px
    P_up = Py
    Wout = ((Win + 2 * Px - Kx) // Sx + 1)
    Hout = ((Hin + 2 * Py - Ky) // Sy + 1)

    # width direction
    overlap_x = (Kx - Sx)
    max_out_width = (POOL_1D_DAT_IN_DEP - Kx) / Sx + 1
    # print("Wout = %d,  max_out_width = %d", Wout,max_out_width)

    if (max_out_width * Ky > POOL_2D_DAT_IN_DEP):
        print("*****")
        print("POOL_2D_DAT_IN_MEM not enough!")
        print("*****")

    split_w_num = (Wout + max_out_width - 1) // max_out_width  # ceil(Wout/max_out_width)-1
    if (split_w_num == 1):
        first_width_out = Wout
        first_width_in = Win

    else:
        first_width_out = max_out_width
        first_width_in = (first_width_out - 1) * Sx + Kx - Px
    # print("first_width_out = %d,    first_width_in=%d", first_width_out,first_width_in)

    middle_width_out = max_out_width
    middle_width_in = max_out_width * Sx + overlap_x
    # print("middle_width_out = %d,    middle_width_in=%d", middle_width_out,middle_width_in)

    if ((Wout - first_width_out) % middle_width_out == 0):
        split_w_num = (Wout - first_width_out) // middle_width_out + 1
        last_width_out = middle_width_out
    else:
        split_w_num = (Wout - first_width_out) // middle_width_out + 2
        last_width_out = (Wout - first_width_out) % middle_width_out

    if (split_w_num == 1):
        last_width_in = first_width_in
        last_width_out = first_width_out
    else:
        last_width_in = Win - first_width_in + overlap_x - (split_w_num - 2) * (middle_width_in - overlap_x)
    # print("last_width_out = %d,    last_width_in=%d", last_width_out,last_width_in)
    # print("split_w_num = %d", split_w_num)

    middle_1D_rd_addr_max = middle_width_out * Kx
    first_1D_rd_addr_max = first_width_out * Kx - P_left

    if (split_w_num == 1):
        if (last_width_in >= (Kx + (last_width_out - 1) * Sx)):
            last_1D_rd_addr_max = last_width_out * Kx
            P_right = 0
            # print("no need the padding_right region!")
            # print("*****")
            # print("width direction warning: the last %d column(s) are not used!",last_width_in-(Kx+(last_width_out-1)*Sx))
            # print("*****")
        else:
            P_right = ((Kx + (last_width_out - 1) * Sx)) - last_width_in - P_left
            last_1D_rd_addr_max = last_width_out * Kx - P_right
            # print("need the padding_right region! padding column(s)=%d",((Kx+(last_width_out-1)*Sx))-last_width_in)
    else:
        if (last_width_in >= (Kx + (last_width_out - 1) * Sx)):
            last_1D_rd_addr_max = last_width_out * Kx
            P_right = 0
            # print("no need the padding_right region!")
            # print("*****")
            # print("width direction warning: the last %d column(s) are not used!",last_width_in-(Kx+(last_width_out-1)*Sx))
            # print("*****")
        else:
            P_right = ((Kx + (last_width_out - 1) * Sx)) - last_width_in
            last_1D_rd_addr_max = last_width_out * Kx - P_right
            # print("need the padding_right region! padding column(s)=%d",((Kx+(last_width_out-1)*Sx))-last_width_in)
    # print("first_1D_rd_addr_max=%d", first_1D_rd_addr_max)
    # print("middle_1D_rd_addr_max=%d", middle_1D_rd_addr_max)
    # print("last_1D_rd_addr_max=%d", last_1D_rd_addr_max)
    # print("P_right=%d", P_right)
    # print("++++++++++++++++++")

    # simple height direction
    first_height_out = 1
    middle_height_out = 1
    last_height_out = 1

    overlap_y = (Ky - Sy)
    split_h_num = Hout
    first_height_in = (Ky - P_up)
    middle_height_in = Ky
    last_height_in = Hin - first_height_in + Ky - Sy - (split_h_num - 2) * Sy
    if (last_height_in > middle_height_in):
        # print("*****")
        # print("height direction warning: the last %d row(s) are not used!",last_height_in-middle_height_in)
        # print("*****")
        last_height_in = middle_height_in

    # print("split_h_num = %d", split_h_num)
    # print("first_height_out=%d,    first_height_in=%d",first_height_out, first_height_in)
    # print("middle_height_out=%d,    middle_height_in=%d",middle_height_out, middle_height_in)
    # print("last_height_out=%d,    last_height_in=%d", last_height_out, last_height_in)

    middle_2D_rd_addr_max = middle_height_out * Ky
    first_2D_rd_addr_max = first_height_out * Ky - P_up

    if (last_height_in == (Ky + (last_height_out - 1) * Sy)):
        last_2D_rd_addr_max = last_height_out * Ky
        P_down = 0
        # print("no need the padding_down region!")

    else:
        P_down = Ky - last_height_in
        last_2D_rd_addr_max = last_height_out * Ky - P_down
        # print("Need the padding_down region! padding row(s)=%d", Ky-last_height_in)

    pool_cfg['Hin'] = int(Hin)
    pool_cfg['Hout'] = int(Hout)
    pool_cfg['Win'] = int(Win)
    pool_cfg['Wout'] = int(Wout)
    pool_cfg['out_ch_slice'] = int(out_ch_slice)
    pool_cfg['Kx'] = int(Kx)
    pool_cfg['Ky'] = int(Ky)
    pool_cfg['Sx'] = int(Sx)
    pool_cfg['Sy'] = int(Sy)
    pool_cfg['P_left'] = int(P_left)
    pool_cfg['P_up'] = int(P_up)
    pool_cfg['P_right'] = int(P_right)
    pool_cfg['split_w_num'] = int(split_w_num)
    pool_cfg['first_width_in'] = int(first_width_in)
    pool_cfg['middle_width_in'] = int(middle_width_in)
    pool_cfg['last_width_in'] = int(last_width_in)
    pool_cfg['first_width_out'] = int(first_width_out)
    pool_cfg['middle_width_out'] = int(middle_width_out)
    pool_cfg['last_width_out'] = int(last_width_out)
    pool_cfg['first_1D_rd_addr_max'] = int(first_1D_rd_addr_max)
    pool_cfg['middle_1D_rd_addr_max'] = int(middle_1D_rd_addr_max)
    pool_cfg['last_1D_rd_addr_max'] = int(last_1D_rd_addr_max)
    pool_cfg['split_h_num'] = int(split_h_num)
    pool_cfg['first_height_in'] = int(first_height_in)
    pool_cfg['middle_height_in'] = int(middle_height_in)
    pool_cfg['last_height_in'] = int(last_height_in)
    pool_cfg['overlap_x'] = int(overlap_x)
    pool_cfg['overlap_y'] = int(overlap_y)
    pool_cfg['first_2D_rd_addr_max'] = int(first_2D_rd_addr_max)
    pool_cfg['middle_2D_rd_addr_max'] = int(middle_2D_rd_addr_max)
    pool_cfg['last_2D_rd_addr_max'] = int(last_2D_rd_addr_max)
    pool_cfg['One_div_KxKy'] = int(One_div_KxKy)

    return pool_cfg

