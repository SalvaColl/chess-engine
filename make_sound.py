import wave
import struct
import math

sample_rate = 44100
duration = 0.08  
frequency = 150  

with wave.open('move.wav', 'w') as wav_file:
    wav_file.setnchannels(1)
    wav_file.setsampwidth(2)
    wav_file.setframerate(sample_rate)

    for i in range(int(sample_rate * duration)):
        volume = 32767 * (1.0 - (i / (sample_rate * duration)))
        
        value = int(volume * math.sin(2 * math.pi * frequency * i / sample_rate))
        
        data = struct.pack('<h', value)
        wav_file.writeframesraw(data)

print("Created move.wav successfully! Run your gui.py again.")