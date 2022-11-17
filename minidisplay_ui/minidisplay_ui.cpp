
#include "minidisplay_ui.h"

bool PinEvent(bool newstate, bool& currentstate)
{
    bool event = false;
    if(newstate && !currentstate) 
        event = true;
    currentstate=newstate;
    return event;    
}

void MDP_Parameter::SyncFromRefValue()
{
    if(!fEditable && fValue != *fRefValue) {
        fValue = *fRefValue;
        fChanged = true;
    }
}
void MDP_Parameter::SyncToRefValue()
{
   if(fEditable && fValue != *fRefValue) {
        *fRefValue = fValue;        
    } 
}

MDP_Parameter::MDP_Parameter(string label,double value, MDP_ParamDisplayType displaytype, bool editable, double* refvalue,
                          double minval, double maxval, double step)
{
    fLabel = label;
    fValue = value;
    if(refvalue) {
        fRefValue = refvalue;
        fValue = *fRefValue;
    } else {
        fRefValue = NULL;
        fValue = value;
    }
    fDisplayType = displaytype;
    fEditable = editable;
    fMinValue = minval;
    fMaxValue = maxval;
    fStep = step;
    fChanged = true;
}

MDP_Parameter::~MDP_Parameter()
{
}
            
string MDP_Parameter::GetLabel()
{
    return fLabel;
}

double MDP_Parameter::GetValue()
{
    return fValue;
}
 
MDP_ParamDisplayType MDP_Parameter::GetDisplayType()
{
    return fDisplayType;
}
 
bool MDP_Parameter::IsReadOnly()
{
    return !fEditable;
}
            
bool MDP_Parameter::SetValue(double value)
{
    if(fValue!=value) {
        fValue = value;
        fChanged = true;
        return true;
    }
    return false;
}

void MDP_Parameter::UpdateValue(int step)
{
    if(!fEditable)
        return;
    double tmpval = fValue + step * fStep;
    if(tmpval < fMinValue)
        tmpval = fMinValue;
    else if(tmpval > fMaxValue)
        tmpval = fMaxValue;
    if(tmpval != fValue) {
        fValue = tmpval;
        if(fRefValue)
            *fRefValue = tmpval;
        fChanged = true;
    }
}

bool MDP_Parameter::IsChanged(bool reset)
{
    SyncFromRefValue();
    bool result = fChanged;
    if(reset)
        fChanged=false;
    return result;
}
            
string MDP_Parameter::GetParameterDisplay(unsigned int width)
{
    char buffer[width+1];
    buffer[0]=0;
    string label=fLabel;
    if(label.size() > (width-PARAMETER_SIZE))
        label.resize(width-PARAMETER_SIZE);
    switch(fDisplayType)
    {
        case MDP_ParamDisplayType::FLOAT:
            sprintf(buffer, "%s: %3.3f", label.c_str(), fValue);
            break;
        case MDP_ParamDisplayType::INT:
            sprintf(buffer, "%s: %*d", label.c_str(), (int) fValue);
            break;
        case MDP_ParamDisplayType::BOOL:
            string val;
            if(fValue > 0)
                val="ON";
            else
                val="OFF";
            sprintf(buffer, "%s: %s", label.c_str(), val.c_str());
            break;
    };
    string result=buffer;
    return result;
}

void MDP_Parameter::Update(U8G2* u8g2, int x, int y, int width, int carwidth, int carhight,bool selected, bool editmode)
{
    string line = GetParameterDisplay(width);
    if(editmode)
    	line += "<";
    if(selected) {                                               // draw the current parameter                 
            u8g2->drawBox(0, y, line.size()*carwidth, carhight);
            u8g2->setDrawColor(0);
            u8g2->drawStr(0, y, line.c_str()); 
            u8g2->setDrawColor(1);
        }
        else {
            u8g2->drawStr(0, y, line.c_str());                    
        }
}

MD_Page::MD_Page(const char* name)
{
    if(name)
        fName = name;
    fCurrentParam = 0;
    fChanged = false;
    fWidthWindow = DISPLAY_WIDTH;
    fHeightWindow = DISPLAY_HEIGHT;
    fCarWidth = 6;
    fCarHeight = 10;
    fCurrentScroll = 0;
}

MD_Page::~MD_Page()
{
    for (auto t : fParameters) {
        delete t;
    }
}

void MD_Page::SetWindow(unsigned int width, unsigned int height, unsigned int carwidth, unsigned int carheight)
{
    fWidthWindow = width;
    fHeightWindow = height;
    fCarWidth = carwidth;
    fCarHeight = carheight;
}

