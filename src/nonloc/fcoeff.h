#pragma once
#include "../common/dtypes.h"
#include "../elphC.h"

void alloc_and_Compute_f_Coeff(struct Lattice* lattice, struct Pseudo* pseudo);
void free_f_Coeff(struct Lattice* lattice, struct Pseudo* pseudo);
