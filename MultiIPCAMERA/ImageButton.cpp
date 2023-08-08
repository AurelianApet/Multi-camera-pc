// CImageButton 
//

#include "stdafx.h"
#include "ImageButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CImageButton

CImageButton::CImageButton()
{

  m_bLoaded      = FALSE;
  m_hbmpDisabled = NULL;
  ZeroMemory( &m_bitmap , sizeof( m_bitmap ) );

  m_dwAlign = TEXT_INCLUDE | IMAGE_VCENTER | FOCUS_TEXTONLY;

  m_RectImage.SetRectEmpty();
  m_RectText.SetRectEmpty();
  m_RectTextFocus.SetRectEmpty();

  m_clrText = GetSysColor( COLOR_WINDOWTEXT );

}


CImageButton::~CImageButton()
{
   if( m_bLoaded ) {
 	  DeleteObject( (HGDIOBJ)m_hbmpDisabled );
	  m_hbmpDisabled = NULL;
	  m_bLoaded = FALSE;
   }
}


BEGIN_MESSAGE_MAP(CImageButton, CButton)
	//{{AFX_MSG_MAP(CImageButton)
	ON_WM_SETFOCUS()
	ON_WM_SYSCOLORCHANGE()
	ON_WM_SIZE()
	ON_WM_ENABLE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImageButton message handlers



void CImageButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct ) 
{
   //Eðer Button Deðilse	
   if( lpDrawItemStruct->CtlType != ODT_BUTTON)
      return;

   //Get the Parent Window's Font
   CFont *pFont = GetParent()->GetFont();
   if( !pFont )
      pFont = CFont::FromHandle( ( HFONT )GetStockObject( DEFAULT_GUI_FONT ) );


   CRect  rectButton( lpDrawItemStruct->rcItem );
   CRect  rectItem( rectButton );

   BOOL   bFocus      = lpDrawItemStruct->itemState & ODS_FOCUS;
   BOOL   bDisabled   = ODS_DISABLED & lpDrawItemStruct->itemState;


   CDC *pDC = CDC::FromHandle( lpDrawItemStruct->hDC );
   
   pDC->FillRect( rectButton , &CBrush( GetSysColor( COLOR_BTNFACE ) ) );

   
   if( bFocus )
      rectItem.DeflateRect( 1 , 1 );

   CRect rectFocus( rectItem );

   rectFocus.DeflateRect( 3 , 3 );

   BOOL bPushed = FALSE;
   if( ( lpDrawItemStruct->itemState & ODS_SELECTED ) )
      bPushed = TRUE;
   


   //Drawing Normal 
   if( !bPushed )
      pDC->DrawFrameControl( &rectItem ,DFC_BUTTON , DFCS_BUTTONPUSH);


   if( !bDisabled ) {

		 //Focused
	     if( bFocus )
		    pDC->FrameRect(&rectButton,CBrush::FromHandle((HBRUSH)GetStockObject(BLACK_BRUSH)));

		 //Pushed
		 if( bPushed ) {
			
			pDC->FrameRect(&rectItem,CBrush::FromHandle((HBRUSH)GetStockObject(BLACK_BRUSH))); 
            
		 }
	  

   }
	  


   if( ( lpDrawItemStruct->itemState & ODS_FOCUS ) ) {

	  if( m_dwAlign & FOCUS_TEXTONLY )   //Text FucusRect
	     pDC->DrawFocusRect( &m_RectTextFocus );
	  else
         pDC->DrawFocusRect( &rectFocus );

   }



   if( ( lpDrawItemStruct->itemAction & ODA_FOCUS ) ) {

      if( m_dwAlign & FOCUS_TEXTONLY )   //Text FucusRect
	     pDC->DrawFocusRect( &m_RectTextFocus );
	  else
         pDC->DrawFocusRect( &rectFocus );

   }




   CRect rectImage;
   CRect rectText;



   //Drawing Bitmap
   if( m_bLoaded ) {
	
      if( !bDisabled )
	     m_ImageList.DrawIndirect( pDC , 0 , CPoint( m_RectImage.left + bPushed , m_RectImage.top + bPushed ), CSize( m_bitmap.bmWidth , m_bitmap.bmHeight ), CPoint( 0 , 0 ) , ILD_NORMAL );
      else
  	     pDC->DrawState( CPoint( m_RectImage.left + bPushed , m_RectImage.top + bPushed ) , CSize( m_bitmap.bmWidth , m_bitmap.bmHeight ) , m_hbmpDisabled , DST_BITMAP | DSS_DISABLED );

   }


   //Drawing Text
   CFont *pOldFont = pDC->SelectObject( pFont );
   
   CString strCaption;

   GetWindowText( strCaption );

   pDC->SetBkMode(TRANSPARENT);

   CRect  rectTemp( m_RectText );

   if( bPushed )
	  rectTemp.OffsetRect( 1 , 1 );
   


   pDC->SetBkColor( GetSysColor( COLOR_BTNFACE ) );

   if ( ODS_DISABLED & lpDrawItemStruct->itemState )
   {  

	  rectTemp.OffsetRect( 1 , 1 );
      pDC->SetTextColor( GetSysColor( COLOR_WINDOW ) );
      pDC->DrawText( strCaption , rectTemp , DT_SINGLELINE | DT_CENTER | DT_VCENTER );

	  rectTemp.OffsetRect( -1 , -1 );
      pDC->SetTextColor( GetSysColor( COLOR_GRAYTEXT ) );
      pDC->DrawText( strCaption , rectTemp , DT_SINGLELINE | DT_CENTER | DT_VCENTER );
   }
   else
   {
      pDC->SetTextColor( m_clrText );
      pDC->DrawText( strCaption , rectTemp , DT_SINGLELINE | DT_CENTER | DT_VCENTER );
   }

}



