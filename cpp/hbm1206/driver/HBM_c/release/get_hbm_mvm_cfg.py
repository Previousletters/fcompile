def get_hbm_mvm_cfg(str, Hin, Win, CHin, CHout, wt_dw):
    print("*********************************************")
    print("start %s" % str)
    mvm_cfg = {}

    overlap = 0
    best_single_dat_banks = 0
    best_wt_banks = 0
    best_method = 0
    mininum_bw = 0

    CHout_div_Tout            = ((CHout+Tout-1)//Tout)
    CHin_div_Tout             = ((CHin+Tout-1)//Tout)
    Tin_div_Tout              = ((Tin+Tout-1)//Tout)
    CHout_Padding_with_Tout   = (CHout_div_Tout*Tout)
    CHout_Padding             = (CHout_div_Tout*Tout)
    CHin_Padding_with_Tout    = (CHin_div_Tout*Tout)

    WT_CHin_div_Tin           = ((CHin+Tin-1)//Tin)
    WT_CHin_Padding_with_Tin  = (WT_CHin_div_Tin*Tin)
    
    WT_CHin_div_Tblock        = (WT_CHin_Padding_with_Tin//T_quant_block)
    WT_scale_group_nums       = ((WT_CHin_Padding_with_Tin+WT_CH_Tgroup-1)//WT_CH_Tgroup)
    WT_scale_bits             = (CHout_Padding*HBM_AXI_DATA_WIDTH*WT_scale_group_nums)
    WT_SIZE_IN_BYTES          = (((CHout_Padding*WT_CHin_Padding_with_Tin*wt_dw)>>3)+((WT_scale_bits)>>3))
    WT_NUM_DIV_TIN            = (CHout_Padding*WT_CHin_div_Tin)

    Wout = Win
    Hout = Hin
    dat_num_per_row = Win * WT_CHin_div_Tin
    min_dat_depth, min_wt_depth = dat_num_per_row, Tout*WT_CHin_div_Tin

    print("dat_num_per_row = ", dat_num_per_row)

    if min_dat_depth > DAT_BRAM_DEPTH:
        print("--------------------------")
        print("Error:  DAT BRAM DEPTH  not enough!!!")
        print("--------------------------")
        return "error"

    if min_wt_depth > WT_BRAM_DEPTH*WT_BRAM_NUM:
        print("--------------------------")
        print("Error:  WT BRAM DEPTH  not enough!!!")
        print("--------------------------")
        return "error"


    out_ch_slice = (WT_BRAM_DEPTH // min_wt_depth) * Tout

    if (out_ch_slice >= CHout_Padding):
        out_ch_slice = CHout_Padding
        CHout_Split_Times = 1
    else:
        CHout_Split_Times = (CHout_Padding+out_ch_slice-1)//out_ch_slice

    if (CHout_Padding % out_ch_slice == 0):
        out_ch_slice_last = out_ch_slice
    else:
        out_ch_slice_last = CHout_Padding % out_ch_slice

    Hout_first = DAT_BRAM_DEPTH//dat_num_per_row
    in_height_first = Hout_first
    Hout_middle = DAT_BRAM_DEPTH//dat_num_per_row
    in_height_middle = Hout_middle

    if Hout_first>=Hout:
        Hout_first = Hout
        in_height_first = Hin

    if (Hout-Hout_first)%Hout_middle == 0:
        Hout_Split_Times = (Hout-Hout_first)//Hout_middle+1
        Hout_last = Hout_middle
    else:
        Hout_Split_Times = (Hout-Hout_first)//Hout_middle+2
        Hout_last = (Hout-Hout_first)%Hout_middle
    in_height_last=Hin-in_height_first+overlap-(Hout_Split_Times-2)*(in_height_middle-overlap)
    
    total_bytes_if_reuse_wt = Tb*(dat_num_per_row*Hin)*CHout_Split_Times*(Tin*MAX_DAT_DW//8) \
                          +CHout_Padding*WT_CHin_div_Tin*(Tin*MAX_WT_DW//8) \
                          +(CHout_Split_Times * Hout_Split_Times * 100 * 32//8);
    total_bytes_if_reuse_dat = Hout_Split_Times*CHout_Padding*WT_CHin_div_Tin*(Tin*MAX_WT_DW//8) \
                          +(dat_num_per_row*Hin)*(Tin*MAX_DAT_DW//8)*Tb \
                          +(CHout_Split_Times*Hout_Split_Times * 100 * 32//8);

    print("total_bytes_if_reuse_wt=%d,total_bytes_if_reuse_dat=%d"%(total_bytes_if_reuse_wt,total_bytes_if_reuse_dat))

    if total_bytes_if_reuse_wt < total_bytes_if_reuse_dat:
        best_method = 0;
    else:
        best_method = 1;

    print("**** final result *****")
    print("HBM MVM of %s" % str)
    print("   ")
    print("CHout_Split_Times = ", CHout_Split_Times)
    print("out_ch_slice = ", out_ch_slice)
    print("out_ch_slice_last = ", out_ch_slice_last)

    print("Hout_Split_Times = ", Hout_Split_Times)
    print("in_height_first = ", in_height_first)
    print("in_height_middle = ", in_height_middle)
    print("in_height_last = ", in_height_last)
    print("total_bw_if_reuse_wt = ", total_bytes_if_reuse_wt)
    print("total_bw_if_reuse_dat = ", total_bytes_if_reuse_dat)

    print("best_method = ", best_method)

    mvm_cfg["CHin_Padding_with_Tin"]    = WT_CHin_Padding_with_Tin
    mvm_cfg["Hin"]                      = Hin
    mvm_cfg["Win"]                      = Win
    mvm_cfg["CHout_Padding_with_Tout"]  = CHout_Padding_with_Tout
    mvm_cfg["Hout"]                     = Hout
    mvm_cfg["Wout"]                     = Wout
    mvm_cfg["overlap"]                  = overlap
    mvm_cfg["best_single_dat_banks"]    = best_single_dat_banks
    mvm_cfg["method"]                   = best_method
    mvm_cfg["dat_num_per_row"]          = dat_num_per_row
    mvm_cfg["wt_size_in_bytes"]         = WT_SIZE_IN_BYTES
    mvm_cfg["wt_num_div_Tin"]           = WT_NUM_DIV_TIN
    mvm_cfg["Hout_Split_Times"]         = Hout_Split_Times
    mvm_cfg["Hout_first"]               = Hout_first
    mvm_cfg["Hout_middle"]              = Hout_middle
    mvm_cfg["Hout_last"]                = Hout_last
    mvm_cfg["in_height_first"]          = in_height_first
    mvm_cfg["in_height_middle"]         = in_height_middle
    mvm_cfg["in_height_last"]           = in_height_last
    mvm_cfg["CHout_Split_Times"]        = CHout_Split_Times
    mvm_cfg["out_ch_slice"]             = out_ch_slice
    mvm_cfg["out_ch_slice_last"]        = out_ch_slice_last

    return mvm_cfg


if __name__ == '__main__':
    ############################## TOP defines ##############################
    Tb                  = 1
    Tin                 = 128
    Tout                = 64
    HBM_Port            = 32
    MAX_DAT_DW          = 16
    MAX_WT_DW           = 4
    T_quant_block       = 128
    HBM_AXI_DATA_WIDTH  = 256

    WT_quant_scale_DW   = 16
    WT_CH_Tgroup        = T_quant_block*HBM_AXI_DATA_WIDTH//WT_quant_scale_DW

    DAT_BRAM_NUM        = 1
    DAT_BRAM_DEPTH      = ((1<<24)//Tin//MAX_DAT_DW//DAT_BRAM_NUM)

    WT_BRAM_NUM         = HBM_Port
    WT_BRAM_DEPTH       = ((1<<24)//HBM_AXI_DATA_WIDTH//WT_BRAM_NUM)
    ############################## END defines ##############################

    mvm0_cfg = get_hbm_mvm_cfg("mvm0", 2, 64, 1024, 512, wt_dw=MAX_WT_DW)
    print("mvm0_cfg=", mvm0_cfg.values())
    print("mvm0_cfg=", mvm0_cfg)
