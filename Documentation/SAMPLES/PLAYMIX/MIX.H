typedef enum boolean {false=0, true=1} boolean;

#define VOCNOERROR        0
#define VOCFILEOPENERROR  1
#define VOCNOTVOCFILE     2

class VOCfile
{
public:
   VOCfile(void);
   int open(char *fname);
   unsigned int GetVersion(void) {return version;};
   unsigned int GetTimeConst(void) {return TimeConst;};
   boolean GetStereo(void) {return stereo;};
   boolean eof(void);
   unsigned char GetSample(void);
   void SetRepeat(boolean Repeat) {repeat=Repeat;};
   void SetPitchShift(boolean ShiftOn) {shifton=ShiftOn;};
   void SetShiftAmount(unsigned int ToUse);
   void ResetVOC(void);
private:
   FILE *f;
   unsigned long SamplesRemaining;
   unsigned int version, idcode, TimeConst;
   boolean stereo;
   unsigned char blocktype, sample, pack;
   boolean repeat;
   boolean endoffile;
   boolean IgnoreBlk1Attr;
   unsigned int DataBlockOffset;
   boolean shifton;
   unsigned int touse, tousecount;
   boolean VOCfile::ProcessBlock(void);
};