BOOL CImageButton::SetButtonImage( UINT uiImageID , COLORREF clrMask )
{

  //Burada Tekrar bir Çaðýrma var ise Mevcut datalarý temizle
  if( m_bLoaded ) {
     DeleteObject( (HGDIOBJ)m_hbmpDisabled );
     m_hbmpDisabled = NULL;

	 m_ImageList.DeleteImageList();

	 m_bitmapImage.DeleteObject();


     m_bLoaded = FALSE;
  }




  //Loading Bitmap
  if( m_bitmapImage.LoadBitmap( uiImageID ) ) {

     m_bitmapImage.GetObject( sizeof( m_bitmap ) , &m_bitmap );

	 if( m_ImageList.Create( m_bitmap.bmWidth , m_bitmap.bmWidth , ILC_COLORDDB | ILC_MASK ,  1 , 1 ) ) {
	
		m_ImageList.Add( &m_bitmapImage , clrMask );
        m_clrMask = clrMask;
		m_bLoaded = TRUE;

		//Create Disable Bitmap
        HBITMAP  hTempBitmap;
        COLORMAP mapColor;
        mapColor.from = clrMask;
        mapColor.to   = RGB(255,255,255);

        hTempBitmap = (HBITMAP)::CreateMappedBitmap(AfxGetApp()->m_hInstance , uiImageID , IMAGE_BITMAP, &mapColor, 1 );
        m_hbmpDisabled = (HBITMAP)::CopyImage( hTempBitmap , IMAGE_BITMAP , m_bitmap.bmWidth, m_bitmap.bmHeight,  LR_COPYDELETEORG );


		//Calculate Rects
		ReCalculateSettings();
		Invalidate();

        
	 }
	 else {

		m_bitmapImage.DeleteObject();
		return FALSE;

	 }

  }
  else
	 return FALSE;

  return TRUE;
}

void CImageButton::OnSetFocus(CWnd* pOldWnd) 
{
	CButton::OnSetFocus(pOldWnd);
	
	// TODO: Add your message handler code here
	Invalidate(TRUE);
}

BOOL CImageButton::SetAlignStyle( DWORD dwStyle )
{

  m_dwAlign = dwStyle;

  return TRUE;
}

