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

#include "math/matrix3.h"
#include "math/vector3d.h"
#include "common/algorithm.h"
#include "twp/scenegraph.h"
#include "twp/twp.h"
#include "twp/gfx.h"
#include "twp/object.h"
#include "twp/util.h"

namespace Twp {

#define DEFAULT_FPS 10.f

#define NUMOBJECTSBYROW 4
#define MARGIN 8.f
#define MARGINBOTTOM 10.f
#define BACKOFFSET 7.f
#define ARROWWIDTH 56.f
#define ARROWHEIGHT 86.f
#define BACKWIDTH 137.f
#define BACKHEIGHT 75.f

static float _getFps(float fps, float animFps) {
	if (fps != 0.f)
		return fps;
	return (animFps < 1e-3) ? DEFAULT_FPS : animFps;
}

Node::Node(const Common::String &name, Math::Vector2d scale, Color color)
	: _name(name),
	  _color(color),
	  _computedColor(color),
	  _scale(scale) {
}

Node::~Node() {}

void Node::addChild(Node *child) {
	if (child->_parent) {
		child->_pos -= getAbsPos();
		for (auto it = _children.begin(); it != _children.end();) {
			Node *node = *it;
			if (node == child) {
				it = child->_parent->_children.erase(it);
				break;
			}
			it++;
		}
	}
	_children.push_back(child);
	child->_parent = this;
	child->updateColor();
	child->updateAlpha();
}

const Node *Node::getRoot() const {
	const Node *result = this;
	while (result->_parent != NULL) {
		result = result->_parent;
	}
	return result;
}

int Node::find(Node *other) {
	for (int i = 0; i < _children.size(); i++) {
		if (_children[i] == other) {
			return i;
		}
	}
	return -1;
}

void Node::removeChild(Node *node) {
	int i = find(node);
	if (i != -1) {
		_children.remove_at(i);
	}
}

void Node::clear() {
	_children.clear();
}

void Node::remove() {
	if (_parent != NULL)
		_parent->removeChild(this);
}

void Node::setColor(Color c) {
	_color.rgba.r = c.rgba.r;
	_color.rgba.g = c.rgba.g;
	_color.rgba.b = c.rgba.b;
	_computedColor.rgba.r = c.rgba.r;
	_computedColor.rgba.g = c.rgba.g;
	_computedColor.rgba.b = c.rgba.b;
	updateColor();
}

void Node::setAlpha(float alpha) {
	_color.rgba.a = alpha;
	_computedColor.rgba.a = alpha;
	updateAlpha();
}

void Node::updateColor() {
	Color parentColor = !_parent ? Color(1.f, 1.f, 1.f) : _parent->_computedColor;
	updateColor(parentColor);
}

void Node::updateAlpha() {
	float parentOpacity = !_parent ? 1.f : _parent->_computedColor.rgba.a;
	updateAlpha(parentOpacity);
}

void Node::updateColor(Color parentColor) {
	_computedColor.rgba.r = _color.rgba.r * parentColor.rgba.r;
	_computedColor.rgba.g = _color.rgba.g * parentColor.rgba.g;
	_computedColor.rgba.b = _color.rgba.b * parentColor.rgba.b;
	onColorUpdated(_computedColor);
	for (int i = 0; i < _children.size(); i++) {
		Node *child = _children[i];
		child->updateColor(_computedColor);
	}
}

void Node::updateAlpha(float parentAlpha) {
	_computedColor.rgba.a = _color.rgba.a * parentAlpha;
	onColorUpdated(_computedColor);
	for (int i = 0; i < _children.size(); i++) {
		Node *child = _children[i];
		child->updateAlpha(_computedColor.rgba.a);
	}
}

void Node::setAnchor(Math::Vector2d anchor) {
	if (_anchor != anchor) {
		_anchorNorm = anchor / _size;
		_anchor = anchor;
	}
}

void Node::setAnchorNorm(Math::Vector2d anchorNorm) {
	if (_anchorNorm != anchorNorm) {
		_anchorNorm = anchorNorm;
		_anchor = _size * _anchorNorm;
	}
}

void Node::setSize(Math::Vector2d size) {
	if (_size != size) {
		_size = size;
		_anchor = size * _anchorNorm;
	}
}

static int cmpNodes(const Node *x, const Node *y) {
	return y->getZSort() < x->getZSort();
}

void Node::draw(Math::Matrix4 parent) {
	if (_visible) {
		Math::Matrix4 trsf = getTrsf(parent);
		Math::Matrix4 myTrsf(trsf);
		myTrsf.translate(Math::Vector3d(-_anchor.getX(), _anchor.getY(), 0.f));
		Common::Array<Node *> children(_children);
		Common::sort(children.begin(), children.end(), cmpNodes);
		drawCore(myTrsf);
		for (int i = 0; i < children.size(); i++) {
			Node *child = children[i];
			child->draw(trsf);
		}
	}
}

Math::Vector2d Node::getAbsPos() const {
	return !_parent ? _pos : _parent->getAbsPos() + _pos;
}

Math::Matrix4 Node::getTrsf(Math::Matrix4 parentTrsf) {
	return parentTrsf * getLocalTrsf();
}

static void scale(Math::Matrix4 &m, const Math::Vector2d &v) {
	m(0, 0) *= v.getX();
	m(1, 1) *= v.getY();
}

Math::Matrix4 Node::getLocalTrsf() {
	Math::Vector2d p = _pos + _offset;
	Math::Matrix4 m1;
	m1.translate(Math::Vector3d(p.getX(), p.getY(), 0.f));
	Math::Matrix3 mRot;
	mRot.buildAroundZ(Math::Angle(-_rotation + _rotationOffset));
	Math::Matrix4 m2;
	m2.setRotation(mRot);
	scale(m2, _scale);
	Math::Matrix4 m3;
	m3.translate(Math::Vector3d(_renderOffset.getX(), _renderOffset.getY(), 0.f));
	return m1 * m2 * m3;
}

Rectf Node::getRect() const {
	Math::Vector2d size = _size * _scale;
	return Rectf::fromPosAndSize(getAbsPos(), Math::Vector2d(-size.getX(), size.getY()) * _anchorNorm * _size);
}

ParallaxNode::ParallaxNode(const Math::Vector2d &parallax, const Common::String &sheet, const Common::StringArray &frames)
	: Node("parallax"),
	  _parallax(parallax),
	  _sheet(sheet),
	  _frames(frames) {
}

ParallaxNode::~ParallaxNode() {}

Math::Matrix4 ParallaxNode::getTrsf(Math::Matrix4 parentTrsf) {
	Gfx &gfx = g_engine->getGfx();
	Math::Matrix4 trsf = Node::getTrsf(parentTrsf);
	Math::Vector2d camPos = gfx.cameraPos();
	Math::Vector2d p = Math::Vector2d(-camPos.getX() * _parallax.getX(), -camPos.getY() * _parallax.getY());
	trsf.translate(Math::Vector3d(p.getX(), p.getY(), 0.0f));
	return trsf;
}

void ParallaxNode::drawCore(Math::Matrix4 trsf) {
	Gfx &gfx = g_engine->getGfx();
	SpriteSheet *sheet = g_engine->_resManager.spriteSheet(_sheet);
	Texture *texture = g_engine->_resManager.texture(sheet->meta.image);
	Math::Matrix4 t = trsf;
	float x = 0.f;
	for (int i = 0; i < _frames.size(); i++) {
		const SpriteSheetFrame &frame = sheet->frameTable[_frames[i]];
		Math::Matrix4 myTrsf = t;
		myTrsf.translate(Math::Vector3d(x + frame.spriteSourceSize.left, frame.sourceSize.getY() - frame.spriteSourceSize.height() - frame.spriteSourceSize.top, 0.0f));
		gfx.drawSprite(frame.frame, *texture, getColor(), myTrsf);
		t = trsf;
		x += frame.frame.width();
	}
}

Anim::Anim(Object *obj)
	: Node("anim") {
	_obj = obj;
	_zOrder = 1000;
}

void Anim::clearFrames() {
	_frames.clear();
}

void Anim::setAnim(const ObjectAnimation *anim, float fps, bool loop, bool instant) {
	_anim = anim;
	_disabled = false;
	setName(anim->name);
	_sheet = anim->sheet;
	_frames = anim->frames;
	_frameIndex = instant && _frames.size() > 0 ? _frames.size() - 1 : 0;
	_frameDuration = 1.0 / _getFps(fps, anim->fps);
	_loop = loop || anim->loop;
	_instant = instant;

	clear();
	for (int i = 0; i < _anim->layers.size(); i++) {
		const ObjectAnimation &layer = _anim->layers[i];
		Anim *node = new Anim(_obj);
		node->setAnim(&layer, fps, loop, instant);
		addChild(node);
	}
}

void Anim::trigSound() {
	if ((_anim->triggers.size() > 0) && _frameIndex < _anim->triggers.size()) {
		const Common::String &trigger = _anim->triggers[_frameIndex];
		if (trigger.size() > 0) {
			_obj->trig(trigger);
		}
	}
}

void Anim::update(float elapsed) {
	if (_anim)
		setVisible(Twp::find(_obj->_hiddenLayers, _anim->name) == -1);
	if (_instant)
		disable();
	else if (_frames.size() != 0) {
		_elapsed += elapsed;
		if (_elapsed > _frameDuration) {
			_elapsed = 0;
			if (_frameIndex < _frames.size() - 1) {
				_frameIndex++;
				trigSound();
			} else if (_loop) {
				_frameIndex = 0;
				trigSound();
			} else {
				disable();
			}
		}
		if (_anim->offsets.size() > 0) {
			Math::Vector2d off = _frameIndex < _anim->offsets.size() ? _anim->offsets[_frameIndex] : Math::Vector2d();
			if (_obj->getFacing() == FACE_LEFT) {
				off.setX(-off.getX());
			}
			_offset = off;
		}
	} else if (_children.size() != 0) {
		bool disabled = true;
		for (int i = 0; i < _children.size(); i++) {
			Anim *layer = static_cast<Anim *>(_children[i]);
			layer->update(elapsed);
			disabled = disabled && layer->_disabled;
		}
		if (disabled) {
			disable();
		}
	} else {
		disable();
	}
}

void Anim::drawCore(Math::Matrix4 trsf) {
	if (_frameIndex < _frames.size()) {
		const Common::String &frame = _frames[_frameIndex];
		bool flipX = _obj->getFacing() == FACE_LEFT;
		if (_sheet.size() == 0) {
			_sheet = _obj->_sheet;
			if (_sheet.size() == 0 && _obj->_room) {
				_sheet = _obj->_room->_sheet;
			}
		}
		if (_sheet == "raw") {
			Texture *texture = g_engine->_resManager.texture(frame);
			Math::Vector3d pos(-texture->width / 2.f, -texture->height / 2.f, 0.f);
			trsf.translate(pos);
			g_engine->getGfx().drawSprite(Common::Rect(texture->width, texture->height), *texture, getComputedColor(), trsf, flipX);
		} else {
			SpriteSheet *sheet = g_engine->_resManager.spriteSheet(_sheet);
			const SpriteSheetFrame &sf = sheet->frameTable[frame];
			Texture *texture = g_engine->_resManager.texture(sheet->meta.image);
			float x = flipX ? -0.5f * (-1.f + sf.sourceSize.getX()) + sf.frame.width() + sf.spriteSourceSize.left : 0.5f * (-1.f + sf.sourceSize.getX()) - sf.spriteSourceSize.left;
			float y = 0.5f * (sf.sourceSize.getY() + 1.f) - sf.spriteSourceSize.height() - sf.spriteSourceSize.top;
			Math::Vector3d pos(int(-x), int(y), 0.f);
			trsf.translate(pos);
			g_engine->getGfx().drawSprite(sf.frame, *texture, getComputedColor(), trsf, flipX);
		}
	}
}

ActorNode::ActorNode(Object *obj)
	: Node(obj->_key), _object(obj) {
}

int ActorNode::getZSort() const { return getPos().getY(); }

Math::Vector2d ActorNode::getScale() const {
	float y = _object->_room->getScaling(_object->_node->getPos().getY());
	return Math::Vector2d(y, y);
}

TextNode::TextNode() : Node("text") {
}

TextNode::~TextNode() {}

void TextNode::setText(Text text) {
	_text = text;
	setSize(text.getBounds());
}

void TextNode::updateBounds() {
	setSize(_text.getBounds());
}

Rectf TextNode::getRect() const {
	Math::Vector2d size = _size * _scale;
	return Rectf::fromPosAndSize(getAbsPos() + Math::Vector2d(0, -size.getY()) + Math::Vector2d(-size.getX(), size.getY()) * _anchorNorm, size);
}

void TextNode::onColorUpdated(Color color) {
	_text.setColor(color);
}

void TextNode::drawCore(Math::Matrix4 trsf) {
	_text.draw(g_engine->getGfx(), trsf);
}

Scene::Scene() : Node("Scene") {
	_zOrder = -100;
}
Scene::~Scene() {}

InputState::InputState() : Node("InputState") {
}

InputState::~InputState() {}

void InputState::drawCore(Math::Matrix4 trsf) {
	// draw cursor
	SpriteSheet *gameSheet = g_engine->_resManager.spriteSheet("GameSheet");
	Texture *texture = g_engine->_resManager.texture(gameSheet->meta.image);
	//   if prefs(ClassicSentence) and self.hotspot:
	//       cursorName = "hotspot_" & self.cursorName
	const SpriteSheetFrame &sf = gameSheet->frameTable["cursor"];
	Math::Vector3d pos(sf.spriteSourceSize.left - sf.sourceSize.getX() / 2.f, -sf.spriteSourceSize.height() - sf.spriteSourceSize.top + sf.sourceSize.getY() / 2.f, 0.f);
	trsf.translate(pos * 2.f);
	scale(trsf, Math::Vector2d(2.f, 2.f));
	g_engine->getGfx().drawSprite(sf.frame, *texture, getComputedColor(), trsf);
}

InputStateFlag InputState::getState() const {
	int tmp = 0;
	tmp |= (_inputActive ? UI_INPUT_ON : UI_INPUT_OFF);
	tmp |= (_inputVerbsActive ? UI_VERBS_ON : UI_VERBS_OFF);
	tmp |= (_showCursor ? UI_CURSOR_ON : UI_CURSOR_OFF);
	tmp |= (_inputHUD ? UI_HUDOBJECTS_ON : UI_HUDOBJECTS_OFF);
	return (InputStateFlag)tmp;
}

void InputState::setState(InputStateFlag state) {
	if ((UI_INPUT_ON & state) == UI_INPUT_ON)
		_inputActive = true;
	if ((UI_INPUT_OFF & state) == UI_INPUT_OFF)
		_inputActive = false;
	if ((UI_VERBS_ON & state) == UI_VERBS_ON)
		_inputVerbsActive = true;
	if ((UI_VERBS_OFF & state) == UI_VERBS_OFF)
		_inputVerbsActive = false;
	if ((UI_CURSOR_ON & state) == UI_CURSOR_ON) {
		_showCursor = true;
		_visible = true;
	}
	if ((UI_CURSOR_OFF & state) == UI_CURSOR_OFF) {
		_showCursor = false;
		_visible = false;
	}
	if ((UI_HUDOBJECTS_ON & state) == UI_HUDOBJECTS_ON)
		_inputHUD = true;
	if ((UI_HUDOBJECTS_OFF & state) == UI_HUDOBJECTS_OFF)
		_inputHUD = false;
}

OverlayNode::OverlayNode() : Node("overlay") {
	_ovlColor = Color(0.f, 0.f, 0.f, 0.f); // transparent
	_zOrder = INT_MIN;
}

void OverlayNode::drawCore(Math::Matrix4 trsf) {
	Math::Vector2d size = g_engine->getGfx().camera();
	g_engine->getGfx().drawQuad(size, _ovlColor);
}

static bool hasUpArrow(Object *actor) {
	return actor->_inventoryOffset != 0;
}

static bool hasDownArrow(Object *actor) {
	return actor->_inventory.size() > (actor->_inventoryOffset * NUMOBJECTSBYROW + NUMOBJECTS);
}

Inventory::Inventory() : Node("Inventory") {
	for (int i = 0; i < NUMOBJECTS; i++) {
		float x = SCREEN_WIDTH / 2.f + ARROWWIDTH + MARGIN + ((i % NUMOBJECTSBYROW) * (BACKWIDTH + BACKOFFSET));
		float y = MARGINBOTTOM + BACKHEIGHT + BACKOFFSET - ((i / NUMOBJECTSBYROW) * (BACKHEIGHT + BACKOFFSET));
		_itemRects[i] = Common::Rect(x, y, x + BACKWIDTH, y + BACKHEIGHT);
	}
}

void Inventory::drawSprite(SpriteSheetFrame &sf, Texture *texture, Color color, Math::Matrix4 trsf) {
	Math::Vector3d pos(sf.spriteSourceSize.left - sf.sourceSize.getX() / 2.f, -sf.spriteSourceSize.height() - sf.spriteSourceSize.top + sf.sourceSize.getY() / 2.f, 0.f);
	trsf.translate(pos);
	g_engine->getGfx().drawSprite(sf.frame, *texture, color, trsf);
}

void Inventory::drawArrows(Math::Matrix4 trsf) {
	// TODO: bool isRetro = prefs(RetroVerbs);
	bool isRetro = false;
	SpriteSheet *gameSheet = g_engine->_resManager.spriteSheet("GameSheet");
	Texture *texture = g_engine->_resManager.texture(gameSheet->meta.image);
	SpriteSheetFrame *arrowUp = &gameSheet->frameTable[isRetro ? "scroll_up_retro" : "scroll_up"];
	SpriteSheetFrame *arrowDn = &gameSheet->frameTable[isRetro ? "scroll_down_retro" : "scroll_down"];
	float alphaUp = hasUpArrow(_actor) ? 1.f : 0.f;
	float alphaDn = hasDownArrow(_actor) ? 1.f : 0.f;
	Math::Matrix4 tUp(trsf);
	tUp.translate(Math::Vector3d(SCREEN_WIDTH / 2.f + ARROWWIDTH / 2.f + MARGIN, 1.5f * ARROWHEIGHT + BACKOFFSET, 0.f));
	Math::Matrix4 tDn(trsf);
	tDn.translate(Math::Vector3d(SCREEN_WIDTH / 2.f + ARROWWIDTH / 2.f + MARGIN, 0.5f * ARROWHEIGHT, 0.f));

	drawSprite(*arrowUp, texture, Color::withAlpha(_verbNormal, alphaUp), tUp);
	drawSprite(*arrowDn, texture, Color::withAlpha(_verbNormal, alphaDn), tDn);
}

void Inventory::drawBack(Math::Matrix4 trsf) {
	SpriteSheet *gameSheet = g_engine->_resManager.spriteSheet("GameSheet");
	Texture *texture = g_engine->_resManager.texture(gameSheet->meta.image);
	SpriteSheetFrame *back = &gameSheet->frameTable["inventory_background"];

	float startOffsetX = SCREEN_WIDTH / 2.f + ARROWWIDTH + MARGIN + back->sourceSize.getX() / 2.f;
	float offsetX = startOffsetX;
	float offsetY = 3.f * back->sourceSize.getY() / 2.f + MARGINBOTTOM + BACKOFFSET;

	for (int i = 0; i < 4; i++) {
		Math::Matrix4 t(trsf);
		t.translate(Math::Vector3d(offsetX, offsetY, 0.f));
		drawSprite(*back, texture, _backColor, t);
		offsetX += back->sourceSize.getX() + BACKOFFSET;
	}

	offsetX = startOffsetX;
	offsetY = back->sourceSize.getY() / 2.f + MARGINBOTTOM;
	for (int i = 0; i < 4; i++) {
		Math::Matrix4 t(trsf);
		t.translate(Math::Vector3d(offsetX, offsetY, 0.f));
		drawSprite(*back, texture, _backColor, t);
		offsetX += back->sourceSize.getX() + BACKOFFSET;
	}
}

void Inventory::drawItems(Math::Matrix4 trsf) {
	float startOffsetX = SCREEN_WIDTH / 2.f + ARROWWIDTH + MARGIN + BACKWIDTH / 2.f;
	float startOffsetY = MARGINBOTTOM + 1.5f * BACKHEIGHT + BACKOFFSET;
	SpriteSheet *itemsSheet = g_engine->_resManager.spriteSheet("InventoryItems");
	Texture *texture = g_engine->_resManager.texture(itemsSheet->meta.image);
	int count = MIN(NUMOBJECTS, (int)_actor->_inventory.size() - _actor->_inventoryOffset * NUMOBJECTSBYROW);

	for (int i = 0; i < count; i++) {
		Object *obj = _actor->_inventory[_actor->_inventoryOffset * NUMOBJECTSBYROW + i];
		Common::String icon = obj->getIcon();
		if (itemsSheet->frameTable.contains(icon)) {
			SpriteSheetFrame *itemFrame = &itemsSheet->frameTable[icon];
			Math::Vector2d pos(startOffsetX + ((float)(i % NUMOBJECTSBYROW) * (BACKWIDTH + BACKOFFSET)), startOffsetY - ((float)(i / NUMOBJECTSBYROW) * (BACKHEIGHT + BACKOFFSET)));
			Math::Matrix4 t(trsf);
			t.translate(Math::Vector3d(pos.getX(), pos.getY(), 0.f));
			float s = obj->getScale();
			Twp::scale(t, Math::Vector2d(s, s));
			drawSprite(*itemFrame, texture, Color(), t);
		}
	}
}

void Inventory::drawCore(Math::Matrix4 trsf) {
	if (_actor) {
		drawArrows(trsf);
		drawBack(trsf);
		drawItems(trsf);
	}
}

void Inventory::update(float elapsed, Object *actor, Color backColor, Color verbNormal) {
	static Common::Rect gArrowUpRect(SCREEN_WIDTH / 2.f, ARROWHEIGHT + MARGINBOTTOM + BACKOFFSET, SCREEN_WIDTH / 2.f + ARROWWIDTH, ARROWHEIGHT + MARGINBOTTOM + BACKOFFSET + ARROWHEIGHT);
	static Common::Rect gArrowDnRect(SCREEN_WIDTH / 2.f, MARGINBOTTOM, SCREEN_WIDTH / 2.f + ARROWWIDTH, MARGINBOTTOM + ARROWHEIGHT);

	// udate colors
	_actor = actor;
	_backColor = backColor;
	_verbNormal = verbNormal;

	_obj = nullptr;
	if (_actor) {
		Math::Vector2d scrPos = g_engine->_cursor.pos;

		// update mouse click
		bool down = g_engine->_cursor.leftDown;
		if (_down && down) {
			_down = true;
			if (gArrowUpRect.contains(scrPos.getX(), scrPos.getY())) {
				_actor->_inventoryOffset -= 1;
				if (_actor->_inventoryOffset < 0)
					_actor->_inventoryOffset = clamp(_actor->_inventoryOffset, 0, ((int)_actor->_inventory.size() - 5) / 4);
			} else if (gArrowDnRect.contains(scrPos.getX(), scrPos.getY())) {
				_actor->_inventoryOffset++;
				_actor->_inventoryOffset = clamp(_actor->_inventoryOffset, 0, ((int)_actor->_inventory.size() - 5) / 4);
			}
		} else if (!down) {
			_down = false;
		}

		for (int i = 0; i < NUMOBJECTS; i++) {
			const Common::Rect &item = _itemRects[i];
			if (item.contains(scrPos.getX(), scrPos.getY())) {
				int index = _actor->_inventoryOffset * NUMOBJECTSBYROW + i;
				if (index < _actor->_inventory.size())
					_obj = _actor->_inventory[index];
				break;
			}
		}

		for (int i = 0; i < _actor->_inventory.size(); i++) {
			Object *obj = _actor->_inventory[i];
			obj->update(elapsed);
		}
	}
}

SentenceNode::SentenceNode() : Node("Sentence") {
	_zOrder = -100;
}

SentenceNode::~SentenceNode() {
}

void SentenceNode::setText(const Common::String &text) {
	_text = text;
}

void SentenceNode::drawCore(Math::Matrix4 trsf) {
	Text text("sayline", _text);
	float x, y;
	//   if prefs(ClassicSentence):
	//     x = (ScreenWidth - text.bounds.x) / 2f;
	//     y = 208f;
	//   else:
	x = MAX(_pos.getX() - text.getBounds().getX() / 2.f, MARGIN);
	x = MIN(x, SCREEN_WIDTH - text.getBounds().getX() - MARGIN);
	y = _pos.getY() + 2.f * 38.f;
	if (y >= SCREEN_HEIGHT)
		y = _pos.getY() - 38.f;
	Math::Matrix4 t;
	t.translate(Math::Vector3d(x, y, 0.f));
	text.draw(g_engine->getGfx(), t);
}

} // namespace Twp
