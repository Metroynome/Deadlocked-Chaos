#include <tamtypes.h>
#include <kernel.h>
#include <libdl/dl.h>
#include <libdl/player.h>
#include <libdl/pad.h>
#include <libdl/game.h>
#include <libdl/string.h>
#include <libdl/stdio.h>
#include <libdl/gamesettings.h>
#include <libdl/graphics.h>
#include <libdl/ui.h>
#include <libdl/math.h>
#include <libdl/math3d.h>
#include <libdl/hud.h>
#include <libdl/music.h>
#include <libdl/weapon.h>
#include <libdl/team.h>

#define EngineAddr 0x80047010

extern void codes;
extern u32 size_codes;

u32 HookAddr = 0x800001A0;
u32 HookValue = (((int)EngineAddr << 4) >> 6) + 0x08000000;
u32 KernelCodesAddr = 0x80050000;
u32 NonKernelCodesAddr = 0x000F0000;

u32 Engine[] = {
	0x27BDFE00,
	0x7FA10000,
	0x7FA20010,
	0x7FA30020,
	0x7FA40030,
	0x7FA50040,
	0x7FA60050,
	0x7FA70060,
	0x7FA80070,
	0x7FA90080,
	0x7FAA0090,
	0x7FAB00A0,
	0x7FAC00B0,
	0x7FAD00C0,
	0x7FAE00D0,
	0x7FAF00E0,
	0x7FB000F0,
	0x7FB10100,
	0x7FB20110,
	0x7FB30120,
	0x7FB40130,
	0x7FB50140,
	0x7FB60150,
	0x7FB70160,
	0x7FB80170,
	0x7FB90180,
	0x7FBA0190,
	0x7FBB01A0,
	0x7FBE01B0,
	0x7FBC01C0,
	0x7FBF01D0,
	0x3C018004,
	0x34217010,
	0x8C39FFF8,
	0x8F390000,
	0x1320000B,
	0x00000000,
	0x3C0B0012,
	0x356B74AC,
	0x3C0C0803,
	0x358CC000,
	0x8D6D0000,
	0x11A00008,
	0x00000000,
	0x11AC0006,
	0x00000000,
	0xAD6C0000,
	0x8C24FFF8,
	0x8C25FFF0,
	0x0C011C57,
	0x8C26FFF4,
	0x7BA10000,
	0x7BA20010,
	0x7BA30020,
	0x7BA40030,
	0x7BA50040,
	0x7BA60050,
	0x7BA70060,
	0x7BA80070,
	0x7BA90080,
	0x7BAA0090,
	0x7BAB00A0,
	0x7BAC00B0,
	0x7BAD00C0,
	0x7BAE00D0,
	0x7BAF00E0,
	0x7BB000F0,
	0x7BB10100,
	0x7BB20110,
	0x7BB30120,
	0x7BB40130,
	0x7BB50140,
	0x7BB60150,
	0x7BB70160,
	0x7BB80170,
	0x7BB90180,
	0x7BBA0190,
	0x7BBB01A0,
	0x7BBE01B0,
	0x7BBC01C0,
	0x7BBF01D0,
	0x03400008,
	0x27BD0200,
	0x0080402D,
	0x2CC20020,
	0x1440001c,
	0x0100182D,
	0x00A81025,
	0x3042000F,
	0x54400019,
	0x24C6FFFF,
	0x0100382D,
	0x78A30000,
	0x24C6FFE0,
	0x24A50010,
	0x2CC40020,
	0x7CE30000,
	0x24E70010,
	0x78A20000,
	0x24A50010,
	0x7CE20000,
	0x1080fff6,
	0x24E70010,
	0x2CC20008,
	0x14400009,
	0x00E0182D,
	0xDCA30000,
	0x24C6FFF8,
	0x24A50008,
	0x2CC20008,
	0xFCE30000,
	0x1040fffa,
	0x24E70008,
	0x00E0182D,
	0x24C6FFFF,
	0x2402FFFF,
	0x10C20007,
	0x0040202D,
	0x90A20000,
	0x24C6FFFF,
	0x24A50001,
	0xA0620000,
	0x14C4fffb,
	0x24630001,
	0x03E00008,
	0x0100102D,

};

void HookKernel()
{
	DI();
	ee_kmode_enter();
		// copy Engline into EngineAddr
		memcpy((u32*)(int)EngineAddr, Engine, sizeof(Engine));
		// memset size of codes - 0x1000 because the first 0x1000 bytes of codes is not needed.
		memset((u32*)KernelCodesAddr, 0, (size_codes - 0x1000));
		// copy codes starting at offset +0x1000 to the end into CodeAddr
		memcpy((u8*)KernelCodesAddr, (u8*)(&codes + 0x1000), (size_codes - 0x1000));
		// Set KernelCodesAddr Pointer for Engine.
		*(u32*)((int)EngineAddr - 0x10) = KernelCodesAddr;
		// set size_codes information for Engine
		*(u32*)((int)EngineAddr - 0xc) = (size_codes - 0x1000);
		// Set NonKernelCodeAddr Pointer for Engine.
		*(u32*)((int)EngineAddr - 0x8) = NonKernelCodesAddr;

		*(u32*)HookAddr = HookValue;
	ee_kmode_exit();
	EI();
}

int main(void)
{
	// if Music is not Loaded, stop.
	if (*(u32*)0x001CF85C != 0x000F8D29)
		return -1;

	// Patch.bin hook: 0x00138DD0
	// run original jal that patch.bin hook took over
	((void (*)(void))0x001270C0)();

	HookKernel();

	return 0;
}