int MD_Page::AddParameter(const string& label, double value, MDP_ParamDisplayType displaytype, bool readonly, double* refvalue,
                          double minval, double maxval, double step)
{
    MDP_Parameter* newparam = new MDP_Parameter(label, value, displaytype, readonly, refvalue, minval, maxval, step);
    fParameters.push_back(newparam);
    fChanged = true;
    return fParameters.size()-1;
}           
 
int MD_Page::Previous()
{
    if(fParameters.size()==0)
        return -1;
    fCurrentParam--;
    if(fCurrentParam<0)
        fCurrentParam = fParameters.size()-1;
    if(fCurrentParam < fCurrentScroll)
       fCurrentScroll = fCurrentParam;
    else if ( fCurrentParam > (fCurrentScroll + (fHeightWindow - 2)))
       fCurrentScroll = fCurrentParam - (fHeightWindow - 2);
    fChanged = true;
    return fCurrentParam;
}
     
int MD_Page::Next()
{
    if(fParameters.size()==0)
        return -1;
    fCurrentParam++;
    if(fCurrentParam >= fParameters.size())
        fCurrentParam = 0;
    if(fCurrentParam < fCurrentScroll)
       fCurrentScroll = fCurrentParam;
    else if ( fCurrentParam > (fCurrentScroll + (fHeightWindow - 2)))
       fCurrentScroll = fCurrentParam - (fHeightWindow - 2);
    fChanged = true;
    return fCurrentParam;
}

int MD_Page::GetCurrentSelection()
{
    return fCurrentParam;
}

int MD_Page::GetSelectionInWindow()
{
   return fCurrentParam-fCurrentScroll; 
}

string MD_Page::GetTitle()
{
    return fName;
}

bool MD_Page::SetParameterValue(const string& Label, double value)
{
    return true;
}
     
bool MD_Page::SetParameterValue(unsigned int index, double value)
{
    if(index < fParameters.size()) {
        return fParameters[index]->SetValue(value);
    }
    return false;
}

bool MD_Page::CanEditParameter()
{
    return !fParameters[fCurrentParam]->IsReadOnly();
}

void MD_Page::UpdateParameterValue(int step)
{
    fParameters[fCurrentParam]->UpdateValue(step);
}
                
double MD_Page::GetParameterValue(const string& Label)
{
    return 0;
}

double MD_Page::GetParameterValue(unsigned int index)
{
    if(index < fParameters.size()) {
        return fParameters[index]->GetValue();
    }
    return 0;
}
                
string MD_Page::GetParameterDisplay(const string& Label)
{
    return "";
}

string MD_Page::GetParameterDisplay(unsigned int index)
{
    if(index < fParameters.size()) {
        return fParameters[index]->GetParameterDisplay(DISPLAY_WIDTH);
    }
    return "";
}

void MD_Page::Update(U8G2* u8g2, bool editmode)
{
    
    u8g2->drawRBox(0,0,fWidthWindow*fCarWidth,fCarHeight-1,2);
    u8g2->setDrawColor(0);
    u8g2->drawStr((fWidthWindow-fName.size())*fCarWidth/2, 0, fName.c_str());                                          //draw the title of the page
    u8g2->setDrawColor(1);
    int cntparam=0;
    for( int i=fCurrentScroll; i<fParameters.size(); i++) {                     // get the parameters that are in the draw window
        if(cntparam == 0 && fCurrentScroll > 0) {
            u8g2->drawTriangle(fWidthWindow*fCarWidth-fCarWidth / 2,fCarHeight, fWidthWindow*fCarWidth-fCarWidth, fCarHeight*2-2, fWidthWindow*fCarWidth, fCarHeight*2-2);
        }
        else if(cntparam == (fHeightWindow-2) && i < (fParameters.size() - 1)) {
            u8g2->drawTriangle(fWidthWindow*fCarWidth-fCarWidth / 2,fHeightWindow*fCarHeight-2, fWidthWindow*fCarWidth-fCarWidth, fHeightWindow*fCarHeight-fCarHeight+1, fWidthWindow*fCarWidth, fHeightWindow*fCarHeight-fCarHeight+1);
        }
        int y = cntparam*fCarHeight+fCarHeight;
        bool selected = false;
        bool edit = false;
        if(i == fCurrentParam) {
            selected = true;
            if(editmode)
                edit = true;
        }
        fParameters[i]->Update(u8g2, 0, y, fWidthWindow, fCarWidth, fCarHeight, selected, edit);
        cntparam++;
        if(cntparam > (fHeightWindow-2))
            break;
    }
    // fChanged = false; //cause segmentation fault when not commented ???
}
  
