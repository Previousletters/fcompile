//parameter N means the actual width of dividend
//using 29/5=5...4

module divider_main
  #(parameter N=16,
    parameter M=10,
    parameter N_ACT = M+N-1)
(
  input                     clk,
  input                     rstn,

  input                     en ,
  input [N-1:0]             dividend,
  input [M-1:0]             divisor,

  output                    res_rdy ,
  output [N-1:0]            quotient
);
wire [N-1:0]tp_quotient;
wire [N_ACT-M-1:0]   dividend_t [N_ACT-M:0] ;
wire [M-1:0]         divisor_t [N_ACT-M:0] ;
wire [M-1:0]         remainder_t [N_ACT-M:0];
wire [N_ACT-M:0]     rdy_t ;
wire [N_ACT-M:0]     quotient_t [N_ACT-M:0] ;


divider_cell      #(.N(N_ACT), .M(M))
u_divider_step0
 (
  .clk              (clk),
  .rstn             (rstn),
  //input
  .en               (en),
  .dividend         ({{(M){1'b0}}, dividend[N-1]}), //minimal cell to calculate
  .divisor          (divisor),                  //divisor
  .quotient_ci      ({(N_ACT-M+1){1'b0}}),   //quotient info from last cell
  .dividend_ci      (dividend[N_ACT-M-1:0]), //original dividend remained
  //output
  .dividend_kp      (dividend_t[N_ACT-M]),   //keep remaind-dividend for pipeline
  .divisor_kp       (divisor_t[N_ACT-M]),    //keep divisor for pipeline
  .rdy              (rdy_t[N_ACT-M]),
  .quotient         (quotient_t[N_ACT-M]),
  .remainder        (remainder_t[N_ACT-M])
  );

genvar               i ;
generate
  for(i=1; i<=N_ACT-M; i=i+1) begin: sqrt_stepx
     divider_cell      #(.N(N_ACT), .M(M))
     u_divider_step
     (
      .clk              (clk),
      .rstn             (rstn),
      //input
      .en               (rdy_t[N_ACT-M-i+1]),
      .dividend         ({remainder_t[N_ACT-M-i+1], dividend_t[N_ACT-M-i+1][N_ACT-M-i]}),
      .divisor          (divisor_t[N_ACT-M-i+1]),
      .quotient_ci      (quotient_t[N_ACT-M-i+1]),
      .dividend_ci      (dividend_t[N_ACT-M-i+1]),
      //output
      .divisor_kp       (divisor_t[N_ACT-M-i]),
      .dividend_kp      (dividend_t[N_ACT-M-i]),
      .rdy              (rdy_t[N_ACT-M-i]),
      .quotient         (quotient_t[N_ACT-M-i]),
      .remainder        (remainder_t[N_ACT-M-i])
      );
  end // block: sqrt_stepx
endgenerate

assign res_rdy         = rdy_t[0];
assign tp_quotient     = quotient_t[0];
wire [M-1:0]remainder  = remainder_t[0];
assign quotient=remainder>divisor[M-1:1]?(tp_quotient+1):tp_quotient;

endmodule