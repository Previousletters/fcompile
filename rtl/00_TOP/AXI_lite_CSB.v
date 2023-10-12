`include "CNN_defines.vh"


module AXI_lite_CSB
(
	input clk,
	input rst_n,

	//AW channel
	input S_AXI_AWVALID,
	output S_AXI_AWREADY,
	input [`log2CSR_REG_NUM+2-1:0]S_AXI_AWADDR,
	input [2:0]S_AXI_AWPROT,//ignore
	    
	//Wr channel
	input [31:0]S_AXI_WDATA,
	input S_AXI_WVALID,
	output S_AXI_WREADY,
	input [3:0]S_AXI_WSTRB,//ignore
	output [1:0]S_AXI_BRESP,//=2'b0
	output S_AXI_BVALID,
	input S_AXI_BREADY,
	
	//AR channel
	input S_AXI_ARVALID,
	output S_AXI_ARREADY,
	input [`log2CSR_REG_NUM+2-1:0]S_AXI_ARADDR,
	input [2:0]S_AXI_ARPROT,//ignore

    //Rd channel
	output [31:0]S_AXI_RDATA,
	output [1:0]S_AXI_RRESP,//=2'b0
	output S_AXI_RVALID,
	input S_AXI_RREADY,

	//CSB to Conv Path: 0~63
	output csb2cp_csr_req_vld,
	input csb2cp_csr_req_rdy,
	output [(`log2CSR_REG_NUM+32+1):0]csb2cp_csr_req_pd,
	input cp_csr2csb_resp_vld,
	input [31:0]cp_csr2csb_resp_pd,

	//CSB to SDP: 64~127
	output csb2csr_req_vld,
	input csb2csr_req_rdy,
	output [(`log2CSR_REG_NUM+32+1):0]csb2csr_req_pd,
	input csr2csb_resp_vld,
	input [31:0]csr2csb_resp_pd,

	//CSB to PDP: 128~255
	output csb2Matrix_csr_req_vld,
	input csb2Matrix_csr_req_rdy,
	output [(`log2CSR_REG_NUM+32+1):0]csb2Matrix_csr_req_pd,
	input Matrix_csr2csb_resp_vld,
	input [31:0]Matrix_csr2csb_resp_pd
);

reg axi_wready;
reg axi_bvalid;
reg axi_rvalid;
reg [1:0]state;
reg [1:0]w_state;
reg [2:0]r_state;
reg axi_awready;
reg axi_arready;

reg cmd_vld;
reg [`log2CSR_REG_NUM:0]cmd_addr;
reg [31:0]cmd_wdata;

wire cmd_wr_rd=(state==2'd1);
wire [`log2CSR_REG_NUM:0]cmd_addr_cp=cmd_addr;
wire [`log2CSR_REG_NUM:0]cmd_addr_sdp=cmd_addr-64;
wire [`log2CSR_REG_NUM:0]cmd_addr_Matrix=cmd_addr-128;
//wire [(`log2CSR_REG_NUM+32+1):0]cmd={cmd_wr_rd,cmd_wdata,cmd_addr};

wire cp_sel=(cmd_addr<64);
wire sdp_sel=(64<=cmd_addr)&(cmd_addr<127);
wire Matrix_sel=(128<=cmd_addr)&(cmd_addr<255);

assign csb2cp_csr_req_vld=cmd_vld&cp_sel;
assign csb2csr_req_vld=cmd_vld&sdp_sel;
assign csb2Matrix_csr_req_vld=cmd_vld&Matrix_sel;
assign csb2cp_csr_req_pd={cmd_wr_rd,cmd_wdata,cmd_addr_cp};//cmd;
assign csb2csr_req_pd={cmd_wr_rd,cmd_wdata,cmd_addr_sdp};//cmd;
assign csb2Matrix_csr_req_pd={cmd_wr_rd,cmd_wdata,cmd_addr_Matrix};//cmd;
wire cmd_rdy=csb2cp_csr_req_rdy|csb2csr_req_rdy|csb2Matrix_csr_req_rdy;

wire rsp_vld=cp_csr2csb_resp_vld|csr2csb_resp_vld|Matrix_csr2csb_resp_vld;
wire [31:0]rsp_data= (cp_sel?cp_csr2csb_resp_pd:0) | (sdp_sel?csr2csb_resp_pd:0) | (Matrix_sel?Matrix_csr2csb_resp_pd:0);

assign S_AXI_WREADY=axi_wready;
assign S_AXI_BRESP=2'b0;
assign S_AXI_RDATA=rsp_data;
assign S_AXI_RRESP=2'b0;
assign S_AXI_BVALID=axi_bvalid;
assign S_AXI_RVALID=axi_rvalid;
assign S_AXI_AWREADY=axi_awready;
assign S_AXI_ARREADY=axi_arready;

always @(posedge clk or negedge rst_n)
if(~rst_n)
    axi_bvalid  <= 0;
else
    begin    
        if ( ~axi_bvalid & S_AXI_WREADY & S_AXI_WVALID)
            axi_bvalid <= 1'b1;                
        else
            begin
                if (S_AXI_BREADY & axi_bvalid) 
                    axi_bvalid <= 1'b0; 
            end
    end

always @(posedge clk or negedge rst_n)
if(~rst_n)
    begin
	state<=1'b0;
	w_state<='d0;
	r_state<='d0;
	axi_awready<=1'b0;
	axi_arready<=1'b0;
	axi_wready<=1'b0;
	axi_rvalid<=1'b0;
	cmd_vld<=1'b0;
	cmd_wdata<='d0;
	cmd_addr<=0;
    end
else
    case(state)
        'd0://Idle
            begin
                if(S_AXI_AWVALID)
                    begin
                        state<='d1;
                        axi_awready<=1'b1;
                    end
                else
                    if(S_AXI_ARVALID)
                        begin
                            state<='d2;
                            axi_arready<=1'b1;
                        end
            end
        'd1://Writing
            begin
                case(w_state)
                'd0:
                    if(S_AXI_AWVALID)
                    begin
                        w_state<='d1;
                        axi_awready<=1'b0;
                        cmd_addr<=S_AXI_AWADDR[`log2CSR_REG_NUM+2-1:2];
                    end
                'd1:
                    if(S_AXI_WVALID)
                    begin
                        w_state<='d2;
                        cmd_wdata<=S_AXI_WDATA;
                        cmd_vld<=1'b1;
                    end
                'd2:
                    if(cmd_rdy)
                    begin
                        w_state<='d3;
                        cmd_vld<=1'b0;
                        axi_wready<=1'b1;
                    end
                'd3:
                    if(S_AXI_WVALID)
                    begin
                        state<='d0;
                        w_state<='d0;
                        axi_wready<=1'b0;
                    end
               endcase
           end
        'd2://Reading
            begin
                case(r_state)
                'd0:
                    if(S_AXI_ARVALID)
                    begin
                        r_state<='d1;
                        axi_arready<=1'b0;
                        cmd_addr<=S_AXI_ARADDR[`log2CSR_REG_NUM+2-1:2];
                    end
                'd1:
                    begin
                        r_state<='d2;
                        cmd_vld<=1'b1;
                    end
                'd2:
                    if(cmd_rdy)
                    begin
                        r_state<='d3;
                        cmd_vld<=1'b0;
                    end
                'd3:
                    if(rsp_vld)
                    begin
                        r_state<='d4;
                        axi_rvalid<=1'b1;
                    end
                'd4:
                    if(S_AXI_RREADY)
                    begin
                        r_state<='d0;
                        state<='d0;
                        axi_rvalid<=1'b0;
                    end
                endcase
            end
    endcase

endmodule