void CImageButton::ReCalculateSettings()
{

  //Calculate Rects
  CRect rectClient;
  GetClientRect( &rectClient );

  m_RectImage.SetRectEmpty();
  m_RectText.SetRectEmpty();
  m_RectTextFocus.SetRectEmpty();


  //Default Left Top
  m_RectImage.top    = 6;
  m_RectImage.bottom = m_RectImage.top + m_bitmap.bmHeight;

  m_RectImage.left  = 6;
  m_RectImage.right = m_bitmap.bmWidth + 6;

  //Image Right 
  if( m_dwAlign & IMAGE_RIGHT ) {
     m_RectImage.left  = ( rectClient.right - m_bitmap.bmWidth ) - 6;
     m_RectImage.right = m_RectImage.left + m_bitmap.bmWidth;
  }

  //Image Center
  if( m_dwAlign & IMAGE_VCENTER ) {

     int iTopIndex = rectClient.Height() - m_RectImage.Height();
     if( iTopIndex > 0 ) {

  	    iTopIndex = iTopIndex / 2;
	    if( iTopIndex < 4 )
	   	   iTopIndex = 4;

        m_RectImage.top = iTopIndex;
  	    m_RectImage.bottom = m_RectImage.top + m_bitmap.bmHeight;

	 }

  }
  else if( m_dwAlign & IMAGE_BOTTOM ) {

     m_RectImage.top = rectClient.Height() - m_bitmap.bmHeight - 6;
     if( m_RectImage.top < 6 )
  	    m_RectImage.top = 6;

     m_RectImage.bottom = m_RectImage.top + m_bitmap.bmHeight;

  }
		

  if( ( m_dwAlign & IMAGE_HCENTER ) ) {

     int iTopIndex = rectClient.Width() - m_RectImage.Width();
     if( iTopIndex > 0 ) {

 	    iTopIndex = iTopIndex / 2;

		if( iTopIndex < 4 )
		   iTopIndex = 4;

        m_RectImage.left   = iTopIndex;
	    m_RectImage.right  = m_RectImage.left + m_bitmap.bmWidth;

	 }

  }

  if( m_dwAlign & TEXT_INCLUDE ) {  //Including Text

     if( m_dwAlign & IMAGE_VCENTER ) {

        m_RectText.top    = 6;
        m_RectText.bottom = rectClient.bottom - 6;

  	    //Calculate Text Focus Rect
		m_RectTextFocus.top = 4;
		m_RectTextFocus.bottom = rectClient.bottom - 4;

	 }
     else {

        if( m_dwAlign & IMAGE_BOTTOM ) {

           m_RectText.top    = 6;
           m_RectText.bottom = m_RectImage.top - 6;

  	       //Calculate Text Focus Rect
		   m_RectTextFocus.top = 4;
		   m_RectTextFocus.bottom = m_RectImage.top - 4;


		}
 	    else {

		   m_RectText.top    = m_RectImage.bottom + 6;
		   m_RectText.bottom = rectClient.bottom - 6;

		   //Calculate Text Focus Rect
		   m_RectTextFocus.top = m_RectImage.bottom + 4;
		   m_RectTextFocus.bottom = rectClient.bottom - 4;


		}

	 }
		   
	 //Image Center
     if( m_dwAlign & IMAGE_VCENTER ) {

        if( m_dwAlign & IMAGE_RIGHT ) {
           m_RectText.left   = 6;
           m_RectText.right  = m_RectImage.left - 6;

  	       //Calculate Text Focus Rect
		   m_RectTextFocus.left  = 4;
		   m_RectTextFocus.right = m_RectImage.left - 4;

		}
   	    else {

	       m_RectText.left   = m_RectImage.right + 6;
	       m_RectText.right  = rectClient.right  - 6;

	       //Calculate Text Focus Rect
	       m_RectTextFocus.left  = m_RectImage.right + 4;
	       m_RectTextFocus.right = rectClient.right  - 4;

		}

	 }
	 else {

	    m_RectText.left   = 6;
	    m_RectText.right  = rectClient.right  - 6;

	    //Calculate Text Focus Rect
	    m_RectTextFocus.left  = 4;
	    m_RectTextFocus.right = rectClient.right  - 4;


	 }

  }

}

void CImageButton::SetTextColor(COLORREF color)
{

  m_clrText = color;

}



void CImageButton::OnSysColorChange() 
{
	CButton::OnSysColorChange();
	Invalidate();	
}

void CImageButton::OnSize(UINT nType, int cx, int cy) 
{
	CButton::OnSize(nType, cx, cy);
	
	ReCalculateSettings();
	Invalidate();
	// TODO: Add your message handler code here
	
}

void CImageButton::OnEnable(BOOL bEnable) 
{
	CButton::OnEnable(bEnable);
	
	// TODO: Add your message handler code here
	
}
