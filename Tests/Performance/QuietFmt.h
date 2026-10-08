/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef PERFORMANCE_QUIETFMT_H_INCLUDED
#define PERFORMANCE_QUIETFMT_H_INCLUDED

namespace Performance
{
	// Makes FMT quiet for the whole process, once: its logger writes nothing, and the errors that
	// doplanning turns into warnings become warnings, so that a production model reads and plans as
	// it does in production. Every benchmark calls it in prepare, before it builds a model.
	//
	// Once only, because FMT keeps one logger for the process, and the solver of a model keeps the
	// message handler of the logger it was built with: replacing the logger while a model exists
	// leaves that model with a destroyed handler, and copying it then fails or crashes.
	void quietFmt();
}

#endif
