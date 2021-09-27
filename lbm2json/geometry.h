/**
 * Library for handing simple geometric shapes.
 * Copyright 2001-2002 Joseph Huckaby.  All rights reserved.
 **/

class Rect {
public:
	short left, top, right, bottom;
	
	Rect();
	Rect(short width, short height);
	Rect(short newLeft, short newTop, short newRight, short newBottom);
	Rect(Rect *source);
	Rect(Rect *source, short newLeft, short newTop);
	
	short width() {return (this->right - this->left);}
	short height() {return (this->bottom - this->top);}
	char empty() {return (this->left >= this->right || this->top >= this->bottom);}
	
	void constrain(Rect *bounds);
	void constrain(short leftPin, short topPin, short rightPin, short bottomPin);
	void offset(short horizDelta, short vertDelta);
	char ptIn(short xPoint, short yPoint);
	void zeroAlign();
	void moveTo(short newLeft, short newTop);
	void inset(short xAmount, short yAmount);
	void set(short newLeft, short newTop, short newRight, short newBottom);
	void set(Rect *source);
	void setUnion(Rect *source);
	void fit(Rect *bounds);
	void intersect(Rect *sectRect);
};
