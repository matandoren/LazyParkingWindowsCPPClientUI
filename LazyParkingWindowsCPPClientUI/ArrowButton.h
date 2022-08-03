#pragma once


// UpButton

class ArrowButton : public CButton
{
	DECLARE_DYNAMIC(ArrowButton)

public:
	ArrowButton(bool i_IsUpArrow);
	virtual ~ArrowButton();

private:
	const bool r_IsUpArrow;

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
};


