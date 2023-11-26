#include "verilated.h"
#include "verilated_vcd_c.h"
#include "obj_dir/Vmux21.h"

VerilatedContext* contextp = NULL;
VerilatedVcdC* tfp = NULL;

static Vmux21* top;

void step_and_dump_wave(){
  top->eval();
  contextp->timeInc(1);
  tfp->dump(contextp->time());
}
void sim_init(){
  contextp = new VerilatedContext;
  tfp = new VerilatedVcdC;
  top = new Vmux21;
  contextp->traceEverOn(true);
  top->trace(tfp, 0);
  tfp->open("dump.vcd");
}

void sim_exit(){
  step_and_dump_wave();
  tfp->close();
}

int main() {
  sim_init();
  top->s=0b00;  top->a=0b00000000;  step_and_dump_wave();
                top->a=0b00000001;  step_and_dump_wave();
				top->a=0b00000010;	step_and_dump_wave();
				top->a=0b00000011;	step_and_dump_wave();
  top->s=0b01;  top->a=0b00001000;  step_and_dump_wave();
                top->a=0b00000101;  step_and_dump_wave();
				top->a=0b00001110;	step_and_dump_wave();
				top->a=0b00000011;	step_and_dump_wave();
  top->s=0b10;  top->a=0b00000000;  step_and_dump_wave();
                top->a=0b00010101;  step_and_dump_wave();
				top->a=0b00111110;	step_and_dump_wave();
				top->a=0b00101011;	step_and_dump_wave();

  top->s=0b11;  top->a=0b11000000;  step_and_dump_wave();
                top->a=0b10000101;  step_and_dump_wave();
				top->a=0b01001110;	step_and_dump_wave();
				top->a=0b00001011;	step_and_dump_wave();

  sim_exit();
}
