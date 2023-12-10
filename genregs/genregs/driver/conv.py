from .basic import *
from ..ne import Var, If, For, Numb, expr_for_hook
from .register import register_driver


class MVM_Cfg:

    def __init__(self):
        self.CHin_Padding_with_Tin = None
        self.Hin = None
        self.Win = None
        self.CHout_Padding_with_Tout = None
        self.Hout = None
        self.Wout = None
        self.overlap = None
        self.best_single_dat_banks = None
        self.method = None
        self.dat_num_per_row = None
        self.wt_size_in_bytes = None
        self.wt_num_div_Tin = None
        self.Hout_Split_Times = None
        self.Hout_first = None
        self.Hout_middle = None
        self.Hout_last = None
        self.in_height_first = None
        self.in_height_middle = None
        self.in_height_last = None
        self.CHout_Split_Times = None
        self.out_ch_slice = None
        self.out_ch_slice_last = None


@register_driver("hbm", "fc", "1126")
def FPGA_RunHBM_FC(f_in, wt, f_out, Out_to_BRAM, In_from_BRAM, out_base_addr, in_base_addr, tag, log2_WT_base_addr_Bank_Step=28, Left_WT_Base_Addr=0):
    current_in_base_addr = If(In_from_BRAM, in_base_addr, f_in.payload)
    current_out_base_addr = If(Out_to_BRAM, out_base_addr, f_out.payload)
    # if In_from_BRAM:
    #     current_in_base_addr = in_base_addr
    # if Out_to_BRAM:
    #     current_out_base_addr = out_base_addr

    wt_dw = 4
    Hin, Win, CHout = f_in.height, f_in.width, f_out.channel
    Tin, Hout, Wout = base_Tin, Hin, Win
    CHout_div_Tout = (CHout+Tout-1)//Tout
    CHin_div_Tout = (f_in.channel+Tout-1)//Tout
    Tin_div_Tout = (base_Tin+Tout-1)//Tout
    CHout_Padding_with_Tout = CHout_div_Tout*Tout
    CHout_Padding = CHout_div_Tout*Tout
    CHin = CHin_div_Tout*Tout
    WT_CHin_div_Tin = (CHin+Tin-1)//Tin
    WT_CHin_Padding_with_Tin = WT_CHin_div_Tin*Tin

    WT_CHin_div_Tblock = WT_CHin_Padding_with_Tin//T_quant_block
    WT_scale_group_nums = (WT_CHin_Padding_with_Tin+WT_CH_Tgroup-1)//WT_CH_Tgroup
    WT_scale_bits = CHout_Padding*HBM_AXI_DATA_WIDTH*WT_scale_group_nums
    WT_SIZE_IN_BYTES = ((CHout_Padding*WT_CHin_Padding_with_Tin*wt_dw)>>3)+((WT_scale_bits)>>3)
    WT_NUM_DIV_TIN = CHout_Padding*WT_CHin_div_Tin

    overlap = 0
    dat_num_per_row = Win*WT_CHin_div_Tin
    min_dat_depth = dat_num_per_row
    min_wt_depth = WT_CHin_div_Tin*((Tin*MAX_WT_DW)//HBM_AXI_DATA_WIDTH)*(Tout//HBM_Port)

    if min_dat_depth>DAT_BRAM_DEPTH:
        print("Error: FPGA DAT BRAM DEPTH not enough!!!!\n")
        exit(-1)
    if min_wt_depth>WT_BRAM_DEPTH*WT_BRAM_NUM:
        print("Error: FPGA WT BRAM DEPTH not enough!!!!!\n")
        exit(-1)

    out_ch_slice = (WT_BRAM_DEPTH//min_wt_depth)*Tout
    CHout_Split_Times = If(out_ch_slice >= CHout_Padding, Numb(1), (CHout_Padding+out_ch_slice-1)//out_ch_slice)
    out_ch_slice = If(out_ch_slice >= CHout_Padding, CHout_Padding, out_ch_slice)
    out_ch_slice_last = If(CHout_Padding%out_ch_slice==0, out_ch_slice, CHout_Padding%out_ch_slice)
    # if out_ch_slice >= CHout_Padding:
    #     out_ch_slice = CHout_Padding
    #     CHout_Split_Times = 1
    # else:
    #     CHout_Split_Times = (CHout_Padding+out_ch_slice-1)//out_ch_slice

    # if CHout_Padding%out_ch_slice==0:
    #     out_ch_slice_last = out_ch_slice
    # else:
    #     out_ch_slice_last=CHout_Padding%out_ch_slice

    CHout_Split_Times_minus1 = CHout_Split_Times-1

    CH_in_extend = ((CHin+Tin-1)//Tin)*Tin
    CH_in_res_Tin_div_Tout_minus1 = If(CHin%Tin==0, Tin//Tout-1, ((CHin%Tin)+Tout-1)//Tout-1)
    # if CHin%Tin==0:
    #     CH_in_res_Tin_div_Tout_minus1 = Tin//Tout-1
    # else:
    #     CH_in_res_Tin_div_Tout_minus1 = ((CHin%Tin)+Tout-1)//Tout-1

    wt_size_in_bits = WT_SIZE_IN_BYTES//CHout_Padding*out_ch_slice*8

    CSB_Write(tag, 7, (CHin+Tout-1)//Tout)
    CSB_Write(tag, 8, (CHin+Tin-1)//Tin)
    CSB_Write(tag, 31, CH_in_res_Tin_div_Tout_minus1)
    CSB_Write(tag, 19, (out_ch_slice+Tout-1)//Tout)

    CSB_Write(tag, 20, current_in_base_addr)
    CSB_Write(tag, 23, wt_size_in_bits)
    CSB_Write(tag, 24, wt.payload)
    CSB_Write(tag, 49, (Left_WT_Base_Addr << 8) | log2_WT_base_addr_Bank_Step)
    CSB_Write(tag, 47, (Out_to_BRAM << 1) | In_from_BRAM)
    CSB_Write(tag, 50, CHout_Split_Times_minus1)

    CSB_Write(tag, 64+6, (out_ch_slice+Tout-1)//Tout)
    CSB_Write(tag, 64+20,current_out_base_addr)
    CSB_Write(tag, 64+23,CHout_Split_Times_minus1)
    CSB_Write(tag, 0,0b111111)
    CSB_Write(tag, 64+0,0b1001)

    CSB_Read(tag, 64+19, 1)


@register_driver("hbm", "mvm_cfg", "1126")
def Get_MVM_Cfg(Hin, Win, CHin, CHout, in_feature_dw, wt_dw, wt_scale_dw):
    ret = MVM_Cfg()

    Tin                    = base_Tin
    Wout                   = Win
    Hout                   = Hin
    CHout_div_Tout         = (CHout+Tout-1)//Tout
    CHin_div_Tout          = (CHin+Tout-1)//Tout
    Tin_div_Tout           = (base_Tin+Tout-1)//Tout
    CHout_Padding_with_Tout   = CHout_div_Tout*Tout
    CHout_Padding          = CHout_div_Tout*Tout
    CHin_Padding_with_Tout = CHin_div_Tout*Tout

    WT_CHin_div_Tin        = (CHin+Tin-1)//Tin
    WT_CHin_Padding_with_Tin=WT_CHin_div_Tin*Tin

    WT_CHin_div_Tblock     =WT_CHin_Padding_with_Tin//T_quant_block
    WT_scale_group_nums    =(WT_CHin_Padding_with_Tin+WT_CH_Tgroup-1)//WT_CH_Tgroup
    WT_scale_bits          =CHout_Padding*HBM_AXI_DATA_WIDTH*WT_scale_group_nums
    WT_SIZE_IN_BYTES       = ((CHout_Padding*WT_CHin_Padding_with_Tin*wt_dw)>>3)+((WT_scale_bits)>>3)
    WT_NUM_DIV_TIN         = CHout_Padding*WT_CHin_div_Tin

    overlap = 0
    dat_num_per_row = Win*WT_CHin_div_Tin
    min_dat_depth = dat_num_per_row
    min_wt_depth = WT_CHin_div_Tin*((Tin*MAX_WT_DW)//HBM_AXI_DATA_WIDTH)*(Tout//HBM_Port)

    if min_dat_depth > DAT_BRAM_DEPTH:
        print(f"Error: Hin={Hin}, Win={Win}, CHin={CHin}, CHout={CHout}")
        print(f"Error: min_dat_depth={min_dat_depth}, DAT_BRAM_DEPTH={DAT_BRAM_DEPTH}")
        print("Error: FPGA DAT BRAM DEPTH not enough!!!!")
        exit(-1)
    if min_wt_depth > WT_BRAM_DEPTH*WT_BRAM_NUM:
        print(f"Error: Hin={Hin}, Win={Win}, CHin={CHin}, CHout{CHout}")
        print("Error: FPGA WT BRAM DEPTH not enough!!!!!")
        exit(-1)

    out_ch_slice = (WT_BRAM_DEPTH//min_wt_depth)*Tout
    out_ch_slice_last = If(CHout_Padding%out_ch_slice==0, out_ch_slice, Tout)
    out_ch_slice = If(CHout_Padding%out_ch_slice==0, out_ch_slice, Tout)
    CHout_Split_Times = If(out_ch_slice >= CHout_Padding, Numb(1), (CHout_Padding+out_ch_slice-1)//out_ch_slice)
    out_ch_slice = If(out_ch_slice >= CHout_Padding, CHout_Padding, out_ch_slice)
    # if(CHout_Padding%out_ch_slice==0)
    #     out_ch_slice_last=out_ch_slice;
    # else {
    #     out_ch_slice=Tout;
    #     out_ch_slice_last=Tout;
    # }
    # if(out_ch_slice>=CHout_Padding)
    # {
    #     out_ch_slice=CHout_Padding;
    #     CHout_Split_Times=1;
    # }
    # else 
    #     CHout_Split_Times=(CHout_Padding+out_ch_slice-1)/out_ch_slice;

    Hout_first = DAT_BRAM_DEPTH//dat_num_per_row
    in_height_first = Hout_first
    Hout_middle = DAT_BRAM_DEPTH//dat_num_per_row
    in_height_middle = Hout_middle

    Hout_first = If(Hout_first >= Hout, Hout, Hout_first)
    in_height_first = If(Hout_first >= Hout, Hin, in_height_first)
    Hout_Split_Times = If((Hout-Hout_first)%Hout_middle == 0, (Hout-Hout_first)//Hout_middle+1, (Hout-Hout_first)//Hout_middle+2)
    Hout_last = If((Hout-Hout_first)%Hout_middle == 0, Hout_middle, (Hout-Hout_first)%Hout_middle)

    # if(Hout_first>=Hout)
    # {
    #     Hout_first=Hout;
    #     in_height_first=Hin;
    # }

    # if((Hout-Hout_first)%Hout_middle == 0)
    # {
    #     Hout_Split_Times=(Hout-Hout_first)/Hout_middle+1;
    #     Hout_last=Hout_middle;
    # }
    # else
    # {
    #     Hout_Split_Times=(Hout-Hout_first)/Hout_middle+2;
    #     Hout_last=(Hout-Hout_first)%Hout_middle;
    # }
    in_height_last = Hin-in_height_first+overlap-(Hout_Split_Times-2)*(in_height_middle-overlap)

    total_bytes_if_reuse_wt=Tb*(dat_num_per_row*Hin)*CHout_Split_Times*(Tin*MAX_DAT_DW//8)\
                           +CHout_Padding*WT_CHin_div_Tin*(Tin*MAX_WT_DW//8) \
                           +(CHout_Split_Times * Hout_Split_Times * 100 * 32//8)
    total_bytes_if_reuse_dat=Hout_Split_Times*CHout_Padding*WT_CHin_div_Tin*(Tin*MAX_WT_DW//8) \
                          +(dat_num_per_row*Hin)*(Tin*MAX_DAT_DW//8)*Tb \
                          +(CHout_Split_Times*Hout_Split_Times * 100 * 32//8)

    print("total_bytes_if_reuse_wt=", total_bytes_if_reuse_wt.simplify())
    print("total_bytes_if_reuse_dat=", total_bytes_if_reuse_dat.simplify())

    if (total_bytes_if_reuse_wt < total_bytes_if_reuse_dat):
        best_method = 0
    else:
        best_method = 1

    ret.CHin_Padding_with_Tin = CHin_Padding_with_Tout
    ret.Hin = Hin
    ret.Win = Win
    ret.CHout_Padding_with_Tout = CHout_Padding_with_Tout
    ret.Hout = Hout
    ret.Wout = Wout
    ret.overlap = overlap
    ret.best_single_dat_banks = 0
    ret.method = best_method
    ret.dat_num_per_row = dat_num_per_row
    ret.wt_size_in_bytes = WT_SIZE_IN_BYTES
    ret.wt_num_div_Tin = WT_NUM_DIV_TIN
    ret.Hout_Split_Times = Hout_Split_Times
    ret.Hout_first = Hout_first
    ret.Hout_middle = Hout_middle
    ret.Hout_last = Hout_last
    ret.in_height_first = in_height_first
    ret.in_height_middle = in_height_middle
    ret.in_height_last = in_height_last
    ret.CHout_Split_Times = CHout_Split_Times
    ret.out_ch_slice = out_ch_slice
    ret.out_ch_slice_last = out_ch_slice_last
    return ret


def FPGA_HBM_MVM_single_time(
    CHin, Hin, Win, CHout, Wout, Hout, Out_to_BRAM, In_from_BRAM, relu_en, 
    feature_in_base, feature_in_surface_stride, feature_in_line_stride,
    wt_base_addr, wt_size_in_bits, wt_num_div_Tin, 
    feature_out_base, feature_out_surface_stride, feature_out_line_stride, 
    single_dat_buf_num, dma_dat_reuse, dma_wt_reuse, tag, log2_WT_base_addr_Bank_Step, Left_WT_Base_Addr
):
    # if CHin % base_Tin == 0:
    #     CH_in_res_Tin_div_Tout_minus1 = base_Tin // Tout-1
    # else:
    #     CH_in_res_Tin_div_Tout_minus1 = ((CHin%base_Tin)+Tout-1)//Tout-1
    CH_in_res_Tin_div_Tout_minus1 = If(CHin % base_Tin, ((CHin%base_Tin)+Tout-1)//Tout-1, base_Tin // Tout-1)
    CH_in_div_Tin = (CHin + base_Tin - 1) // base_Tin
    CH_in_div_Tout = (CHin + Tout - 1) // Tout
    CH_out_div_Tout = (CHout + Tout - 1) // Tout
    effect_in_pixel = Win * Hin
    effect_out_pixel = Wout*Hout
    effect_out_pixel_div_Tout = (effect_out_pixel+Tout-1)//Tout

    CSB_Write(tag, 3, single_dat_buf_num)
    CSB_Write(tag, 4, Win)
    CSB_Write(tag, 5, Hin)
    CSB_Write(tag, 6, effect_in_pixel)
    CSB_Write(tag, 7, CH_in_div_Tout)
    CSB_Write(tag, 8, CH_in_div_Tin)
    CSB_Write(tag, 31, CH_in_res_Tin_div_Tout_minus1)
    CSB_Write(tag, 15, Wout)
    CSB_Write(tag, 16, effect_out_pixel)
    CSB_Write(tag, 17, Hout)
    CSB_Write(tag, 18, CHout)
    CSB_Write(tag, 19, CH_out_div_Tout)
    CSB_Write(tag, 33, effect_out_pixel_div_Tout)

    CSB_Write(tag, 20, feature_in_base)
    CSB_Write(tag, 21, feature_in_surface_stride)
    CSB_Write(tag, 22, feature_in_line_stride)
    CSB_Write(tag, 23, wt_size_in_bits)
    CSB_Write(tag, 32, wt_num_div_Tin)
    CSB_Write(tag, 24, wt_base_addr)
    CSB_Write(tag, 49, (Left_WT_Base_Addr * (2**8)) + log2_WT_base_addr_Bank_Step)
    CSB_Write(tag, 25, (dma_wt_reuse * 2) + dma_dat_reuse)
    CSB_Write(tag, 47, (Out_to_BRAM * 2) + In_from_BRAM)

    CSB_Write(tag, 64+4,Hout)
    CSB_Write(tag, 64+5,Wout)
    CSB_Write(tag, 64+6,CH_out_div_Tout)
    CSB_Write(tag, 64+12,effect_out_pixel)
    CSB_Write(tag, 64+18,relu_en)
    CSB_Write(tag, 64+20,feature_out_base)
    CSB_Write(tag, 64+21,feature_out_surface_stride)
    CSB_Write(tag, 64+22,feature_out_line_stride)

    CSB_Write(tag, 0, 0b011111)
    CSB_Write(tag, 64+0,0b0001)

    CSB_Read(tag, 64 + 19, 1)


@register_driver("hbm", "mvm", "1126")
def FPGA_HBM_MVM(mvm_cfg, relu_en, f_in, wt, f_out, Out_to_BRAM, In_from_BRAM, out_base_addr, in_base_addr, tag, log2_WT_base_addr_Bank_Step, Left_WT_Base_Addr):
    current_in_base_addr = f_in.payload
    current_out_base_addr = f_out.payload
    if In_from_BRAM:
        current_in_base_addr = in_base_addr
    if Out_to_BRAM:
        current_out_base_addr = out_base_addr

    if mvm_cfg.method == 0:
        @expr_for_hook(CSB_For, CSB_End)
        def for_body_CHout_Split_Times(n):

            @expr_for_hook(CSB_For, CSB_End)
            def for_body_Hout_Split_Times(k):
                CH_in_single = mvm_cfg.CHin_Padding_with_Tin
                CH_out_single = If(n != mvm_cfg.CHout_Split_Times-1, mvm_cfg.out_ch_slice, mvm_cfg.out_ch_slice_last)
                line_offset_in = If(k == 0, Numb(0), (mvm_cfg.in_height_first - mvm_cfg.overlap) + (k - 1) * (mvm_cfg.in_height_middle - mvm_cfg.overlap))
                line_offset_out = If(k == 0, Numb(0), mvm_cfg.Hout_first + (k - 1) * mvm_cfg.Hout_middle)
                pad_y_single = 0
                dma_wt_reuse_single = If(k == 0, Numb(0), Numb(1))
                in_height_single = If(k == 0, mvm_cfg.in_height_first, If(k == mvm_cfg.Hout_Split_Times - 1, mvm_cfg.in_height_last, mvm_cfg.in_height_middle))
                Hout_single = If(k == 0, mvm_cfg.Hout_first, If(k == mvm_cfg.Hout_Split_Times - 1, mvm_cfg.Hout_last, mvm_cfg.Hout_middle))
                FPGA_HBM_MVM_single_time(CH_in_single,in_height_single,mvm_cfg.Win,CH_out_single,mvm_cfg.Wout,Hout_single, 
                        Out_to_BRAM, In_from_BRAM, relu_en,
                        (current_in_base_addr) + f_in.line_stride * line_offset_in, f_in.surface_stride, f_in.line_stride,
                        (wt.payload) + mvm_cfg.wt_size_in_bytes // HBM_Port // mvm_cfg.CHout_Padding_with_Tout * mvm_cfg.out_ch_slice * n, 
                        mvm_cfg.wt_size_in_bytes // mvm_cfg.CHout_Padding_with_Tout * CH_out_single * 8, 
                        mvm_cfg.wt_num_div_Tin // mvm_cfg.CHout_Padding_with_Tout * CH_out_single,
                        (current_out_base_addr) + f_out.line_stride * line_offset_out + f_out.surface_stride * n * (mvm_cfg.out_ch_slice // Tout), 
                        f_out.surface_stride, f_out.line_stride, mvm_cfg.best_single_dat_banks,0,dma_wt_reuse_single, tag, log2_WT_base_addr_Bank_Step, Left_WT_Base_Addr)

            for_k = Var("k")
            for_expr_k = For(for_k, Numb(0), mvm_cfg.Hout_Split_Times, for_body_Hout_Split_Times)
            for_expr_k.run(tag)

        for_n = Var("n")
        for_expr_n = For(for_n, Numb(0), mvm_cfg.CHout_Split_Times, for_body_CHout_Split_Times)
        for_expr_n.run(tag)
    else:
        @expr_for_hook(CSB_For, CSB_End)
        def for_body_Hout_Split_Times(k):

            @expr_for_hook(CSB_For, CSB_End)
            def for_body_CHout_Split_Times(n):
                CH_in_single = mvm_cfg.CHin_Padding_with_Tin
                CH_out_single = If(n != mvm_cfg.CHout_Split_Times-1, mvm_cfg.out_ch_slice, mvm_cfg.out_ch_slice_last)
                dma_dat_reuse_single = If(n == 0, Numb(0), Numb(1))
                line_offset_in = If(k == 0, Numb(0), (mvm_cfg.in_height_first - mvm_cfg.overlap) + (k - 1) * (mvm_cfg.in_height_middle - mvm_cfg.overlap))
                line_offset_out = If(k == 0, Numb(0), mvm_cfg.Hout_first + (k - 1) * mvm_cfg.Hout_middle)
                pad_y_single = 0
                in_height_single = If(k == 0, mvm_cfg.in_height_first, If(k == mvm_cfg.Hout_Split_Times - 1, mvm_cfg.in_height_last, mvm_cfg.in_height_middle))
                Hout_single = If(k == 0, mvm_cfg.Hout_first, If(k == mvm_cfg.Hout_Split_Times - 1, mvm_cfg.Hout_last, mvm_cfg.Hout_middle))
                FPGA_HBM_MVM_single_time(CH_in_single,in_height_single,mvm_cfg.Win,CH_out_single,mvm_cfg.Wout,Hout_single,
                        Out_to_BRAM, In_from_BRAM, relu_en,
                        (current_in_base_addr) + f_in.line_stride * line_offset_in, f_in.surface_stride, f_in.line_stride,
                        (wt.payload) + mvm_cfg.wt_size_in_bytes // HBM_Port // mvm_cfg.CHout_Padding_with_Tout * mvm_cfg.out_ch_slice * n, 
                        mvm_cfg.wt_size_in_bytes // mvm_cfg.CHout_Padding_with_Tout * CH_out_single * 8, 
                        mvm_cfg.wt_num_div_Tin // mvm_cfg.CHout_Padding_with_Tout * CH_out_single,
                        (current_out_base_addr) + f_out.line_stride * line_offset_out + f_out.surface_stride * n * (mvm_cfg.out_ch_slice // Tout), 
                        f_out.surface_stride, f_out.line_stride, mvm_cfg.best_single_dat_banks,dma_dat_reuse_single,0, tag, log2_WT_base_addr_Bank_Step, Left_WT_Base_Addr)

            for_n = Var("n")
            for_expr_n = For(for_n, Numb(0), mvm_cfg.CHout_Split_Times, for_body_CHout_Split_Times)
            for_expr_n.run(tag)

        for_k = Var("k")
        for_expr_k = For(for_k, Numb(0), mvm_cfg.Hout_Split_Times, for_body_Hout_Split_Times)
        for_expr_k.run(tag)
        '''
        for n in range(mvm_cfg.CHout_Split_Times):
            for k in range(mvm_cfg.Hout_Split_Times):
                CH_in_single = mvm_cfg.CHin_Padding_with_Tin

                if n != mvm_cfg.CHout_Split_Times-1:
                    CH_out_single = mvm_cfg.out_ch_slice
                else:
                    CH_out_single = mvm_cfg.out_ch_slice_last

                if k == 0:
                    line_offset_in, line_offset_out, pad_y_single, dma_wt_reuse_single = 0, 0, 0, 0
                else:
                    line_offset_in = (mvm_cfg.in_height_first - mvm_cfg.overlap) + (k - 1) * (mvm_cfg.in_height_middle - mvm_cfg.overlap)
                    line_offset_out = mvm_cfg.Hout_first + (k - 1) * mvm_cfg.Hout_middle
                    pad_y_single = 0
                    dma_wt_reuse_single = 1

                if k == 0:
                    in_height_single, Hout_single = mvm_cfg.in_height_first, mvm_cfg.Hout_first
                else:
                    if k == mvm_cfg.Hout_Split_Times - 1:
                        in_height_single, Hout_single = mvm_cfg.in_height_last, mvm_cfg.Hout_last
                    else:
                        in_height_single, Hout_single = mvm_cfg.in_height_middle, mvm_cfg.Hout_middle
                FPGA_HBM_MVM_single_time(CH_in_single,in_height_single,mvm_cfg.Win,CH_out_single,mvm_cfg.Wout,Hout_single, 
                        Out_to_BRAM, In_from_BRAM, relu_en,
                        (current_in_base_addr) + f_in.line_stride * line_offset_in, f_in.surface_stride, f_in.line_stride,
                        (wt.payload) + mvm_cfg.wt_size_in_bytes // HBM_Port // mvm_cfg.CHout_Padding_with_Tout * mvm_cfg.out_ch_slice * n, 
                        mvm_cfg.wt_size_in_bytes // mvm_cfg.CHout_Padding_with_Tout * CH_out_single * 8, 
                        mvm_cfg.wt_num_div_Tin // mvm_cfg.CHout_Padding_with_Tout * CH_out_single,
                        (current_out_base_addr) + f_out.line_stride * line_offset_out + f_out.surface_stride * n * (mvm_cfg.out_ch_slice // Tout), 
                        f_out.surface_stride, f_out.line_stride, mvm_cfg.best_single_dat_banks,0,dma_wt_reuse_single, tag, log2_WT_base_addr_Bank_Step, Left_WT_Base_Addr)
    else:
        for k in range(mvm_cfg.Hout_Split_Times):
            for n in range(mvm_cfg.CHout_Split_Times):
                CH_in_single = mvm_cfg.CHin_Padding_with_Tin
                if n != mvm_cfg.CHout_Split_Times - 1:
                    CH_out_single = mvm_cfg.out_ch_slice
                else:
                    CH_out_single = mvm_cfg.out_ch_slice_last

                if n == 0:
                    dma_dat_reuse_single = 0
                else:
                    dma_dat_reuse_single = 1

                if k == 0:
                    line_offset_in, line_offset_out, pad_y_single = 0, 0, 0
                else:
                    line_offset_in = (mvm_cfg.in_height_first - mvm_cfg.overlap) + (k - 1) * (mvm_cfg.in_height_middle - mvm_cfg.overlap)
                    line_offset_out = mvm_cfg.Hout_first + (k - 1) * mvm_cfg.Hout_middle
                    pad_y_single = 0

                if k == 0:
                    in_height_single, Hout_single = mvm_cfg.in_height_first, mvm_cfg.Hout_first
                else:
                    if k == mvm_cfg.Hout_Split_Times - 1:
                        in_height_single = mvm_cfg.in_height_last
                        Hout_single = mvm_cfg.Hout_last
                    else:
                        in_height_single = mvm_cfg.in_height_middle
                        Hout_single = mvm_cfg.Hout_middle
                FPGA_HBM_MVM_single_time(CH_in_single,in_height_single,mvm_cfg.Win,CH_out_single,mvm_cfg.Wout,Hout_single,
                        Out_to_BRAM, In_from_BRAM, relu_en,
                        (current_in_base_addr) + f_in.line_stride * line_offset_in, f_in.surface_stride, f_in.line_stride,
                        (wt.payload) + mvm_cfg.wt_size_in_bytes // HBM_Port // mvm_cfg.CHout_Padding_with_Tout * mvm_cfg.out_ch_slice * n, 
                        mvm_cfg.wt_size_in_bytes // mvm_cfg.CHout_Padding_with_Tout * CH_out_single * 8, 
                        mvm_cfg.wt_num_div_Tin // mvm_cfg.CHout_Padding_with_Tout * CH_out_single,
                        (current_out_base_addr) + f_out.line_stride * line_offset_out + f_out.surface_stride * n * (mvm_cfg.out_ch_slice // Tout), 
                        f_out.surface_stride, f_out.line_stride, mvm_cfg.best_single_dat_banks,dma_dat_reuse_single,0, tag, log2_WT_base_addr_Bank_Step, Left_WT_Base_Addr)
        '''
