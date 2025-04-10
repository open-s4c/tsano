# Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
# SPDX-License-Identifier: MIT

# ------------------------------------------------------------------------------
# Commands and variables for all LevelDB benchmark variants
# ------------------------------------------------------------------------------
ROOT!=		pwd
DBDIR=		/tmp/bench.db
COMMAND=	./db_bench --db=$(DBDIR)
CONFIGURE=	cmake -S ../../leveldb -DCMAKE_BUILD_TYPE=Release
BUILD= 		$(MAKE)
PREPARE= 	rm -rf $(DBDIR) && $(COMMAND) --threads=1 --benchmarks=fillseq
RUNCMD= 	$(COMMAND) --threads=1 --benchmarks=readrandom --duration=10
PARSE=		cat $(ROOT)/$*.run.log  \
		| grep benchstats \
		| cut -d: -f2 \
		| xargs -n1 echo "$*"';' \
		| tee -a $(ROOT)/results.csv

# ------------------------------------------------------------------------------
# clone level, apply patches, build bingo
# ------------------------------------------------------------------------------

URL=		https://github.com/google/leveldb.git
SHA=		068d5ee1a3ac40dabd00d211d5013af44be55bea

TARGET+=	clone
DIR.clone=	leveldb
PRE.clone=	git clone --recurse-submodules $(URL) .
RUN.clone=	git checkout $(SHA)

TARGET+=	patch
DEP.patch=	.clone.run
DIR.patch=	leveldb
RUN.patch=	git checkout . && git apply $(ROOT)/patches/01-leveldb

TARGET+=	lib
CFG.lib=	cmake -S$(ROOT)/.. -DCMAKE_BUILD_TYPE=Release
BLD.lib=	$(MAKE) -C lib && $(MAKE) -C mod

# ------------------------------------------------------------------------------
# Variants
# ------------------------------------------------------------------------------

TARGET+=	baseline
DEP.baseline=	.lib.bld .patch.run
DIR.baseline=	build/leveldb
CFG.baseline= 	$(CONFIGURE)
BLD.baseline=	$(BUILD)
PRE.baseline= 	$(PREPARE)
RUN.baseline= 	$(RUNCMD)
PRO.baseline=	$(PARSE)

TARGET+=	tsan
DEP.tsan=	.baseline.pre
DIR.tsan=	build/leveldb-tsan
CFG.tsan= 	$(CONFIGURE) -DCMAKE_CXX_FLAGS=-fsanitize=thread
BLD.tsan=	$(BUILD)
RUN.tsan=	env TSAN_OPTIONS=report_bugs=0 $(RUNCMD)
PRO.tsan=	$(PARSE)

TARGET+=	tsano
DEP.tsano=	.tsan.bld
DIR.tsano=	$(DIR.tsan)
RUN.tsano=	tsano $(RUNCMD)
PRO.tsano=	$(PARSE)

TARGET+=	bingo
DEP.bingo=	.tsan.bld
DIR.bingo=	$(DIR.tsan)
RUN.bingo=	bingo -pthread -mutex -tsan $(RUNCMD)
PRO.bingo=	$(PARSE)

TARGET+=	bingo-bund
DEP.bingo-bund=	.tsan.bld
DIR.bingo-bund=	$(DIR.tsan)
RUN.bingo-bund=	bingo -bundle $(RUNCMD)
PRO.bingo-bund=	$(PARSE)

# ------------------------------------------------------------------------------
include bench.mk
