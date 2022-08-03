// UpButton.cpp : implementation file
//

#include "pch.h"
#include "LazyParkingWindowsCPPClientUI.h"
#include "ArrowButton.h"


// UpButton

IMPLEMENT_DYNAMIC(ArrowButton, CButton)

ArrowButton::ArrowButton(bool i_IsUpArrow): r_IsUpArrow(i_IsUpArrow)
{

}

ArrowButton::~ArrowButton()
{
}


BEGIN_MESSAGE_MAP(ArrowButton, CButton)
END_MESSAGE_MAP()



// UpButton message handlers




void ArrowButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    CDC dc;
    CRect rt(lpDrawItemStruct->rcItem); //Get button rect
    UINT state = lpDrawItemStruct->itemState; //Get state of the button
    CPoint vertices[3];
    CBrush brushRed(RGB(255, 0, 0));
    CBrush brushGray(RGB(204, 204, 204));
    CBrush* pOldBrush;

    vertices[0].x = rt.left;
    vertices[0].y = r_IsUpArrow ? rt.bottom : rt.top;
    vertices[1].x = rt.left + (rt.right - rt.left) / 2;
    vertices[1].y = r_IsUpArrow ? rt.top : rt.bottom;
    vertices[2].x = rt.right;
    vertices[2].y = r_IsUpArrow ? rt.bottom : rt.top;;
    dc.Attach(lpDrawItemStruct->hDC);     //Get device context object
    pOldBrush = dc.SelectObject((state & ODS_DISABLED) ? &brushGray : &brushRed);
    dc.Polygon(vertices, 3);
    if ((state & ODS_SELECTED))            // If it is pressed
    {
        dc.DrawEdge(rt, EDGE_SUNKEN, BF_RECT);    // Draw a sunken face
    }
    else
    {
        dc.DrawEdge(rt, EDGE_ETCHED, BF_RECT);    // Draw a raised face
    }

    dc.SelectObject(pOldBrush);
    dc.Detach();
}
