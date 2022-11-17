/************************************************************************
// support of I2c LCD & OLED display screen for Bela plateforme
// fonctionalitys :
//  - displaying parameters values
//  - sheet managment
//  - UI interfaces with 3 or 5 keys
//  - parameter edition in KEY5 mode

CFLAGS=-I$(PROJECT_DIR)/minidisplay_ui/u8g2/csrc;
CPPFLAGS=-I$(PROJECT_DIR)/minidisplay_ui/u8g2/csrc;
************************************************************************/

#ifndef __MiniDisplay_UI_H__
#define __MiniDisplay_UI_H__

#include <cstddef>
#include <string>
#include <cstdlib>
#include "u8g2/U8g2LinuxI2C.h"
#include <Bela.h>
#include <vector>

using namespace std;

#define DISPLAY_WIDTH 16
#define DISPLAY_HEIGHT 5
#define PARAMETER_SIZE 10  


// font description in the fontbank
struct MD_Font                  
{
    unsigned int Width;
    unsigned int Height;
    const uint8_t * Font;
    
};

// type of parameters for formatting
enum MDP_ParamDisplayType
{
        FLOAT = 0,
        INT,
        BOOL,
        STRING
};

// structure for updating parameters values
struct MD_BridgeValue
{
    unsigned int sPageIdx;
    unsigned int sParamIdx;
    double value;
};

class MDP_Parameter
{
    protected:
        string fLabel;
        double fValue;
        double* fRefValue;
        MDP_ParamDisplayType fDisplayType;
        bool fEditable;
        double fMinValue;
        double fMaxValue;
        double fStep;
        bool fChanged;
        
        void SyncFromRefValue();
        void SyncToRefValue();
    
    public:
        MDP_Parameter(string label,double value, MDP_ParamDisplayType displaytype, bool editable, double* refvalue = NULL,
                      double minval = 0, double maxval = 0, double step = 0);
        virtual ~MDP_Parameter();
        
        string GetLabel();
        double GetValue();
        MDP_ParamDisplayType GetDisplayType();
        bool IsReadOnly();
        
        bool SetValue(double value);
        
        void UpdateValue(int step);
        
        bool IsChanged(bool reset = true);
        
        virtual string GetParameterDisplay(unsigned int width);
        virtual void Update(U8G2* u8g2, int x, int y, int width, int carwidth, int carhight, bool selected, bool editmode);    
};


class MD_Page
{
    protected:
            
        string fName;
        vector<MDP_Parameter*> fParameters;
        
        int fCurrentParam;
        bool fChanged;
        unsigned int fWidthWindow;
        unsigned int fHeightWindow;
        unsigned int fCarWidth;
        unsigned int fCarHeight;
        int fCurrentScroll;
        
    public:
    
        MD_Page(const char* name=NULL);
        virtual ~MD_Page();
        
        void SetWindow(unsigned int width, unsigned int height, unsigned int carwidth, unsigned int carheight);
        int AddParameter(const string& Label, double value, MDP_ParamDisplayType displaytype, bool readonly = true, double* refvalue = NULL,
                         double minval = 0, double maxval = 0, double step = 0);
        
        virtual int Previous();
        virtual int Next();
        int GetCurrentSelection();
        int GetSelectionInWindow();
        
        string GetTitle();
                      
        bool SetParameterValue(const string& Label, double value);
        bool SetParameterValue(unsigned int index, double value);
        
        bool CanEditParameter();
        void UpdateParameterValue(int step);
        
        double GetParameterValue(const string& Label);
        double GetParameterValue(unsigned int index);
        
        string GetParameterDisplay(const string& Label);
        string GetParameterDisplay(unsigned int index);
        
        virtual void Update(U8G2* u8g2, bool editmode = false);
        
        void SetChanged(bool changed);
        bool IsChanged();                
};

/************************************************************************
// MiniDisplay_UI : this classe manage the display screen communication, 
// the data updating and the UI
************************************************************************/

bool PinEvent(bool newstate, bool& currentstate);

class MiniDisplay_UI
{
    protected:
        enum MD_UIMode
        {
                KEY3 = 0,
                KEY5     
        };
        enum MD_UIState
        {
                NOT_SET = -1,
                PAGE_NAV,
                PARAMETER_NAV,
                PARAMETER_EDIT
        };
        vector<MD_Font> gMD_FontBank = {
            {4, 6, u8g2_font_4x6_tf},
            {5, 7, u8g2_font_5x7_tf},
            {5, 8, u8g2_font_5x8_tf},
            {6, 10, u8g2_font_6x10_tf},
            {6, 12, u8g2_font_6x12_tf},
            {6, 13, u8g2_font_6x13_tf},
            {7, 13, u8g2_font_7x13_tf},
            {7, 14, u8g2_font_7x14_tf}, 
            {8, 13, u8g2_font_8x13_tf},
            {9, 15, u8g2_font_9x15_tf}	
        };
                
        U8G2* u8g2;                     // display screen library object
        unsigned int fDisplayWidth;     // width of the screen
        unsigned int fDisplayHeight;    // Height of the screen
        unsigned int fCarWidth;
        unsigned int fCarHeight;
        uint8_t fUpPin;                 //UI pins
        uint8_t fDownPin;
        uint8_t fLeftPin;
        uint8_t fRightPin;
        uint8_t fSelectPin;
        bool fUpPinState;
        bool fDownPinState;
        bool fLeftPinState;
        bool fRightPinState;
        bool fSelectPinState;
        vector<MD_Page*> fPages;        // Page contener
        
        int fCurrentPage;      // idx of the Displaying page
        MD_UIMode fUIMode;              // mode of ui
        MD_UIState fState;              
        bool fChanged;
        
        void EventUpDownKey(int step);
        void EventLeftRightKey(int step);
        
    public:	
        MiniDisplay_UI();
        ~MiniDisplay_UI();
        bool InitDisplay(const char* displaytype, const uint8_t i2cbus, const uint8_t i2cadress,    // initialize the display screen object
                         unsigned int displaywidth = 16, unsigned int displayhight = 8);
        bool InitUi(const uint8_t uppin, const uint8_t downpin, const uint8_t selectpin,            // initialize the UI with the bela digital input pin
                    const uint8_t leftpin=-1, const uint8_t rightpin=-1);
                    
        int AddPage(MD_Page* newpage);
        MD_Page* GetPage(const string& title);
        MD_Page* GetPage(unsigned int index);
        
        void UpdateUI(BelaContext* context);                                                        // check the pins states and uptate the displaying informations
        void UpdateValues(vector<MD_BridgeValue>& values);                                          // update the values of the parameters
        bool UpdateScreen(bool force = false);                                                      // update the display screen if informations have changed
                
                        
};




#endif

