/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "ags/shared/script/cc_error.h"
#include "ags/engine/script/runtime_script_value.h"
#include "ags/engine/ac/dynobj/cc_dynamic_object.h"
#include "ags/engine/ac/statobj/static_object.h"
#include "ags/shared/util/memory.h"

//include <string.h> // for memcpy()

namespace AGS3 {

using namespace AGS::Shared;

//
// NOTE to future optimizers: I am using 'this' ptr here to better
// distinguish Runtime Values.
//

// TODO: test again if stack entry really can hold an offset itself

// TODO: use endian-agnostic method to access global vars

uint8_t RuntimeScriptValue::ReadByte() const {
	if (this->Type == kScValStackPtr || this->Type == kScValGlobalVar) {
		if (RValue->Type == kScValData) {
			return *(uint8_t *)(RValue->GetPtrWithOffset() + this->IValue);
		} else {
			return RValue->IValue; // get RValue as int
		}
	} else if (this->Type == kScValStaticObject || this->Type == kScValStaticArray) {
		return this->StcMgr->ReadInt8(this->Ptr, this->IValue);
	} else if (this->Type == kScValDynamicObject) {
		return this->DynMgr->ReadInt8(this->Ptr, this->IValue);
	}
	return *((uint8_t *)this->GetPtrWithOffset());
}

int16_t RuntimeScriptValue::ReadInt16() const {
	if (this->Type == kScValStackPtr) {
		if (RValue->Type == kScValData) {
			return *(int16_t *)(RValue->GetPtrWithOffset() + this->IValue);
		} else {
			return RValue->IValue; // get RValue as int
		}
	} else if (this->Type == kScValGlobalVar) {
		if (RValue->Type == kScValData) {
			return Memory::ReadInt16LE(RValue->GetPtrWithOffset() + this->IValue);
		} else {
			return RValue->IValue; // get RValue as int
		}
	} else if (this->Type == kScValStaticObject || this->Type == kScValStaticArray) {
		return this->StcMgr->ReadInt16(this->Ptr, this->IValue);
	} else if (this->Type == kScValDynamicObject) {
		return this->DynMgr->ReadInt16(this->Ptr, this->IValue);
	}
	return *((int16_t *)this->GetPtrWithOffset());
}

int32_t RuntimeScriptValue::ReadInt32() const {
	if (this->Type == kScValStackPtr) {
		if (RValue->Type == kScValData) {
			return *(int32_t *)(RValue->GetPtrWithOffset() + this->IValue);
		} else {
			return RValue->IValue; // get RValue as int
		}
	} else if (this->Type == kScValGlobalVar) {
		if (RValue->Type == kScValData) {
			return Memory::ReadInt32LE(RValue->GetPtrWithOffset() + this->IValue);
		} else {
			return RValue->IValue; // get RValue as int
		}
	} else if (this->Type == kScValStaticObject || this->Type == kScValStaticArray) {
		return this->StcMgr->ReadInt32(this->Ptr, this->IValue);
	} else if (this->Type == kScValDynamicObject) {
		return this->DynMgr->ReadInt32(this->Ptr, this->IValue);
	}
	return *((int32_t *)this->GetPtrWithOffset());
}

bool RuntimeScriptValue::WriteByte(uint8_t val) {
	if (this->Type == kScValStackPtr || this->Type == kScValGlobalVar) {
		if (RValue->Type == kScValData) {
			*(uint8_t *)(RValue->GetPtrWithOffset() + this->IValue) = val;
		} else {
			RValue->SetUInt8(val); // set RValue as int
		}
	} else if (this->Type == kScValStaticObject || this->Type == kScValStaticArray) {
		this->StcMgr->WriteInt8(this->Ptr, this->IValue, val);
	} else if (this->Type == kScValDynamicObject) {
		this->DynMgr->WriteInt8(this->Ptr, this->IValue, val);
	} else {
		*((uint8_t *)this->GetPtrWithOffset()) = val;
	}
	return true;
}

bool RuntimeScriptValue::WriteInt16(int16_t val) {
	if (this->Type == kScValStackPtr) {
		if (RValue->Type == kScValData) {
			*(int16_t *)(RValue->GetPtrWithOffset() + this->IValue) = val;
		} else {
			RValue->SetInt16(val); // set RValue as int
		}
	} else if (this->Type == kScValGlobalVar) {
		if (RValue->Type == kScValData) {
			Memory::WriteInt16LE(RValue->GetPtrWithOffset() + this->IValue, val);
		} else {
			RValue->SetInt16(val); // set RValue as int
		}
	} else if (this->Type == kScValStaticObject || this->Type == kScValStaticArray) {
		this->StcMgr->WriteInt16(this->Ptr, this->IValue, val);
	} else if (this->Type == kScValDynamicObject) {
		this->DynMgr->WriteInt16(this->Ptr, this->IValue, val);
	} else {
		*((int16_t *)this->GetPtrWithOffset()) = val;
	}
	return true;
}

bool RuntimeScriptValue::WriteInt32(int32_t val) {
	if (this->Type == kScValStackPtr) {
		if (RValue->Type == kScValData) {
			*(int32_t *)(RValue->GetPtrWithOffset() + this->IValue) = val;
		} else {
			RValue->SetInt32(val); // set RValue as int
		}
	} else if (this->Type == kScValGlobalVar) {
		if (RValue->Type == kScValData) {
			Memory::WriteInt32LE(RValue->GetPtrWithOffset() + this->IValue, val);
		} else {
			RValue->SetInt32(val); // set RValue as int
		}
	} else if (this->Type == kScValStaticObject || this->Type == kScValStaticArray) {
		this->StcMgr->WriteInt32(this->Ptr, this->IValue, val);
	} else if (this->Type == kScValDynamicObject) {
		this->DynMgr->WriteInt32(this->Ptr, this->IValue, val);
	} else {
		*((int32_t *)this->GetPtrWithOffset()) = val;
	}
	return true;
}

// Notice, that there are only two valid cases when a pointer may be written:
// when the destination is a stack entry or global variable of free type
// (not kScValData type).
// In any other case, only the numeric value (integer/float) will be written.
bool RuntimeScriptValue::WriteValue(const RuntimeScriptValue &rval) {
	if (this->Type == kScValStackPtr) {
		if (RValue->Type == kScValData) {
			*(int32_t *)(RValue->GetPtrWithOffset() + this->IValue) = rval.IValue;
		} else {
			// NOTE: we cannot just WriteValue here because when an integer
			// is pushed to the stack, script assumes that it is always 4
			// bytes and uses that size when calculating offsets to local
			// variables;
			// Therefore if pushed value is of integer type, we should rather
			// act as WriteInt32 (for int8, int16 and int32).
			if (rval.Type == kScValInteger) {
				RValue->SetInt32(rval.IValue);
			} else {
				*RValue = rval;
			}
		}
	} else if (this->Type == kScValGlobalVar) {
		if (RValue->Type == kScValData) {
			Memory::WriteInt32LE(RValue->GetPtrWithOffset() + this->IValue, rval.IValue);
		} else {
			*RValue = rval;
		}
	} else if (this->Type == kScValStaticObject || this->Type == kScValStaticArray) {
		this->StcMgr->WriteInt32(this->Ptr, this->IValue, rval.IValue);
	} else if (this->Type == kScValDynamicObject) {
		this->DynMgr->WriteInt32(this->Ptr, this->IValue, rval.IValue);
	} else {
		*((int32_t *)this->GetPtrWithOffset()) = rval.IValue;
	}
	return true;
}

RuntimeScriptValue &RuntimeScriptValue::DirectPtr() {
	if (Type == kScValGlobalVar || Type == kScValStackPtr) {
		int ival = IValue;
		*this = *RValue;
		IValue += ival;
	}

	if (Ptr) {
		if (Type == kScValDynamicObject)
			Ptr = const_cast<char *>(DynMgr->GetFieldPtr(Ptr, IValue));
		else if (Type == kScValStaticObject)
			Ptr = const_cast<char *>(StcMgr->GetFieldPtr(Ptr, IValue));
		else
			Ptr += IValue;
		IValue = 0;
	}
	return *this;
}

RuntimeScriptValue &RuntimeScriptValue::DirectPtrObj() {
	if (Type == kScValGlobalVar || Type == kScValStackPtr)
		*this = *RValue;
	return *this;
}

intptr_t RuntimeScriptValue::GetDirectPtr() const {
	const RuntimeScriptValue *temp_val = this;
	int ival = temp_val->IValue;
	if (temp_val->Type == kScValGlobalVar || temp_val->Type == kScValStackPtr) {
		temp_val = temp_val->RValue;
		ival += temp_val->IValue;
	}
	if (temp_val->Type == kScValDynamicObject)
		return (intptr_t)temp_val->DynMgr->GetFieldPtr(temp_val->Ptr, ival);
	else if (temp_val->Type == kScValStaticObject)
		return (intptr_t)temp_val->StcMgr->GetFieldPtr(temp_val->Ptr, ival);
	else
		return (intptr_t)(temp_val->Ptr + ival);
}

} // namespace AGS3
