module SUPERIO_QSYS
(
	// Top Level Inputs into the system
	input CLOCK_50, /// System Clock
	output [14:0] hps_memory_mem_a,
	output [2:0] hps_memory_mem_ba,
	output hps_memory_mem_ck,
	output hps_memory_mem_ck_n,
	output hps_memory_mem_cke,
	output hps_memory_mem_cs_n,
	output hps_memory_mem_ras_n,
	output hps_memory_mem_cas_n,
	output hps_memory_mem_we_n,
	output hps_memory_mem_reset_n,
	inout [39:0] hps_memory_mem_dq,
	inout [4:0] hps_memory_mem_dqs,
	inout [4:0] hps_memory_mem_dqs_n,
	output hps_memory_mem_odt,
	output [4:0] hps_memory_mem_dm,
	input hps_memory_oct_rzqin,
	
	output [15:0] A,
	output AEN,
	inout [15:0] D,
	output DACK1,
	output DACK3,
	output DACK5,
	output DACK7,
	input DRQ1,
	input DRQ3,
	input DRQ5,
	input DRQ7,
	output IOR,
	output IOW,
	input IRQ2,
	input IRQ5,
	input IRQ7,
	input IRQ10,
	output RESET,
	
	output [3:0] state_out,
	output clk,
	
	input sw
	
);

wire clock_pll;

//PLL pll(
//		.refclk(CLOCK_50),   //  refclk.clk
//		.rst(!sw),      //   reset.reset
//		.outclk_0(clock_pll)  // outclk0.clk
//);

pll pll(
		.refclk(CLOCK_50),   //  refclk.clk
		.rst(!sw),      //   reset.reset
		.outclk_0(clock_pll)  // outclk0.clk
);

soc_system soc(
		.clk_clk(clock_pll),                                  //                       clk.clk
		.isa_superio_0_conduit_end_export_address(A), // isa_superio_0_conduit_end.export_address
		.isa_superio_0_conduit_end_export_aen(AEN),     //                          .export_aen
		.isa_superio_0_conduit_end_export_data(D),    //                          .export_data
		.isa_superio_0_conduit_end_export_dack1(DACK1),   //                          .export_dack1
		.isa_superio_0_conduit_end_export_dack3(DACK3),   //                          .export_dack3
		.isa_superio_0_conduit_end_export_dack5(DACK5),   //                          .export_dack5
		.isa_superio_0_conduit_end_export_dack7(DACK7),   //                          .export_dack7
		.isa_superio_0_conduit_end_export_drq1(DRQ1),    //                          .export_drq1
		.isa_superio_0_conduit_end_export_drq3(DRQ3),    //                          .export_drq3
		.isa_superio_0_conduit_end_export_drq5(DRQ5),    //                          .export_drq5
		.isa_superio_0_conduit_end_export_drq7(DRQ7),    //                          .export_drq7
		.isa_superio_0_conduit_end_export_ior(IOR),     //                          .export_ior
		.isa_superio_0_conduit_end_export_iow(IOW),     //                          .export_iow
		.isa_superio_0_conduit_end_export_irq2(IRQ2),    //                          .export_irq2
		.isa_superio_0_conduit_end_export_irq5(IRQ5),    //                          .export_irq5
		.isa_superio_0_conduit_end_export_irq7(IRQ7),    //                          .export_irq7
		.isa_superio_0_conduit_end_export_irq10(IRQ10),   //                          .export_irq10
		.isa_superio_0_conduit_end_export_reset(RESET),   //                          .export_reset
		.isa_superio_0_conduit_end_export_state_out(state_out),
		.isa_superio_0_conduit_end_export_clk(clk),       //                          .export_clk
		.memory_mem_a(hps_memory_mem_a),                             //                    memory.mem_a
		.memory_mem_ba(hps_memory_mem_ba),                            //                          .mem_ba
		.memory_mem_ck(hps_memory_mem_ck),                            //                          .mem_ck
		.memory_mem_ck_n(hps_memory_mem_ck_n),                          //                          .mem_ck_n
		.memory_mem_cke(hps_memory_mem_cke),                           //                          .mem_cke
		.memory_mem_cs_n(hps_memory_mem_cs_n),                          //                          .mem_cs_n
		.memory_mem_ras_n(hps_memory_mem_ras_n),                         //                          .mem_ras_n
		.memory_mem_cas_n(hps_memory_mem_cas_n),                         //                          .mem_cas_n
		.memory_mem_we_n(hps_memory_mem_we_n),                          //                          .mem_we_n
		.memory_mem_reset_n(hps_memory_mem_reset_n),                       //                          .mem_reset_n
		.memory_mem_dq(hps_memory_mem_dq),                            //                          .mem_dq
		.memory_mem_dqs(hps_memory_mem_dqs),                           //                          .mem_dqs
		.memory_mem_dqs_n(hps_memory_mem_dqs_n),                         //                          .mem_dqs_n
		.memory_mem_odt(hps_memory_mem_odt),                           //                          .mem_odt
		.memory_mem_dm(hps_memory_mem_dm),                            //                          .mem_dm
		.memory_oct_rzqin(hps_memory_oct_rzqin),                         //                          .oct_rzqin
		.reset_reset_n(sw)                             //                     reset.reset_n
);

endmodule
