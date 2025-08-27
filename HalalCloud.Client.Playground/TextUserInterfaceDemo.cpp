#pragma warning(disable: 4244)
#pragma warning(disable: 4589)

#define Uses_TView
#define Uses_TRect
#define Uses_TStatusLine
#define Uses_TStatusDef
#define Uses_TStatusItem
#define Uses_TKeys
#define Uses_MsgBox
#define Uses_fpstream
#define Uses_TEvent
#define Uses_TDeskTop
#define Uses_TApplication
#define Uses_TWindow
#define Uses_TDeskTop
#define Uses_TScreen

#include <tvision/tv.h>

#if !defined( __BORLANDC__ ) && defined( _WIN32 )
#include <tvision/compat/windows/windows.h>
#endif

#include <cstdio>

class TClockView;

class HccApplication : public TApplication
{
private:

public:
    HccApplication() : TProgInit(
        &HccApplication::initStatusLine,
        &HccApplication::initMenuBar,
        &HccApplication::initDeskTop)
    {

    }

    ~HccApplication()
    {

    }
};

void TextUserInterfaceMain()
{
    HccApplication App;
    App.run();
    App.shutDown();

    std::getchar();
}
