#include "generator.h"
#include "ensitlm.h"
#include "LCDC.h"
#include "LCDC_registermap.h"
#include "ROM.h"

using namespace std;

const ensitlm::addr_t MEMORY_BASE_ADDR = 0x10000000;
const ensitlm::addr_t MEMORY_VIDEO_BASE_ADDR = 0x10002800;
const ensitlm::addr_t LCDC_BASE_ADDR = 0x00000000;

Generator::Generator(sc_core::sc_module_name name) : sc_core::sc_module(name) {
	SC_THREAD(process);
	SC_METHOD(wait_interrupt);
	sensitive << itrp;
}
void Generator::process(){
	int IMG_SIZE = 320 * 240/4;
	Generator::initiator.write(LCDC_START_REG,0x00000001);
	Generator::initiator.write(LCDC_ADDR_REG, MEMORY_VIDEO_BASE_ADDR);
	cpfromROM(); //envoi de l'image de la ROM à vecteur_rom
	while(true){
		for (int i = 0; i < IMG_SIZE; i++) {
			/* envoi de l'iùage de vecteur_rom à la mémoire vidéo*/
			Generator::initiator.write(MEMORY_VIDEO_BASE_ADDR + 4 * i, vecteur_rom[i]);
		}
		wait(interruption);
		cout << "prise en charge de l'interruption..." << endl;
		wait(1, SC_SEC);
		/*rotation de vecteur_rom: déplacement des pixels de 320 positions
		=> image décalée vers le bas et partie tronquée envoyée en haut*/
		rotate(vecteur_rom.begin(), vecteur_rom.begin() + 320,
				    vecteur_rom.end());
	}
}

void Generator::cpfromROM() {
	ensitlm::data_t valeur = 0x00;
	ensitlm::data_t valeur_interm = 0x00;
	for (int i =0; i < ROM_SIZE/4; i++) {
		initiator.read(ROM_BASE_ADDR + 4 * i, valeur);
		valeur_interm = (valeur & 0xF0000000) | ((valeur & 0x0F000000) >> 4)
						      | ((valeur & 0x00F00000) >> 8)
						      | ((valeur & 0x000F0000) >> 12);
		vecteur_rom.push_back(valeur_interm);
		valeur_interm = ((valeur & 0x0000F000) << 16) | ((valeur & 0x00000F00) << 12)
						              | ((valeur & 0x000000F0) << 8 )
						              | ((valeur & 0x0000000F) << 4);
		vecteur_rom.push_back(valeur_interm);
	}
}

void Generator::wait_interrupt() {
	interruption.notify();
	/* to deassert the interruption */
	Generator::initiator.write(LCDC_INT_REG, 0x00000000);

}
