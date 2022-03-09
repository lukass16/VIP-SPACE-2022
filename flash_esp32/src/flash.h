#include "FS.h"
#include <LITTLEFS.h>

#define FORMAT_LITTLEFS_IF_FAILED true

void initializeFlash(){
     if(!LITTLEFS.begin(FORMAT_LITTLEFS_IF_FAILED)){
        Serial.println("LITTLEFS Mount Failed. Its possible you need to format the partition with LITTLEFS.format() just once");
        //LITTLEFS.format();
        return;
    }
    Serial.println( "SPIFFS-like write file to new path and delete it w/folders" );
}

void printSpaceLeft(){
    unsigned int totalBytes = LITTLEFS.totalBytes();
    unsigned int usedBytes = LITTLEFS.usedBytes();

    Serial.print("Total space:      ");
    Serial.print(totalBytes);
    Serial.print(" bytes ");

    Serial.print(" Total space used: ");
    Serial.print(usedBytes);
    Serial.print(" bytes, ");

    float percentage = float(usedBytes) / float(totalBytes);
    Serial.print(percentage);
    Serial.print(" %");
    Serial.println();
}

void deleteFile(fs::FS &fs, const char * path){
    Serial.printf("Deleting file: %s\r\n", path);
    if(fs.remove(path)){
        Serial.println("- file deleted");
    } else {
        Serial.println("- delete failed");
    }
}


template <unsigned long size>
struct Buffer {
    int offset = 0;
    uint8_t buf[size];

    void push(uint8_t* value){
        for(int i=0; i<sizeof(value); i++)
            buf[i + offset] = value[i];
        offset += sizeof(value);
        // Serial.print("buffer offset: ");
        // Serial.println(offset);
    };

    void clean(){
        offset = 0;

        for (int x = 0; x < size; x++)
            buf[x] = 0;
    }
};





void testFileIO(File file, int multiplier){
    float x_float = 3.1415967987 + multiplier;
    float y_float = 1.1212332 + multiplier;
    float z_float = 99.9999 + multiplier;

    auto x = (uint8_t *)(&x_float);
    auto y = (uint8_t *)(&y_float);
    auto z = (uint8_t *)(&z_float);

    auto const buf_size = sizeof(x) + sizeof(y) + sizeof(z);
    Buffer<buf_size> buffer;

    buffer.push(x);
    buffer.push(y);
    buffer.push(z);

    if(!file){
        Serial.println("- failed to open file for writing");
        return;
    }

    for(int i=0; i < 1; i++){
        // buffer.clean();


        //uint32_t start = millis();
        file.write(buffer.buf, buf_size);
        //uint32_t end = millis() - start;
        // printSpaceLeft();
    }

}

template <unsigned long size>
struct StreamPipe {
    char buf_out[size];
    int offset = 0;

    template <typename T>
    void getValue(T* value_p){
        auto& value = *value_p;
        char char_buf[sizeof(T)];
        for(int i=0; i<sizeof(T); i++)
            char_buf[i] = buf_out[i + offset];
        offset += sizeof(T);

        value = *(T*)char_buf; // Cast char array to type
    }
};


void readFlash(fs::FS &fs, const char * path){
    // Reading
    File file = fs.open(path);
   
    auto const buf_size = sizeof(float) + sizeof(float) + sizeof(float);
   

    while(file.available()){;
        
        StreamPipe<buf_size> stream;
        file.readBytes(stream.buf_out, buf_size);
        // Serial.print("Position: ");
        // Serial.println(file.position());

        float x = 0;
        stream.getValue<float>(&x);
        Serial.println(x, 10);

        float y = 0;
        stream.getValue<float>(&y);
        // Serial.println(y, 10);

        float z = 0;
        stream.getValue<float>(&y);
        // Serial.println(y, 10);


        // Serial.println(y, 10);
    }
    file.close();
}