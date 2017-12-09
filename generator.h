#include "ensitlm.h"

using namespace sc_core;

SC_MODULE(Generator) {
	ensitlm::initiator_socket<Generator, false> initiator;
	std::vector<ensitlm::addr_t> vecteur_rom;
	sc_in<bool> itrp;
	sc_event interruption;
	SC_CTOR(Generator);
	void process();
	void cpfromROM();
	void wait_interrupt();
};
