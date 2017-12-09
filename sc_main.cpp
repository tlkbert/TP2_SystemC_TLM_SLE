#include "bus.h"
#include "generator.h"
#include "ensitlm.h"
#include "memory.h"
#include "LCDC.h"
#include "ROM.h"

using namespace sc_core;

int sc_main(int argc, char** argv) {
	sc_signal<bool, SC_MANY_WRITERS> irq_signal ("IRQ");

	/* instanciations*/
	Generator gen("Generator");
	Memory mem("Memory", 87040);
	Bus bus("Bus");
	LCDC controller("LCDC", sc_time(1.0 / 25, SC_SEC));
	ROM rom_memory("ROM");

	/* connexions*/
	gen.initiator.bind(bus.target);
	bus.initiator.bind(mem.target);
	bus.initiator.bind(controller.target_socket);
	controller.initiator_socket.bind(bus.target);
	controller.display_int.bind(irq_signal);
	gen.itrp.bind(irq_signal);
	bus.initiator.bind(rom_memory.socket);

	/*mapping mémoire*/
	bus.map(mem.target, 0x10000000, 0x10015400);
	bus.map(controller.target_socket, 0x00000000, 0x0C);
	bus.map(rom_memory.socket, ROM_BASE_ADDR, 0x9610); //plage d'adresses 38400 octets

	/*simulation*/
	sc_start();

	return 0;
}