void MD_Page::SetChanged(bool changed)
{
    fChanged=changed;
}    
bool MD_Page::IsChanged()
{
    if(fChanged)
        return true;
    for(int i = fCurrentScroll; i < fParameters.size() && i < (fCurrentScroll + fHeightWindow - 1); i++) {
        if(fParameters[i]->IsChanged())
            return true;
    }
    return false;
}



void MiniDisplay_UI::EventUpDownKey(int step)
{
    if(fPages.size() == 0)
            return;
    if(fState == MD_UIState::PAGE_NAV) {
        fCurrentPage += step;
        if(fCurrentPage < 0)
            fCurrentPage = fPages.size()-1;
        else if(fCurrentPage >= fPages.size())
            fCurrentPage = 0;
        fChanged = true;                
    } else if(fState == MD_UIState::PARAMETER_NAV) {
        MD_Page* curpage = fPages[fCurrentPage];
        if(step < 0)
            curpage->Previous();
        else
            curpage->Next();
    } else if(fState == MD_UIState::PARAMETER_EDIT) {
        MD_Page* curpage = fPages[fCurrentPage];
        curpage->UpdateParameterValue(step);
    }
}

void MiniDisplay_UI::EventLeftRightKey(int step)
{
    if(fState == MD_UIState::PARAMETER_NAV) {
        fCurrentPage += step;
        if(fCurrentPage < 0)
            fCurrentPage = fPages.size()-1;
        else if(fCurrentPage >= fPages.size())
            fCurrentPage = 0;
        fChanged = true;     
    } else if(fState == MD_UIState::PARAMETER_EDIT) {
        //Nothing 
    } 
}

MiniDisplay_UI::MiniDisplay_UI()
{
    u8g2 = NULL;
    fDisplayWidth = DISPLAY_WIDTH;
    fDisplayHeight = DISPLAY_HEIGHT;
    fUpPin = 0;
    fDownPin = 1;
    fLeftPin = 2;
    fRightPin = 3;
    fSelectPin = 4;
    fUpPinState = false;
    fDownPinState = false;
    fLeftPinState = false;
    fRightPinState = false;
    fSelectPinState = false;
    
    fCurrentPage = 0;
    fUIMode = MD_UIMode::KEY5;
    fState =  MD_UIState::NOT_SET;
    fChanged = false;
}

MiniDisplay_UI::~MiniDisplay_UI()
{
    if(u8g2) {
        u8g2->setPowerSave(1);
        delete u8g2;
    }
    for (auto t : fPages) {
        delete t;
    }
}

bool MiniDisplay_UI::InitDisplay(const char* displaytype, const uint8_t i2cbus, const uint8_t i2cadress,
                                 unsigned int displaywidth, unsigned int displayhight)
{
    if(strcasecmp(displaytype, "U8G2_SH1106_128X64_NONAME_F_HW_I2C_LINUX") == 0) {
       u8g2 = new U8G2_SH1106_128X64_NONAME_F_HW_I2C_LINUX(U8G2_R0, i2cbus, i2cadress);
    }
    else if(strcasecmp(displaytype, "U8G2_SSD1306_128X64_NONAME_F_HW_I2C_LINUX") == 0) {
       u8g2 = new U8G2_SSD1306_128X64_NONAME_F_HW_I2C_LINUX(U8G2_R0, i2cbus, i2cadress);
    }
    else if(strcasecmp(displaytype, "U8G2_SSD1309_128X64_NONAME2_F_HW_I2C_LINUX") == 0) {
       u8g2 = new U8G2_SSD1309_128X64_NONAME2_F_HW_I2C_LINUX(U8G2_R0, i2cbus, i2cadress);
    }
    else {
        return false;
    }
    if(!u8g2)
       return false;
    u8g2->initDisplay();
    u8g2->clearDisplay();
    u8g2->setPowerSave(0);
    int cw = u8g2->getDisplayWidth() / displaywidth;
    int ch = u8g2->getDisplayHeight() / displayhight;

    for(int i = gMD_FontBank.size()-1; i >= 0; i--) {
        if( (gMD_FontBank[i].Height <= ch && gMD_FontBank[i].Width <= cw) || i == 0) {
            u8g2->setFont(gMD_FontBank[i].Font);
            fDisplayWidth = u8g2->getDisplayWidth() / gMD_FontBank[i].Width;
            fDisplayHeight = u8g2->getDisplayHeight() / gMD_FontBank[i].Height;
            fCarWidth = gMD_FontBank[i].Width;
            fCarHeight = gMD_FontBank[i].Height;
            break;       
        }
    }    
    //u8g2->setFont(u8g2_font_6x10_tf);
    u8g2->setFontMode(1);
    u8g2->setFontRefHeightText();
    u8g2->setFontPosTop();
    for(int i = 0; i < fPages.size(); i++) {
        fPages[i]->SetWindow(fDisplayWidth,fDisplayHeight,fCarWidth,fCarHeight);
    }
    return true;
}
    
