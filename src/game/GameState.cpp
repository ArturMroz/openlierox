/*
 *  GameState.cpp
 *  OpenLieroX
 *
 *  Created by Albert Zeyer on 15.02.12.
 *  code under LGPL
 *
 */

#include "GameState.h"
#include "Game.h"
#include "Settings.h"
#include "CWorm.h"
#include "CBytestream.h"
#include "ClassInfo.h"
#include "ProfileSystem.h"

ScriptVar_t ObjectState::getValue(AttribRef a) const {
	Attribs::const_iterator it = attribs.find(a);
	if(it == attribs.end())
		return a.getAttrDesc()->defaultValue;
	return it->second.value;
}

GameStateUpdates::operator bool() const {
	if(!objs.empty()) return true;
	if(!objCreations.empty()) return true;
	if(!objDeletions.empty()) return true;
	return false;
}

void GameStateUpdates::writeToBs(CBytestream* bs) const {
	bs->writeInt16((uint16_t)objCreations.size());
	foreach(o, objCreations) {
		o->writeToBs(bs);
	}

	bs->writeInt16((uint16_t)objDeletions.size());
	foreach(o, objDeletions) {
		o->writeToBs(bs);
	}

	bs->writeInt((uint32_t)objs.size(), 4);
	foreach(a, objs) {
		a->writeToBs(bs);
		bs->writeVar(a->get());
	}
}

static BaseObject* getObjFromRef(ObjRef r) {
	switch(r.classId) {
	case LuaID<Game>::value: return &game;
	case LuaID<CWorm>::value: return game.wormById(r.objId);
	default: break;
	}
	return NULL;
}

static bool attrBelongsToClass(const ClassInfo* classInfo, const AttrDesc* attrDesc) {
	return classInfo->isTypeOf(attrDesc->objTypeId);
}

void GameStateUpdates::handleFromBs(CBytestream* bs) {
	uint16_t creationsNum = bs->readInt16();
	for(uint16_t i = 0; i < creationsNum; ++i) {
		ObjRef r;
		r.readFromBs(bs);
		//BaseObject* o = getClassInfo(r.classId)->createInstance();
		//o->thisRef.classId = r.classId;
		//o->thisRef.objId = r.objId;

		// we only handle/support CWorm objects for now...
		if(game.isServer()) {
			errors << "GameStateUpdates::handleFromBs: got obj creation as server: " << r.description() << endl;
			bs->SkipAll();
			return;
		}
		if(r.classId != LuaID<CWorm>::value) {
			errors << "GameStateUpdates::handleFromBs: obj-creation: invalid class: " << r.description() << endl;
			bs->SkipAll();
			return;
		}
		if(game.wormById(r.objId, false) != NULL) {
			errors << "GameStateUpdates::handleFromBs: worm-creation: worm " << r.objId << " already exists" << endl;
			bs->SkipAll();
			return;
		}
		game.createNewWorm(r.objId, false, NULL, Version());
	}

	uint16_t deletionsNum = bs->readInt16();
	for(uint16_t i = 0; i < deletionsNum; ++i) {
		ObjRef r;
		r.readFromBs(bs);

		// we only handle/support CWorm objects for now...
		if(game.isServer()) {
			errors << "GameStateUpdates::handleFromBs: got obj deletion as server: " << r.description() << endl;
			bs->SkipAll();
			return;
		}
		if(r.classId != LuaID<CWorm>::value) {
			errors << "GameStateUpdates::handleFromBs: obj-deletion: invalid class: " << r.description() << endl;
			bs->SkipAll();
			return;
		}
		CWorm* w = game.wormById(r.objId, false);
		if(!w) {
			errors << "GameStateUpdates::handleFromBs: obj-deletion: worm " << r.objId << " does not exist" << endl;
			bs->SkipAll();
			return;
		}
		game.removeWorm(w);
	}

	uint32_t attrUpdatesNum = bs->readInt(4);
	for(uint32_t i = 0; i < attrUpdatesNum; ++i) {
		ObjAttrRef r;
		r.readFromBs(bs);

		const AttrDesc* attrDesc = r.attr.getAttrDesc();
		if(attrDesc == NULL) {
			errors << "GameStateUpdates::handleFromBs: AttrDesc for update not found" << endl;
			bs->SkipAll();
			return;
		}

		const ClassInfo* classInfo = getClassInfo(r.obj.classId);
		if(classInfo == NULL) {
			errors << "GameStateUpdates::handleFromBs: class " << r.obj.classId << " for obj-update unknown" << endl;
			bs->SkipAll();
			return;
		}

		if(!attrBelongsToClass(classInfo, attrDesc)) {
			errors << "GameStateUpdates::handleFromBs: attr " << attrDesc->description() << " does not belong to class " << r.obj.classId << " for obj-update" << endl;
			bs->SkipAll();
			return;
		}

		ScriptVar_t v;
		bs->readVar(v);

		// for now, this is somewhat specific to the only types we support
		if(r.obj.classId == LuaID<Settings>::value) {
			if(game.isServer()) {
				errors << "GameStateUpdates::handleFromBs: got settings update as server" << endl;
				bs->SkipAll();
				return;
			}
			if(!Settings::getAttrDescs().belongsToUs(attrDesc)) {
				errors << "GameStateUpdates::handleFromBs: settings update AttrDesc " << attrDesc->description() << " is not a setting attr" << endl;
				bs->SkipAll();
				return;
			}
			FeatureIndex fIndex = Settings::getAttrDescs().getIndex(attrDesc);
			cClient->getGameLobby().overwrite[fIndex] = v;
		}
		else {
			BaseObject* o = getObjFromRef(r.obj);
			if(o == NULL) {
				errors << "GameStateUpdates::handleFromBs: object for attr update not found" << endl;
				bs->SkipAll();
				return;
			}
			if(o->thisRef != r.obj) {
				errors << "GameStateUpdates::handleFromBs: object-ref for attr update invalid" << endl;
				bs->SkipAll();
				return;
			}
			ScriptVarPtr_t p = attrDesc->getValueScriptPtr(o);
			p.fromScriptVar(v);
		}

		//notes << "game state update: <" << r.obj.description() << "> " << attrDesc->attrName << " to " << v.toString() << endl;
	}
}

