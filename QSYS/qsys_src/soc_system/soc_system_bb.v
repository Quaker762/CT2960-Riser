
module soc_system (
	clk_clk,
	reset_reset_n,
	memory_mem_a,
	memory_mem_ba,
	memory_mem_ck,
	memory_mem_ck_n,
	memory_mem_cke,
	memory_mem_cs_n,
	memory_mem_ras_n,
	memory_mem_cas_n,
	memory_mem_we_n,
	memory_mem_reset_n,
	memory_mem_dq,
	memory_mem_dqs,
	memory_mem_dqs_n,
	memory_mem_odt,
	memory_mem_dm,
	memory_oct_rzqin,
	isa_superio_0_conduit_end_export_address,
	isa_superio_0_conduit_end_export_aen,
	isa_superio_0_conduit_end_export_data,
	isa_superio_0_conduit_end_export_dack1,
	isa_superio_0_conduit_end_export_dack3,
	isa_superio_0_conduit_end_export_dack5,
	isa_superio_0_conduit_end_export_dack7,
	isa_superio_0_conduit_end_export_drq1,
	isa_superio_0_conduit_end_export_drq3,
	isa_superio_0_conduit_end_export_drq5,
	isa_superio_0_conduit_end_export_drq7,
	isa_superio_0_conduit_end_export_ior,
	isa_superio_0_conduit_end_export_iow,
	isa_superio_0_conduit_end_export_irq2,
	isa_superio_0_conduit_end_export_irq5,
	isa_superio_0_conduit_end_export_irq7,
	isa_superio_0_conduit_end_export_irq10,
	isa_superio_0_conduit_end_export_reset);	

	input		clk_clk;
	input		reset_reset_n;
	output	[12:0]	memory_mem_a;
	output	[2:0]	memory_mem_ba;
	output		memory_mem_ck;
	output		memory_mem_ck_n;
	output		memory_mem_cke;
	output		memory_mem_cs_n;
	output		memory_mem_ras_n;
	output		memory_mem_cas_n;
	output		memory_mem_we_n;
	output		memory_mem_reset_n;
	inout	[7:0]	memory_mem_dq;
	inout		memory_mem_dqs;
	inout		memory_mem_dqs_n;
	output		memory_mem_odt;
	output		memory_mem_dm;
	input		memory_oct_rzqin;
	output	[15:0]	isa_superio_0_conduit_end_export_address;
	output		isa_superio_0_conduit_end_export_aen;
	inout	[15:0]	isa_superio_0_conduit_end_export_data;
	output		isa_superio_0_conduit_end_export_dack1;
	output		isa_superio_0_conduit_end_export_dack3;
	output		isa_superio_0_conduit_end_export_dack5;
	output		isa_superio_0_conduit_end_export_dack7;
	input		isa_superio_0_conduit_end_export_drq1;
	input		isa_superio_0_conduit_end_export_drq3;
	input		isa_superio_0_conduit_end_export_drq5;
	input		isa_superio_0_conduit_end_export_drq7;
	output		isa_superio_0_conduit_end_export_ior;
	output		isa_superio_0_conduit_end_export_iow;
	input		isa_superio_0_conduit_end_export_irq2;
	input		isa_superio_0_conduit_end_export_irq5;
	input		isa_superio_0_conduit_end_export_irq7;
	input		isa_superio_0_conduit_end_export_irq10;
	inout		isa_superio_0_conduit_end_export_reset;
endmodule
