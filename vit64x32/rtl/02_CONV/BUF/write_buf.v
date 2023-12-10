`include "CNN_defines.vh"


module write_buf
(
    input dma2buf_DAT_wr_en,
    input [`log2BUF_DEP-1:0]dma2buf_DAT_wr_addr,
    input [`base_Tin*`MAX_DAT_DW-1:0]dma2buf_DAT_wr_data,

    input dma2buf_WT_wr_en,
    input [`log2BUF_DEP-1:0]dma2buf_WT_wr_addr,
    input [`base_Tin*`MAX_WT_DW-1:0]dma2buf_WT_wr_data,

    output reg [`BRAM_NUM-1:0]logic_mem_wr_en,
    output reg [`log2BRAM_DEPTH-1:0]logic_mem_wr_addr,
    output reg [`base_Tin*`MAX_DW-1:0]logic_mem_wr_dat
);


integer i;
reg [`BRAM_NUM-1:0]dat_mem_sel;
reg [`BRAM_NUM-1:0]wt_mem_sel;

always @(*)
begin
	for(i=0;i<`BRAM_NUM;i=i+1)
	begin
		dat_mem_sel[i]= dma2buf_DAT_wr_en & (dma2buf_DAT_wr_addr[`log2BUF_DEP-1:`log2BRAM_DEPTH]== i);
		wt_mem_sel[i]=  dma2buf_WT_wr_en &  (dma2buf_WT_wr_addr [`log2BUF_DEP-1:`log2BRAM_DEPTH]== i);
	end
end

`ifdef AXI_3ports
    always @(*)
    begin
        for(i=0;i<`BRAM_NUM;i=i+1)
        begin
            logic_mem_wr_en[i]=dat_mem_sel[i]|wt_mem_sel[i];
            logic_mem_wr_addr[i*`log2BRAM_DEPTH+:`log2BRAM_DEPTH]= dat_mem_sel[i]?
                        dma2buf_DAT_wr_addr[`log2BRAM_DEPTH-1:0]:(wt_mem_sel[i]?dma2buf_WT_wr_addr[`log2BRAM_DEPTH-1:0]:0);
            logic_mem_wr_dat[i*`base_Tin*`MAX_DW+:(`base_Tin*`MAX_DW)]= dat_mem_sel[i]?dma2buf_DAT_wr_data:(wt_mem_sel[i]?dma2buf_WT_wr_data:0);
        end
    end

//`else
//    always @(*)
//    begin
//        for(i=0;i<`BRAM_NUM;i=i+1)
//        begin
//            logic_mem_wr_en[i]=dat_mem_sel[i]|wt_mem_sel[i];
//            logic_mem_wr_addr= dma2buf_DAT_wr_en?dma2buf_DAT_wr_addr[`log2BRAM_DEPTH-1:0]:
//                              (dma2buf_WT_wr_en?dma2buf_WT_wr_addr[`log2BRAM_DEPTH-1:0]:0);
//            logic_mem_wr_dat= dma2buf_DAT_wr_en?dma2buf_DAT_wr_data:(dma2buf_WT_wr_en?dma2buf_WT_wr_data:0);
//        end
//    end
//`endif

`else
    always @(*)
    begin
        for(i=0;i<`BRAM_NUM;i=i+1)
            logic_mem_wr_en[i]=dat_mem_sel[i]|wt_mem_sel[i];
    end
	
	always @(*)
	begin
        case({dma2buf_DAT_wr_en,dma2buf_WT_wr_en})
            2'b01:begin
                logic_mem_wr_addr= dma2buf_WT_wr_addr[`log2BRAM_DEPTH-1:0];
                logic_mem_wr_dat= dma2buf_WT_wr_data;
                end
            2'b10:begin
                logic_mem_wr_addr= dma2buf_DAT_wr_addr[`log2BRAM_DEPTH-1:0];
                logic_mem_wr_dat= dma2buf_DAT_wr_data;
                end	
            default:begin
                logic_mem_wr_addr= 0;
                logic_mem_wr_dat= 0;
                end
		endcase
    end
	
`endif

endmodule