void GameStateUpdates::pushObjAttrUpdate(ObjAttrRef a) {
	objs.insert(a);
}

void GameStateUpdates::pushObjCreation(ObjRef o) {
	objDeletions.erase(o);
	objCreations.insert(o);
}

void GameStateUpdates::pushObjDeletion(ObjRef o) {
	Objs::iterator itStart = objs.lower_bound(ObjAttrRef::LowerLimit(o));
	Objs::iterator itEnd = objs.upper_bound(ObjAttrRef::UpperLimit(o));
	objs.erase(itStart, itEnd);
	objCreations.erase(o);
	objDeletions.insert(o);
}

void GameStateUpdates::reset() {
	objs.clear();
	objCreations.clear();
	objDeletions.clear();
}

static bool ownObject(ObjRef o) {
	// This is very custom right now and need to be made somewhat more general.
	if(o.classId == LuaID<CWorm>::value) {
		CWorm* w = game.wormById(o.objId, false);
		if(!w) return false;
		return w->getLocal();
	}
	if(game.isServer()) return true;
	return false;
}

void GameStateUpdates::diffFromStateToCurrent(const GameState& s) {
	reset();
	foreach(o, game.gameStateUpdates->objCreations) {
		if(game.isClient()) continue; // none-at-all right now... worm-creation is handled independently atm
		if(!ownObject(*o)) continue;
		if(!s.haveObject(*o))
			pushObjCreation(*o);
	}
	foreach(u, game.gameStateUpdates->objs) {
		if(!ownObject(u->obj)) continue;
		const AttrDesc* attrDesc = u->attr.getAttrDesc();
		if(game.isServer() && !attrDesc->serverside) continue;
		if(game.isClient() && attrDesc->serverside) continue;
		ScriptVar_t curValue = u->get();
		ScriptVar_t stateValue = attrDesc->defaultValue;
		if(s.haveObject(u->obj))
			stateValue = s.getValue(*u);
		//notes << "update " << u->description() << ": " << curValue.toString() << " -> " << stateValue.toString() << endl;
		if(curValue != stateValue)
			pushObjAttrUpdate(*u);
	}
	foreach(o, game.gameStateUpdates->objDeletions) {
		if(game.isClient()) continue; // see obj-creations
		if(!ownObject(*o)) continue;
		if(s.haveObject(*o))
			pushObjDeletion(*o);
	}
}

static ObjectState& GameState_registerObj(GameState& s, BaseObject* obj) {
	return s.objs[obj->thisRef] = ObjectState(obj);
}

GameState::GameState() {
	// register singletons which are always there
	GameState_registerObj(*this, &game);
	GameState_registerObj(*this, &gameSettings);
}

GameState GameState::Current() {
	GameState s;
	s.updateToCurrent();
	return s;
}

void GameState::reset() {
	*this = GameState();
}

void GameState::updateToCurrent() {
	reset();
	foreach(o, game.gameStateUpdates->objCreations) {
		objs[*o] = ObjectState(*o);
	}
	foreach(u, game.gameStateUpdates->objs) {
		ObjAttrRef r = *u;
		Objs::iterator it = objs.find(r.obj);
		assert(it != objs.end());
		ObjectState& s = it->second;
		assert(s.obj == it->first);
		s.attribs[u->attr].value = r.get();
	}
}

void GameState::addObject(ObjRef o) {
	assert(!haveObject(o));
	objs[o] = ObjectState(o);
}

bool GameState::haveObject(ObjRef o) const {
	Objs::const_iterator it = objs.find(o);
	return it != objs.end();
}

ScriptVar_t GameState::getValue(ObjAttrRef a) const {
	Objs::const_iterator it = objs.find(a.obj);
	assert(it != objs.end());
	assert(it->second.obj == a.obj);
	return it->second.getValue(a.attr);
}

