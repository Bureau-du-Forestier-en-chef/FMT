# Copyright (c) 2019 Gouvernement du Québec
#
# SPDX-License-Identifier: LiLiQ-R-1.1
# License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt

from FMT import Parallel


def testParallelObjectsCanBeConstructed():
    constructors = {
        "FMTtask": Parallel.FMTtask,
        "FMTreplanningtask": Parallel.FMTreplanningtask,
        "FMTplanningtask": Parallel.FMTplanningtask,
        "FMTtaskhandler": Parallel.FMTtaskhandler,
    }
    for name, constructor in constructors.items():
        assert constructor() is not None
