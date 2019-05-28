
module SUPERIO_QSYS(
	// Top Level Inputs into the system
	input 	CLOCK_50, /// System Clock
	output 	[14:0] hps_memory_mem_a,
	output 	[2:0] hps_memory_mem_ba,
	output 	hps_memory_mem_ck,
	output 	hps_memory_mem_ck_n,
	output 	hps_memory_mem_cke,
	output 	hps_memory_mem_cs_n,
	output 	hps_memory_mem_ras_n,
	output 	hps_memory_mem_cas_n,
	output 	hps_memory_mem_we_n,
	output 	hps_memory_mem_reset_n,
	inout 	[39:0] hps_memory_mem_dq,
	inout 	[4:0] hps_memory_mem_dqs,
	inout 	[4:0] hps_memory_mem_dqs_n,
	output 	hps_memory_mem_odt,
	output 	[4:0] hps_memory_mem_dm,
	input 	hps_memory_oct_rzqin,
	
	// I2C Specific I/O
    inout   [15:0]  D,
    output  [15:0]  A,
	 output          IOW,
    output          IOR
);

wire main_clk = CLOCK_50;

soc_system soc(
	.memory_mem_a (hps_memory_mem_a),
	.memory_mem_ba (hps_memory_mem_ba),
	.memory_mem_ck (hps_memory_mem_ck),
	.memory_mem_ck_n (hps_memory_mem_ck_n),
	.memory_mem_cke (hps_memory_mem_cke),
	.memory_mem_cs_n (hps_memory_mem_cs_n),
	.memory_mem_ras_n (hps_memory_mem_ras_n),
	.memory_mem_cas_n (hps_memory_mem_cas_n),
	.memory_mem_we_n (hps_memory_mem_we_n),
	.memory_mem_reset_n (hps_memory_mem_reset_n),
	.memory_mem_dq (hps_memory_mem_dq),
	.memory_mem_dqs (hps_memory_mem_dqs),
	.memory_mem_dqs_n (hps_memory_mem_dqs_n),
	.memory_mem_odt (hps_memory_mem_odt),
	.memory_mem_dm (hps_memory_mem_dm),
	.memory_oct_rzqin (hps_memory_oct_rzqin),
	
	.clk_clk(main_clk),
	.isa_bus_0_conduit_end_export_busaddr(A), // isa_bus_0_conduit_end.export_busaddr
	.isa_bus_0_conduit_end_export_data(D),    //                      .export_data
	.isa_bus_0_conduit_end_export_iow(IOW),     //                      .export_iow
	.isa_bus_0_conduit_end_export_ior(IOR)     //                      .export_ior
	
);



endmodule 