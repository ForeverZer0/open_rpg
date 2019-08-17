module OpenRPG

  module Audio



    class Stream
      # rb_define_method(rb_cStream, "volume", rpg_stream_volume, 0);
      # rb_define_method(rb_cStream, "pitch", rpg_stream_pitch, 0);
      # rb_define_method(rb_cStream, "playing?", rpg_stream_playing_p, 0);
      # rb_define_method(rb_cStream, "paused?", rpg_stream_paused_p, 0);
      # rb_define_method(rb_cStream, "looping?", rpg_stream_looping_p, 0);
      # rb_define_method(rb_cStream, "pause", rpg_stream_pause, 0);
      # rb_define_method(rb_cStream, "stop", rpg_stream_stop, 0);
      # rb_define_method(rb_cStream, "play", rpg_stream_play, 0);
      # rb_define_method(rb_cStream, "sound", rpg_stream_sound, 0);
      # rb_define_method(rb_cStream, "rewind", rpg_stream_rewind, 0);

    end

    class Sound

      module Type
        # Microsoft WAV format (little endian default). 
        WAV = 0x010000
  
        ##
        # Apple/SGI AIFF format (big endian). 
        AIFF = 0x020000
          
        ##
        # Sun/NeXT AU format (big endian). 
        AU = 0x030000
          
        ##
        # RAW PCM data. 
        RAW = 0x040000
          
        ##
        # Ensoniq PARIS file format. 
        PAF = 0x050000
          
        ##
        # Amiga IFF / SVX8 / SV16 format. 
        SVX = 0x060000
          
        ##
        # Sphere NIST format. 
        NIST = 0x070000
          
        ##
        # VOC files. 
        VOC = 0x080000
          
        ##
        # Berkeley/IRCAM/CARL 
        IRCAM = 0x0A0000
          
        ##
        # Sonic Foundry's 64 bit RIFF/WAV 
        W64 = 0x0B0000
          
        ##
        # Matlab (tm) V4.2 / GNU Octave 2.0 
        MAT4 = 0x0C0000
          
        ##
        # Matlab (tm) V5.0 / GNU Octave 2.1 
        MAT5 = 0x0D0000
          
        ##
        # Portable Voice Format 
        PVF = 0x0E0000
          
        ##
        # Fasttracker 2 Extended Instrument 
        XI = 0x0F0000
          
        ##
        # HMM Tool Kit format 
        HTK = 0x100000
          
        ##
        # Midi Sample Dump Standard 
        SDS = 0x110000
          
        ##
        # Audio Visual Research 
        AVR = 0x120000
          
        ##
        # MS WAVE with WAVEFORMATEX 
        WAVEX = 0x130000
          
        ##
        # Sound Designer 2 
        SD2 = 0x160000
          
        ##
        # FLAC lossless file format 
        FLAC = 0x170000
          
        ##
        # Core Audio File format 
        CAF = 0x180000
          
        ##
        # Psion WVE format 
        WVE = 0x190000
          
        ##
        # Xiph OGG container 
        OGG = 0x200000
          
        ##
        # Akai MPC 2000 sampler 
        MPC2K = 0x210000
          
        ##
        # RF64 WAV file 
        RF64 = 0x220000  
      end
    
      module SubType
        # Signed 8 bit data 
        PCM_S8 = 0x0001
          
        ##
        # Signed 16 bit data 
        PCM_16 = 0x0002
          
        ##
        # Signed 24 bit data 
        PCM_24 = 0x0003
          
        ##
        # Signed 32 bit data 
        PCM_32 = 0x0004
          
        ##
        # Unsigned 8 bit data (WAV and RAW only) 
        PCM_U8 = 0x0005
          
        ##
        # 32 bit float data 
        FLOAT = 0x0006
          
        ##
        # 64 bit float data 
        DOUBLE = 0x0007
          
        ##
        # U-Law encoded. 
        ULAW = 0x0010
          
        ##
        # A-Law encoded. 
        ALAW = 0x0011
          
        ##
        # IMA ADPCM. 
        IMA_ADPCM = 0x0012
          
        ##
        # Microsoft ADPCM. 
        MS_ADPCM = 0x0013
          
        ##
        # GSM 6.10 encoding. 
        GSM610 = 0x0020
          
        ##
        # OKI / Dialogix ADPCM 
        VOX_ADPCM = 0x0021
          
        ##
        # 32kbs G721 ADPCM encoding. 
        G721_32 = 0x0030
          
        ##
        # 24kbs G723 ADPCM encoding. 
        G723_24 = 0x0031
          
        ##
        # 40kbs G723 ADPCM encoding. 
        G723_40 = 0x0032
          
        ##
        # 12 bit Delta Width Variable Word encoding. 
        DWVW_12 = 0x0040
          
        ##
        # 16 bit Delta Width Variable Word encoding. 
        DWVW_16 = 0x0041
          
        ##
        # 24 bit Delta Width Variable Word encoding. 
        DWVW_24 = 0x0042
          
        ##
        # N bit Delta Width Variable Word encoding. 
        DWVW_N = 0x0043
          
        ##
        # 8 bit differential PCM (XI only) 
        DPCM_8 = 0x0050
          
        ##
        # 16 bit differential PCM (XI only) 
        DPCM_16 = 0x0051
          
        ##
        # Xiph Vorbis encoding. 
        VORBIS = 0x0060
          
        ##
        # Apple Lossless Audio Codec (16 bit). 
        ALAC_16 = 0x0070
          
        ##
        # Apple Lossless Audio Codec (20 bit). 
        ALAC_20 = 0x0071
          
        ##
        # Apple Lossless Audio Codec (24 bit). 
        ALAC_24 = 0x0072
          
        ##
        # Apple Lossless Audio Codec (32 bit). 
        ALAC_32 = 0x0073
      end
    
      module Endian
        ##
        # Default file endian-ness. 
        FILE = 0x00000000
          
        ##
        # Force little endian-ness. 
        LITTLE = 0x10000000
          
        ##
        # Force big endian-ness. 
        BIG = 0x20000000
          
        ##
        # Force CPU endian-ness. 
        CPU = 0x30000000
      end
    end
  end
end