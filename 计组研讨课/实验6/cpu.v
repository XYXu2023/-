`timescale 10ns / 1ns
`define DATA_WIDTH 32
`define ADDR_WIDTH 5


module ALUop(
	input [5:0] opcode,
	input [5:0] func,
	output [2:0] ALUop
);
        
	assign ALUop = (
		opcode[5:3] == 3'b100 ||  
                opcode[5:3] == 3'b101
        )  ? 3'b010 :  
		(
		(opcode == 6'b000100) ||      // beq
		(opcode == 6'b000101) ||      //bne
		(opcode == 6'b000110) ||      // blez
		(opcode == 6'b000001) ||      // bltz,bgez
		(opcode == 6'b000111)      //bgtz
        ) ? 3'b110  :  (
		opcode == 6'b000000 &&  (   // R-type 指令
		func == 6'b001011 ||     // movn
		func == 6'b001010 )   // movz
	)  ? 3'b001 :   (
		opcode == 6'b000000 && func [5:2]== 4'b1000 
        )  ? {func[1],2'b10} :(
		opcode == 6'b000000 && func [5:2]== 4'b1001 
        )  ? {func[1],1'b0,func[0]} :  (
		opcode == 6'b000000 && func [5:2]== 4'b1010 
        )  ? {~func[0],2'b11} : (
		opcode[5:1] == 5'b00100
        )  ? {opcode[1],2'b10} : (
		opcode[5:2] == 4'b0011
        )  ? {opcode[1],1'b0,opcode[0]} :(
		opcode[5:1] == 5'b00101
        )  ? {~opcode[0],2'b11} :  
	3'b000 ;      
endmodule


module top_module(
	input             clk,
	input             rst,

	output [31:0]     PC,
	input  [31:0]     Instruction,

	output [31:0]     Address,
	output            MemWrite,
	output [31:0]     Write_data,
	output [ 3:0]     Write_strb,

	input  [31:0]     Read_data,
	output            MemRead
);

	// THESE THREE SIGNALS ARE USED IN OUR TESTBENCH
	// PLEASE DO NOT MODIFY SIGNAL NAMES
	// AND PLEASE USE THEM TO CONNECT PORTS
	// OF YOUR INSTANTIATION OF THE REGISTER FILE MODULE
	wire			RF_wen;
	wire [4:0]		RF_waddr;
	wire [31:0]		RF_wdata;

	// TODO: PLEASE ADD YOUR CODE BELOW


	wire [2:0] ALUSrc;     //选择寄存器数据还是立即数读入alu计算,高1位判断alu操作数A的选择，低两位判断alu操作数B的选择
	wire [31:0] rsdata;
	wire [31:0] rtdata;
	 
	
    wire [5:0] opcode =  Instruction[31:26];
	wire [4:0] rs     =  Instruction[25:21];
	wire [4:0] rt     =  Instruction[20:16];
	wire [4:0] rd     =  Instruction[15:11];
	wire [4:0] shamt  =  Instruction[10: 6];
	wire [5:0] func   =  Instruction[ 5: 0];
	wire [15:0] imm   =  Instruction[15: 0];
	wire [25:0] tar   =  Instruction[25: 0];

	wire [31:0] imm_SE = {{16{imm[15]}}, imm};  // 符号扩展
    wire [31:0] imm_0E = {16'b0, imm};          // 零扩展
	wire [31:0] imm_SE00 ={{14{imm[15]}},imm,2'b00};
    wire R_TYPE;
	wire REGIMM;
	wire J_TYPE;
	wire JR_TYPE;
	wire Branch;
	wire OP_I;
	wire LD;
	wire ST;

	assign R_TYPE = (opcode == 6'b000000)? 1:0;
    assign REGIMM = (opcode == 6'b000001) ?1:0;
    assign J_TYPE = (opcode == 6'b000010 || opcode == 6'b000011)?1:0;
    assign JR_TYPE = (R_TYPE && (func == 6'b001000 || func == 6'b001001))?1:0;
    assign Branch = (opcode[5:2] == 4'b0001) ?1:0;    //I-Type 分支指令​
    assign OP_I = (opcode[5:3] == 3'b001)   ?1:0;   //I-Type 计算指令​
    assign LD   = (opcode[5:3] == 3'b100) ?1:0;     //I-Type 内存读指令​
    assign ST   = (opcode[5:3] == 3'b101) ?1:0;     //I-Type 内存写指令​

//MemRead
    assign MemRead =opcode[5:3]== 3'b100 ?1 :0 ;
//Memwrite
    assign MemWrite =opcode[5:3]== 3'b101 ?1 : 0;


	assign RF_waddr = R_TYPE    ? rd  : 
	                (opcode == 6'b000011)? 5'b11111 :                // jal指令目标为31号寄存器
                                                 rt;


	//reg_file模块
	reg_file instance_reg_file(
		.clk(clk),
		.waddr(RF_waddr),
		.raddr1(rs),
		.raddr2(rt),
		.wen(RF_wen),
		.wdata(RF_wdata),
		.rdata1(rsdata),
		.rdata2(rtdata)
	);

//ALUSrc[2]为0时，第一个操作数取0，其余取rsdata;ALUSrc[1:0]为00时，第二个操作数B取0，为10时取imm（SE），为01时取imm（0E），为11时取rtdata
	assign ALUSrc [2]= (R_TYPE && (func[5:0]==6'b001011 ||func[5:0]==6'b001010)) ? 0: 1;

	assign ALUSrc [1:0]= ((R_TYPE &&func[5]==1)||opcode==6'b000100||opcode==6'b000101||(R_TYPE && (func[5:0]==6'b001011 || func[5:0]==6'b001010)))   ? 2'b11:
	                     (opcode==6'b000110)||(opcode==6'b000111)||(opcode==6'b000001)   ? 2'b00:
			     (opcode==6'b001100)||(opcode==6'b001101)||(opcode==6'b001110)   ? 2'b01:
			                                                                       2'b10;

	wire [31:0] A;
	wire [31:0] B;
	wire [2:0] ALUop;
	wire [31:0]alu_result;
	assign A= ALUSrc[2]==1'b0 ? 32'b0: rsdata ;


	assign B= ALUSrc[1:0]==2'b00 ? 32'b0:
	          ALUSrc[1:0]==2'b01 ?imm_0E:
	          ALUSrc[1:0]==2'b10 ?imm_SE:
			              rtdata;

        ALUop instance_ALUop(
		.opcode(opcode),
		.func(func),
        .ALUop(ALUop)
	);

	alu instance_alu(
		.A(A),
		.B(B),
		.ALUop(ALUop),
		.Zero(Zero),
		.Result(alu_result)
	);

//写使能RF_wen判断
        assign RF_wen = Branch||
	                REGIMM ||
	                ST ||
	                (opcode==6'b000010)  ||
	                (opcode==6'b000000 && func==6'b001000)         ||
	                (opcode==6'b000000 && func==6'b001011 && Zero) ||
	                (opcode==6'b000000 && func==6'b001010 && ~Zero) ?0 : 1 ;

	//shifter模块
	wire [4:0]shiftlength;
	wire [31:0]shifter_result;
	assign shiftlength = func[2]? rsdata[4:0] :shamt;
	shifter instance_shifter(
		.Shiftop(func[1:0]),
		.A(rtdata),
		.B(shiftlength),
		.Result(shifter_result)
	);

	//跳转指令
	wire is_jorb;  //记录是否跳转
	wire [31:0] next_pc;// 跳转地址
	assign is_jorb = 
    // 跳转指令（J/JR类型）
                (J_TYPE || JR_TYPE) ? 1'b1 : 
    // 分支指令（Branch类型）
                (Branch) ? (
                        (opcode[2:1] == 2'b10) ? 
                        (opcode[0] ^ Zero) : 
                        (opcode[0] ^ (alu_result[31] || Zero)) 
                ) : 
    // 寄存器条件跳转（REGIMM类型）
        (REGIMM && rt[4:0]==5'b00000 && alu_result==1) ? 1'b1 :
		(REGIMM && rt[4:0]==5'b00001 && alu_result==0) ? 1'b1 :
    // 默认不跳转
                                                         1'b0;

	wire [31:0] pc_add4;
	assign pc_add4 = PC+4;
	
	assign next_pc = 
                (Branch || REGIMM) ? (pc_add4 + imm_SE00) :  // 分支或 REGIMM 跳转
                (J_TYPE)? { pc_add4[31:28], tar, 2'b00 } :  // j/jal 绝对跳转
                                                     rsdata;  // 默认：jr/jalr 寄存器跳转

	//Address
	assign Address = {alu_result[31:2],2'b00};

	//I_type内存读
	wire [31:0]mem_data; //通过 RF_wdata 信号写入目标寄存器

        
    wire [31:0] lwr_data;
    wire [31:0] lb_mem_data;
    wire [31:0] lbu_mem_data;
    wire [31:0] lh_mem_data;
    wire [31:0] lhu_mem_data;
	wire [31:0] lwl_data;


        assign lwl_data = 
                (alu_result[1:0] == 2'b00) ? {Read_data[7:0],   rtdata[23:0]} :
                (alu_result[1:0] == 2'b01) ? {Read_data[15:0],  rtdata[15:0]} :
                (alu_result[1:0] == 2'b10) ? {Read_data[23:0],  rtdata[7:0]}  :
                                  Read_data;


assign lwr_data = 
    (alu_result[1:0] == 2'b00) ? Read_data :
    (alu_result[1:0] == 2'b01) ? {rtdata[31:24], Read_data[31:8]} :
    (alu_result[1:0] == 2'b10) ? {rtdata[31:16], Read_data[31:16]} :
                                  {rtdata[31:8],  Read_data[31:24]};


assign lb_mem_data = 
    (alu_result[1:0] == 2'b00) ? {{24{Read_data[7]}},  Read_data[7:0]}   :
    (alu_result[1:0] == 2'b01) ? {{24{Read_data[15]}}, Read_data[15:8]} :
    (alu_result[1:0] == 2'b10) ? {{24{Read_data[23]}}, Read_data[23:16]} :
                                  {{24{Read_data[31]}}, Read_data[31:24]};


assign lbu_mem_data = 
    (alu_result[1:0] == 2'b00) ? {24'h0, Read_data[7:0]}    :
    (alu_result[1:0] == 2'b01) ? {24'h0, Read_data[15:8]}   :
    (alu_result[1:0] == 2'b10) ? {24'h0, Read_data[23:16]}   :
                                  {24'h0, Read_data[31:24]};


assign lh_mem_data = 
    (alu_result[1:0] == 2'b00) ? {{16{Read_data[15]}}, Read_data[15:0]} :
                                  {{16{Read_data[31]}}, Read_data[31:16]};


assign lhu_mem_data = 
    (alu_result[1:0] == 2'b00) ? {16'h0, Read_data[15:0]}  :
                                  {16'h0, Read_data[31:16]};


assign mem_data = 
    (opcode[2:0] == 3'b000) ? lb_mem_data   :
    (opcode[2:0] == 3'b100) ? lbu_mem_data  :
    (opcode[2:0] == 3'b001) ? lh_mem_data   :
    (opcode[2:0] == 3'b101) ? lhu_mem_data  :
    (opcode[2:0] == 3'b011) ? Read_data     :
    (opcode[2:0] == 3'b010) ? lwl_data     :
                               lwr_data;    // 默认分支

	//RF_wdata的确定
	wire [31:0]imm_1600 ={imm, {16{1'b0}}};

	assign RF_wdata = opcode==6'b001111 ? imm_1600 :
	                  (R_TYPE && func[5:3]==3'b000) ? shifter_result :
			  J_TYPE || JR_TYPE ? PC+8 :
			  LD ? mem_data :
			  (R_TYPE && (func[5:0]==6'b001011 || func[5:0]==6'b001010) )? rsdata:
                          alu_result;

	//I_type内存写：Write_data（要写入内存的数据，通常来源于 rt 寄存器的值（rtdata），但需要根据指令类型和地址对齐进行 ​​移位或拼接​​。）和Write strb（4-bit 信号，表示内存的哪些字节需要被写入（每个 bit 对应一个字节）。
//Write_strb[i] = 1 → 内存地址 address + i 的字节会被写入）的确定
 
        assign Write_data = opcode == 6'b101000 ? rtdata << (8 * alu_result[1:0]) :
                   opcode == 6'b101001 ? rtdata << (16 * alu_result[1]) :
                   opcode == 6'b101011 ? rtdata :
                   (opcode == 6'b101010 && alu_result[1:0] == 2'b00) ? {24'b0, rtdata[31:24]} :
                   (opcode == 6'b101010 && alu_result[1:0] == 2'b01) ? {16'b0, rtdata[31:16]} :
                   (opcode == 6'b101010 && alu_result[1:0] == 2'b10) ? {8'b0, rtdata[31:8]} :
                   (opcode == 6'b101010 && alu_result[1:0] == 2'b11) ? rtdata :
                   (opcode == 6'b101110 && alu_result[1:0] == 2'b00) ? rtdata :
                   (opcode == 6'b101110 && alu_result[1:0] == 2'b01) ? {rtdata[23:0],8'b0} :
                   (opcode == 6'b101110 && alu_result[1:0] == 2'b11) ? {rtdata[7:0],24'b0} :
                   {rtdata[15:0],16'b0};

	wire [3:0] strb_1;
	wire [3:0] strb_2;
	wire [3:0] strb_3;
	wire [3:0] strb_4;
	wire [3:0] strb_5;

	assign strb_1[0] = (alu_result[1:0] == 2'b00);
	assign strb_1[1] = (alu_result[1:0] == 2'b01);
	assign strb_1[2] = (alu_result[1:0] == 2'b10);
	assign strb_1[3] = (alu_result[1:0] == 2'b11);

	assign strb_2[1:0] = {2{(alu_result[1] == 1'b0)}};
	assign strb_2[3:2] = {2{(alu_result[1] == 1'b1)}};

	assign strb_3 = 4'b1111;

	assign strb_4 = 4'b1111 >> (3-alu_result[1:0]);

	assign strb_5 = 4'b1111 << alu_result[1:0];

        assign Write_strb = opcode == 6'b101000 ? strb_1 :
                            opcode == 6'b101001 ? strb_2 :
                            opcode == 6'b101011 ? strb_3 :
                            opcode == 6'b101010 ? strb_4 :
			                          strb_5 ;
	
		     
	//程序计数器
	reg[31:0]reg_pc;
	always @(posedge clk or posedge rst) begin
                if (rst) begin
                        reg_pc <= 32'b0;          // 复位时 PC 设为初始地址
                end else begin
                        if (is_jorb) begin
                                reg_pc <= next_pc;        // 发生跳转时更新为 next_pc
                        end else begin
                        reg_pc <= pc_add4;         // 顺序执行时 PC += 4
                        end 
                end
        end

	assign PC = reg_pc;

endmodule


module alu(
    input [`DATA_WIDTH - 1:0] A,
	input [`DATA_WIDTH - 1:0] B,
	input [2:0] ALUop,
	output Overflow,
	output CarryOut,
	output Zero,
	output [`DATA_WIDTH - 1:0] Result
);
    // 控制信号
    wire subtract = (ALUop == 3'b110 || ALUop == 3'b111 || ALUop == 3'b011);
    
    // 统一加法器逻辑
    wire [31:0] B_sel = subtract ? ~B : B;
    wire [32:0] sum_ext = {1'b0, A} + {1'b0, B_sel} + subtract;
    
    // 符号位处理
    wire sign_A = A[31];
    wire sign_B = B[31];
    
    // 进位标志优化
    wire B_is_zero = ~(|B);
    assign CarryOut = (ALUop == 3'b010) ? sum_ext[32] : 
                     (ALUop == 3'b110 || ALUop== 3'b011) ? (B_is_zero ? 1'b0 : ~sum_ext[32]) : 1'b0;

    // 溢出判断改进
    wire op_add = (ALUop == 3'b010);
    wire op_sub = (ALUop == 3'b110);
    wire same_sign_add = (sign_A == (subtract ? ~sign_B : sign_B));
    wire overflow = (same_sign_add && (sum_ext[31] != sign_A));
     // 当操作数符号相同但结果符号不同时触发
    assign Overflow = (op_add | op_sub) ? overflow : 1'b0;
    //仅在加减法操作有效

    // 有符号比较
    wire slt_sign = (sign_A ^ sign_B) ? sign_A : 
                   (sum_ext[31] ^ (overflow & op_sub));
                   // 符号不同时直接比较原始符号, 符号相同时结合结果符号和溢出修正
    wire [31:0] result_slt = {31'b0, slt_sign};

     // 无符号比较（SLTU）
    wire [31:0] result_sltu = {31'b0, CarryOut};

    // 结果选择
    assign Result = (ALUop == 3'b000) ? (A & B) :
                    (ALUop == 3'b001) ? (A | B) :
                    (ALUop == 3'b100) ? (A ^ B) :       // XOR
                    (ALUop == 3'b101) ? ~(A | B) :     // NOR
                    (op_add | op_sub) ? sum_ext[31:0] :
                    (ALUop == 3'b011) ? result_sltu :   // SLTU
                    (ALUop == 3'b111) ? result_slt : 32'b0;

    // 零标志
    assign Zero = ~(|Result);

endmodule



// -------------------- 移位器模块（支持变量移位）--------------------

module shifter (
	input  [`DATA_WIDTH - 1:0] A,
	input  [              4:0] B,
	input  [              1:0] Shiftop,
	output [`DATA_WIDTH - 1:0] Result
);
	// TODO: Please add your logic code here

	// 算术右移符号位处理
        wire [`DATA_WIDTH - 1:0] arith_shift_mask = 
        (B == 0) ? 0 :  // B=0 时无需填充
        ({32{A[31]}} & ~((32'b1 << (32 - B)) - 1)); // 生成高 B 位为符号位的掩码
	

// 使用条件运算符实现多路选择器
        assign Result = (Shiftop == 2'b00) ? (A << B) :      // 左移
               (Shiftop == 2'b10) ? (A >> B) :       // 逻辑右移
               (Shiftop == 2'b11) ? ((A >> B) | arith_shift_mask) : // 算术右移
               32'b0;                                // 默认值

	
endmodule

module reg_file(
	input clk,
	input rst,
	input [`ADDR_WIDTH - 1:0] waddr,
	input [`ADDR_WIDTH - 1:0] raddr1,
	input [`ADDR_WIDTH - 1:0] raddr2,
	input wen,
	input [`DATA_WIDTH - 1:0] wdata,
	output [`DATA_WIDTH - 1:0] rdata1,
	output [`DATA_WIDTH - 1:0] rdata2
);

	// TODO: Please add your logic code here
	reg [`DATA_WIDTH-1:0] regfile[0:(1<<`ADDR_WIDTH)-1];
	assign rdata1=(raddr1==0)?32'b0:regfile[raddr1];
	assign rdata2=(raddr2==0)?32'b0:regfile[raddr2];

	always @(posedge clk) begin
		if (wen==1&&(waddr!=0)) begin
			regfile[waddr]<=wdata;
		end
	end

endmodule