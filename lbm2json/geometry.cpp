/**
 * Library for handing simple geometric shapes.
 * Copyright 2001-2002 Joseph Huckaby.  All rights reserved.
 **/

#include "geometry.h"

Rect::Rect() {
	this->left = 0;
	this->top = 0;
	this->right = 0;
	this->bottom = 0;
}

Rect::Rect(short width, short height) {
	this->left = 0;
	this->top = 0;
	this->right = width;
	this->bottom = height;
}

Rect::Rect(short newLeft, short newTop, short newRight, short newBottom) {
	this->left = newLeft;
	this->top = newTop;
	this->right = newRight;
	this->bottom = newBottom;
}

Rect::Rect(Rect *source) {
	this->set(source);
}

Rect::Rect(Rect *source, short newLeft, short newTop) {
	this->left = source->left;
	this->top = source->top;
	this->right = source->right;
	this->bottom = source->bottom;
	this->moveTo(newLeft, newTop);
}

void Rect::constrain(Rect *bounds) {
	if (this->left < bounds->left) this->left = bounds->left;
	if (this->top < bounds->top) this->top = bounds->top;
	if (this->right > bounds->right) this->right = bounds->right;
	if (this->bottom > bounds->bottom) this->bottom = bounds->bottom;
	
	if (this->left > bounds->right) this->left = bounds->right;
	if (this->top > bounds->bottom) this->top = bounds->bottom;
	if (this->right < bounds->left) this->right = bounds->left;
	if (this->bottom < bounds->top) this->bottom = bounds->top;
}

void Rect::constrain(short leftPin, short topPin, short rightPin, short bottomPin) {
	if (this->left < leftPin) this->left = leftPin;
	if (this->top < topPin) this->top = topPin;
	if (this->right > rightPin) this->right = rightPin;
	if (this->bottom > bottomPin) this->bottom = bottomPin;
	
	if (this->left > rightPin) this->left = rightPin;
	if (this->top > bottomPin) this->top = bottomPin;
	if (this->right < leftPin) this->right = leftPin;
	if (this->bottom < topPin) this->bottom = topPin;
}

void Rect::offset(short horizDelta, short vertDelta) {
	this->left += horizDelta;
	this->top += vertDelta;
	this->right += horizDelta;
	this->bottom += vertDelta;
}

char Rect::ptIn(short xPoint, short yPoint) {
	if (xPoint>=this->left && xPoint<this->right && yPoint>=this->top && yPoint<this->bottom) return 1;
	return 0;
}

void Rect::zeroAlign() {
	this->right -= this->left;
	this->bottom -= this->top;
	this->left -= this->left;
	this->top -= this->top;
}

void Rect::moveTo(short newLeft, short newTop) {
	this->offset(newLeft - this->left, newTop - this->top);
}

void Rect::inset(short xAmount, short yAmount) {
	this->left += xAmount;
	this->top += yAmount;
	this->right -= xAmount;
	this->bottom -= yAmount;
}

void Rect::set(short newLeft, short newTop, short newRight, short newBottom) {
	this->left = newLeft;
	this->top = newTop;
	this->right = newRight;
	this->bottom = newBottom;	
}

void Rect::set(Rect *source) {
	this->left = source->left;
	this->top = source->top;
	this->right = source->right;
	this->bottom = source->bottom;
}

void Rect::setUnion(Rect *source) {
	if (source->left < this->left) this->left = source->left;
	if (source->top < this->top) this->top = source->top;
	if (source->right > this->right) this->right = source->right;
	if (source->bottom > this->bottom) this->bottom = source->bottom;
}

void Rect::fit(Rect *bounds) {
	if (this->width() > bounds->width()) this->right = this->left + bounds->width();
	if (this->height() > bounds->height()) this->bottom = this->top + bounds->height();
	
	if (this->left < bounds->left) this->offset(bounds->left - this->left, 0);
	if (this->top < bounds->top) this->offset(0, bounds->top - this->top);
	
	if (this->right > bounds->right) this->offset(bounds->right - this->right, 0);
	if (this->bottom > bounds->bottom) this->offset(bounds->bottom - this->bottom, 0);
}

void Rect::intersect(Rect *sectRect) {
	if (sectRect->left > this->left) this->left = sectRect->left;
	if (sectRect->top > this->top) this->top = sectRect->top;
	if (sectRect->right < this->right) this->right = sectRect->right;
	if (sectRect->bottom < this->bottom) this->bottom = sectRect->bottom;
}