bool MiniDisplay_UI::InitUi(const uint8_t uppin, const uint8_t downpin, const uint8_t selectpin, 
                            const uint8_t leftpin, const uint8_t rightpin)
{
    fUpPin = uppin;
    fDownPin = downpin;
    fLeftPin = leftpin;
    fRightPin = rightpin;
    fSelectPin = selectpin;
    if(leftpin < 0 || rightpin < 0) {
        fUIMode = MD_UIMode::KEY3;
        fState = MD_UIState::PAGE_NAV;
    }
    else {
        fUIMode = MD_UIMode::KEY5;
        fState = MD_UIState::PARAMETER_NAV;
    }
    return true;    
}    
                            
int MiniDisplay_UI::AddPage(MD_Page* newpage)
{
    newpage->SetWindow(fDisplayWidth, fDisplayHeight, fCarWidth, fCarHeight);
    fPages.push_back(newpage);
    return fPages.size() -1;
}

MD_Page* MiniDisplay_UI::GetPage(const string& title)
{
   for(int i=0; i<fPages.size(); i++) {
       if(fPages[i]->GetTitle() == title)
           return fPages[i];
   }       
   return NULL;
}

MD_Page* MiniDisplay_UI::GetPage(unsigned int index)
{
    if(index < fPages.size()) {
        return fPages[index];
    }
    return NULL;
}
                
void MiniDisplay_UI::UpdateUI(BelaContext* context)
{ 
    //get the pin states
    if(PinEvent(digitalRead(context, 0, fUpPin), fUpPinState)) {
        EventUpDownKey(1);
    } else if(PinEvent(digitalRead(context, 0, fDownPin), fDownPinState)) {
        EventUpDownKey(-1);
    } else if(PinEvent(digitalRead(context, 0, fSelectPin), fSelectPinState)) {
        if(fUIMode == MD_UIMode::KEY3) {
            if(fState == MD_UIState::PAGE_NAV)
                fState = MD_UIState::PARAMETER_NAV;
            else 
                fState = MD_UIState::PAGE_NAV;
        } else if(fUIMode == MD_UIMode::KEY5) {
            if(fState == MD_UIState::PARAMETER_EDIT) {
                fState = MD_UIState::PARAMETER_NAV;
            }
            else {
                if(fPages[fCurrentPage]->CanEditParameter())
                    fState = MD_UIState::PARAMETER_EDIT;
            }
        }
    } else if(fLeftPin >= 0 && PinEvent(digitalRead(context, 0, fLeftPin), fLeftPinState)) {
        EventLeftRightKey(-1);
    } else if(fRightPin >= 0 && PinEvent(digitalRead(context, 0, fRightPin), fRightPinState)) {
        EventLeftRightKey(1);
    }
}

void MiniDisplay_UI::UpdateValues(vector<MD_BridgeValue>& values)
{
    for(int i = 0; i < values.size(); i++) {
        MD_Page* curpage = fPages[values[i].sPageIdx];
        if(curpage) {
            curpage->SetParameterValue(values[i].sParamIdx, values[i].value);
        }
    }
}

bool MiniDisplay_UI::UpdateScreen(bool force)
{
    if(fPages.size() == 0 || u8g2 == NULL)
        return false;
    MD_Page* curPage = fPages[fCurrentPage];
    if(curPage->IsChanged() || fChanged || force)
    {
        bool editmode = false;
        if(fState == MD_UIState::PARAMETER_EDIT)
            editmode = true ;
        u8g2->clearBuffer();
        curPage->Update(u8g2, editmode);
        u8g2->sendBuffer();
        fChanged = false;
        return true;
    }
    return false;
    
}