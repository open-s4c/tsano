# Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
# SPDX-License-Identifier: MIT

ROOT!=		pwd
WORK=		$(ROOT)/work
DICE!=		readlink -f $(ROOT)/../..
BINDIR=		$(DICE)/build/bench/micro

OUTPARSE= 	's/.*=\([0-9.]*\) .*=\([0-9.]*\) .*=\([0-9.]*\)s/\1,\2,\3/g'
FORMAT=		grep elapsed | sed $(OUTPARSE) \
		| xargs printf '%s,%s\n' $*

# ------------------------------------------------------------------------------
# build dice
# ------------------------------------------------------------------------------

TARGET+=	dice
CFG.dice=	cmake $(DICE) \
		-DCMAKE_BUILD_TYPE=Release \
		-DCMAKE_INSTALL_PREFIX=$(WORK)
BLD.dice=	$(MAKE) install

# ------------------------------------------------------------------------------
# variants
# ------------------------------------------------------------------------------

LIBPATH=	$(WORK)/lib:$(WORK)/lib/dice

TARGET+=	micro
DEP.micro=	.dice.bld
RUN.micro=	env LD_LIBRARY_PATH=$(LIBPATH) $(BINDIR)/micro
PRO.micro=	cat $(ROOT)/$*.run.log | $(FORMAT) >> $(ROOT)/results.dat

TARGET+=	micro2
DEP.micro2=	.dice.bld
RUN.micro2=	env LD_LIBRARY_PATH=$(LIBPATH) $(BINDIR)/micro2
PRO.micro2=	cat $(ROOT)/$*.run.log | $(FORMAT) >> $(ROOT)/results.dat

TARGET+=	micro3
DEP.micro3=	.dice.bld
RUN.micro3=	env LD_LIBRARY_PATH=$(LIBPATH) $(BINDIR)/micro3
PRO.micro3=	cat $(ROOT)/$*.run.log | $(FORMAT) >> $(ROOT)/results.dat

# ------------------------------------------------------------------------------
include ../bench.mk
