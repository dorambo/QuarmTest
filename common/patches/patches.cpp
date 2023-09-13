
#include "../global_define.h"
#include "patches.h"

#include "mac.h"
#include "evo.h"

void RegisterAllPatches(EQStreamIdentifier &into) {
	Mac::Register(into);
	Evo::Register(into);

}

void ReloadAllPatches() {
	Mac::Reload();
	Evo::Reload();
}

