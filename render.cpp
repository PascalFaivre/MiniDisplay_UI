#include <Bela.h>
#include <minidisplay_ui/minidisplay_ui.h>
#include <time.h>
#include <math.h>

double gFrequency = 440.0;
float gPhase;
float gInverseSampleRate;

static MiniDisplay_UI gDisplay;

int DELAYSCREENLOOP_WITHOUT_REFRESH = 40000;
int DELAYSCREENLOOP_WITH_REFRESH = 10000;


static void ScreenLoop(void*) // loop for refresh the screen
{
	//struct timespec start, stop;
    bool result = false;
    while (!Bela_stopRequested())
    {
    	
        result = gDisplay.UpdateScreen(false);
        
        if(result)                                  //the screen has refresh every 50 ms approximately
            usleep(DELAYSCREENLOOP_WITH_REFRESH);
        else 
            usleep(DELAYSCREENLOOP_WITHOUT_REFRESH);
    }
}

// setup() is called once before the audio rendering starts.
// Use it to perform any initialisation and allocation which is dependent
// on the period size or sample rate.
// Return true on success; returning false halts the program.
bool setup(BelaContext *context, void *userData)
{
     unsigned int screenWidth=12;
    unsigned int screenHeight=6;
    unsigned int NbPages = 3;
    unsigned int NbParams = 9;

	gDisplay.InitDisplay("U8G2_SSD1309_128X64_NONAME2_F_HW_I2C_LINUX", 2, 0x3c, screenWidth, screenHeight, true); //init screen with : type, i2cbus number, i2c adress, width and height of display in caracter, displaying at 180°
    gDisplay.InitUi(0, 1, 2, 3, 4); // init the pin of bela for navigation
    char buffer [20];
    MD_Page* newpage = new MD_Page("Synthé");
    
    newpage->AddParameter("Fréquence", 440, MDP_ParamDisplayType::INT, true, &gFrequency, 40, 3000, 5); //editable change the fréquence of the sin wave
    newpage->AddParameter("Amplitude", 0.5, MDP_ParamDisplayType::DEC, false, NULL, 0, 1, 0.1);         //dummy parameter
    newpage->AddParameter("Réverb", 0, MDP_ParamDisplayType::BOOL, false, NULL, 0, 1, 1);               //dummy parameter
    newpage->AddParameter("Low Pass", 0, MDP_ParamDisplayType::BOOL, false, NULL, 0, 1, 1);             //dummy parameter
    gDisplay.AddPage(newpage);
   //add other sample pages :
   for(int p=0; p<NbPages; p++) {
        sprintf(buffer, "PAGE_%d",p);
        MD_Page* newpage = new MD_Page(buffer);
        int type = 0;
        for(int i=0; i<NbParams; i++) {
            sprintf(buffer, "Param_%d_%d",p,i);
            double val =  rand() % 10000 / 10 - 500;
            newpage->AddParameter(buffer, val,  (MDP_ParamDisplayType) type, true, NULL, -500, 500, 0.5);
            type++;
            if(type > 2)
             type = 0;
        }
        gDisplay.AddPage(newpage);
    }
    
    
    gInverseSampleRate = 1.0 / context->audioSampleRate;
	gPhase = 0.0;
	
	Bela_runAuxiliaryTask(ScreenLoop); 
	
    return true;
}


// render() is called regularly at the highest priority by the audio engine.
// Input and output are given from the audio hardware and the other
// ADCs and DACs (if available).
void render(BelaContext *context, void *userData)
{
    
    gDisplay.UpdateUI(context); //check the navigation pin
    
    for(unsigned int n = 0; n < context->audioFrames; n++) {
		float out = 0.8f * sinf(gPhase);
		gPhase += 2.0f * (float)M_PI * gFrequency * gInverseSampleRate;
		if(gPhase > M_PI)
			gPhase -= 2.0f * (float)M_PI;

		for(unsigned int channel = 0; channel < context->audioOutChannels; channel++) {
			audioWrite(context, n, channel, out);
		}
	}
}

// cleanup() is called once at the end, after the audio has stopped.
// Release any resources that were allocated in setup().
void cleanup(BelaContext *context, void *userData)
{

}