module AXI_GP_Master_CPU #
(
    parameter M_AXI_DATA_WIDTH=32,
    parameter M_AXI_ADDR_WIDTH=32 
)
(
    input M_AXI_ACLK,
    input M_AXI_ARESETN,
    
    //AW channel
    output reg M_AXI_AWVALID,
    input M_AXI_AWREADY,
    output reg [M_AXI_ADDR_WIDTH-1:0]M_AXI_AWADDR,
    output [2:0]M_AXI_AWPROT,//=3'h0

    //Wr channel
    output reg [M_AXI_DATA_WIDTH-1:0]M_AXI_WDATA,
    output reg M_AXI_WVALID,
    input M_AXI_WREADY,
    output [M_AXI_DATA_WIDTH/8-1:0]M_AXI_WSTRB,//={(M_AXI_DATA_WIDTH/8){1'b1}}
    input [1:0]M_AXI_BRESP,//ignore
    input M_AXI_BVALID,
    output reg M_AXI_BREADY,        

    //AR channel
    output reg M_AXI_ARVALID,
    input M_AXI_ARREADY,
    output reg [M_AXI_ADDR_WIDTH-1:0]M_AXI_ARADDR,
    output [2:0]M_AXI_ARPROT,//=3'b0

    //Rd channel
    input [M_AXI_DATA_WIDTH-1:0]M_AXI_RDATA,
    input [1:0]M_AXI_RRESP,//ignore
    input M_AXI_RVALID,
    output reg M_AXI_RREADY
);

assign M_AXI_AWPROT=3'h0;
assign M_AXI_WSTRB={(M_AXI_DATA_WIDTH/8){1'b1}};
assign M_AXI_ARPROT=3'b0;

always @(negedge M_AXI_ARESETN)
begin
    M_AXI_AWVALID<=0;
    M_AXI_AWADDR<=0;
    M_AXI_WDATA<=0;
    M_AXI_WVALID<=0;
    M_AXI_ARVALID<=0;
    M_AXI_ARADDR<=0;
    M_AXI_RREADY<=0;
end

always @(posedge M_AXI_ACLK or negedge M_AXI_ARESETN)                                                                                                     
if(~M_AXI_ARESETN)                                                           
    M_AXI_BREADY<=1'b0;                                                                                                                          
else
    if(M_AXI_BVALID & ~M_AXI_BREADY)                                                                               
        M_AXI_BREADY<=1'b1;                                                                                                                                       
    else
        if(M_AXI_BVALID & M_AXI_BREADY)
            M_AXI_BREADY<=1'b0;

task Xil_Out32(input int unsigned addr,data);
begin
    @(posedge M_AXI_ACLK) M_AXI_AWVALID<=1;M_AXI_AWADDR<=addr;M_AXI_WVALID<=1;M_AXI_WDATA<=data;
    @(posedge M_AXI_ACLK);
    while((M_AXI_WVALID|M_AXI_AWVALID)==1)
        begin
            if(M_AXI_AWREADY)
                M_AXI_AWVALID<=0;
            if(M_AXI_WREADY)
                M_AXI_WVALID<=0;
            @(posedge M_AXI_ACLK);
        end
    while(~(M_AXI_BVALID && M_AXI_BREADY)) @(posedge M_AXI_ACLK);
end
endtask

task Xil_In32(input int unsigned addr,output int unsigned data);
begin
    M_AXI_ARADDR<=addr;
    @(posedge M_AXI_ACLK) M_AXI_ARVALID<=1;
    @(posedge M_AXI_ACLK);
    while(~(M_AXI_ARVALID && M_AXI_ARREADY)) @(posedge M_AXI_ACLK);
    M_AXI_ARVALID<=0;
    @(posedge M_AXI_ACLK);
    while(~M_AXI_RVALID) @(posedge M_AXI_ACLK);
    M_AXI_RREADY<=1;
    @(posedge M_AXI_ACLK) M_AXI_RREADY<=0;
    data=M_AXI_RDATA;
end
endtask

endmodule

