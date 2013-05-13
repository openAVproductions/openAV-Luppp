
#ifndef LUPPP_BUFFERS_H
#define LUPPP_BUFFERS_H

class Buffers
{
  public:
    Buffers()
    {
      memset( audio, 0, sizeof(float*)*2);
    }
    float* audio[2];
    
    enum BUFFER {
      MASTER_OUTPUT = 0,
      MASTER_INPUT,
    };
};

#endif // LUPPP_BUFFERS_H

