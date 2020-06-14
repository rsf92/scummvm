/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

// Heavily inspired by hoc
// Copyright (C) AT&T 1995
// All Rights Reserved
//
// Permission to use, copy, modify, and distribute this software and
// its documentation for any purpose and without fee is hereby
// granted, provided that the above copyright notice appear in all
// copies and that both that the copyright notice and this
// permission notice and warranty disclaimer appear in supporting
// documentation, and that the name of AT&T or any of its entities
// not be used in advertising or publicity pertaining to
// distribution of the software without specific, written prior
// permission.
//
// AT&T DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
// INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.
// IN NO EVENT SHALL AT&T OR ANY OF ITS ENTITIES BE LIABLE FOR ANY
// SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
// IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
// ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
// THIS SOFTWARE.

#include "director/director.h"
#include "director/cast.h"
#include "director/score.h"
#include "director/lingo/lingo.h"
#include "director/lingo/lingo-builtins.h"
#include "director/lingo/lingo-code.h"

#include "director/util.h"

namespace Director {

void Lingo::cleanLocalVars() {
	// Clean up current scope local variables and clean up memory
	debugC(3, kDebugLingoExec, "cleanLocalVars: have %d vars", _localvars->size());

	g_lingo->_localvars->clear();
	delete g_lingo->_localvars;

	g_lingo->_localvars = nullptr;
}

Symbol Lingo::define(Common::String &name, int nargs, ScriptData *code, Common::Array<Common::String> *argNames, Common::Array<Common::String> *varNames, Object *factory) {
	Symbol sym;
	sym.name = new Common::String(name);
	sym.type = HANDLER;
	sym.u.defn = code;
	sym.nargs = nargs;
	sym.maxArgs = nargs;
	sym.argNames = argNames;
	sym.varNames = varNames;
	sym.ctx = _assemblyContext;
	sym.archiveIndex = _assemblyArchive;

	if (debugChannelSet(1, kDebugCompile)) {
		uint pc = 0;
		while (pc < sym.u.defn->size()) {
			uint spc = pc;
			Common::String instr = g_lingo->decodeInstruction(sym.u.defn, pc, &pc);
			debugC(1, kDebugCompile, "[%5d] %s", spc, instr.c_str());
		}
		debugC(1, kDebugCompile, "<end define code>");
	}

	if (factory) {
		if (factory->methods.contains(name)) {
			warning("Redefining method '%s' on factory '%s'", name.c_str(), factory->name->c_str());
		}
		factory->methods[name] = sym;
	} else {
		Symbol existing = getHandler(name);
		if (existing.type != VOID)
			warning("Redefining handler '%s'", name.c_str());

		if (!_eventHandlerTypeIds.contains(name)) {
			_archives[_assemblyArchive].functionHandlers[name] = sym;
		} else {
			_archives[_assemblyArchive].eventHandlers[ENTITY_INDEX(_eventHandlerTypeIds[name], _currentEntityId)] = sym;
		}
	}

	return sym;
}

Symbol Lingo::codeDefine(Common::String &name, int start, int nargs, Object *factory, int end, bool removeCode) {
	debugC(1, kDebugCompile, "codeDefine(\"%s\"(len: %d), %d, %d, \"%s\", %d) entity: %d",
			name.c_str(), _currentAssembly->size() - 1, start, nargs, (factory ? factory->name->c_str() : ""),
			end, _currentEntityId);

	if (end == -1)
		end = _currentAssembly->size();

	ScriptData *code = new ScriptData(&(*_currentAssembly)[start], end - start);
	Common::Array<Common::String> *argNames = new Common::Array<Common::String>;
	for (uint i = 0; i < _argstack.size(); i++) {
		argNames->push_back(Common::String(_argstack[i]->c_str()));
	}
	Common::Array<Common::String> *varNames = new Common::Array<Common::String>;
	for (Common::HashMap<Common::String, VarType, Common::IgnoreCase_Hash, Common::IgnoreCase_EqualTo>::iterator it = _methodVars->begin(); it != _methodVars->end(); ++it) {
		if (it->_value == kVarLocal)
			varNames->push_back(Common::String(it->_key));
	}
	Symbol sym = define(name, nargs, code, argNames, varNames, factory);

	if (debugChannelSet(1, kDebugCompile)) {
		debug("Function vars");
		debugN("  Args: ");
		for (uint i = 0; i < argNames->size(); i++) {
			debugN("%s, ", (*argNames)[i].c_str());
		}
		debugN("\n");
		debugN("  Local vars: ");
		for (uint i = 0; i < varNames->size(); i++) {
			debugN("%s, ", (*varNames)[i].c_str());
		}
		debugN("\n");
	}

	// Now remove all defined code from the _currentAssembly
	if (removeCode)
		for (int i = end - 1; i >= start; i--) {
			_currentAssembly->remove_at(i);
		}

	return sym;
}

int Lingo::codeString(const char *str) {
	int numInsts = calcStringAlignment(str);

	// Where we copy the string over
	int pos = _currentAssembly->size();

	// Allocate needed space in script
	for (int i = 0; i < numInsts; i++)
		_currentAssembly->push_back(0);

	byte *dst = (byte *)&_currentAssembly->front() + pos * sizeof(inst);

	memcpy(dst, str, strlen(str) + 1);

	return _currentAssembly->size();
}

int Lingo::codeFloat(double f) {
	int numInsts = calcCodeAlignment(sizeof(double));

	// Where we copy the string over
	int pos = _currentAssembly->size();

	// Allocate needed space in script
	for (int i = 0; i < numInsts; i++)
		_currentAssembly->push_back(0);

	double *dst = (double *)((byte *)&_currentAssembly->front() + pos * sizeof(inst));

	*dst = f;

	return _currentAssembly->size();
}

int Lingo::codeInt(int val) {
	inst i = 0;
	WRITE_UINT32(&i, val);
	g_lingo->code1(i);

	return _currentAssembly->size();
}

bool Lingo::isInArgStack(Common::String *s) {
	for (uint i = 0; i < _argstack.size(); i++)
		if (_argstack[i]->equalsIgnoreCase(*s))
			return true;

	return false;
}

void Lingo::codeArg(Common::String *s) {
	_argstack.push_back(new Common::String(*s));
}

void Lingo::clearArgStack() {
	for (uint i = 0; i < _argstack.size(); i++)
		delete _argstack[i];

	_argstack.clear();
}

int Lingo::codeSetImmediate(bool state) {
	g_lingo->_immediateMode = state;

	int res = g_lingo->code1(LC::c_setImmediate);
	inst i = 0;
	WRITE_UINT32(&i, state);
	g_lingo->code1(i);

	return res;
}

int Lingo::codeFunc(Common::String *s, int numpar) {
	int ret = g_lingo->code1(LC::c_call);

	g_lingo->codeString(s->c_str());

	inst num = 0;
	WRITE_UINT32(&num, numpar);
	g_lingo->code1(num);

	return ret;
}

void Lingo::codeLabel(int label) {
	_labelstack.push_back(label);
	debugC(4, kDebugCompile, "codeLabel: Added label %d", label);
}

void Lingo::processIf(int toplabel, int endlabel) {
	inst iend;

	debugC(4, kDebugCompile, "processIf(%d, %d)", toplabel, endlabel);

	while (true) {
		if (_labelstack.empty()) {
			warning("Lingo::processIf(): Label stack underflow");
			break;
		}

		int label = _labelstack.back();
		_labelstack.pop_back();

		// This is beginning of our if()
		if (!label)
			break;

		debugC(4, kDebugCompile, "processIf: label at %d", label);

		WRITE_UINT32(&iend, endlabel - label + 1);

		(*_currentAssembly)[label] = iend;	/* end, if cond fails */
	}
}

void Lingo::varCreate(const Common::String &name, bool global, SymbolHash *localvars) {
	if (localvars == nullptr) {
		localvars = _localvars;
	}

	if (localvars && localvars->contains(name)) {
		if (global)
			warning("varCreate: variable %s is local, not global", name.c_str());
		return;
	} else if (_currentMe.type == OBJECT && _currentMe.u.obj->hasVar(name)) {
		if (global)
			warning("varCreate: variable %s is instance or property, not global", name.c_str());
		return;
	} else if (_globalvars.contains(name)) {
		if (!global)
			warning("varCreate: variable %s is global, not local", name.c_str());
		return;
	}

	if (global) {
		_globalvars[name] = Symbol();
		_globalvars[name].name = new Common::String(name);
		_globalvars[name].type = INT;
		_globalvars[name].u.i = 0;
	} else {
		(*localvars)[name] = Symbol();
		(*localvars)[name].name = new Common::String(name);
	}
}

void Lingo::codeFactory(Common::String &name) {
	Object *obj = new Object(name, kFactoryObj);

	_currentFactory = obj;
	if (!_globalvars.contains(name)) {
		_globalvars[name] = Symbol();
		_globalvars[name].name = new Common::String(name);
		_globalvars[name].global = true;
		_globalvars[name].type = OBJECT;
		_globalvars[name].u.obj = obj;
	} else {
		warning("Factory '%s' already defined", name.c_str());
	}
}

}
