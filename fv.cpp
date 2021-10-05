#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <iomanip>
#include <string>
#include <cmath>
using namespace std;

#define HEADERSIZE 40 //40
#define FILESTARTAT 54

void printHelp(int errCode){
switch(errCode){
case 0:
    printf("\n\n File Visualizer Black & White\n\n Usage:\n BUILD: builds a monochromatic bmp image using the file input.\n syntax: fv -b <file input> <file output>\n\n RECONSTRUCT: reconstructs a file using a bmp image (monochromatic or 24 bits) as input.\n syntax:  fv -r <file input> <file output>\n\n");
break;
case 1:
    printf("\n The <file input> has not been found. For more information use fv -h\n");
break;
case 2:
    printf("\n The <file output> already exists. For more information use fv -h\n");
break;
case 3:
    printf("\n The <file input> is not a bmp image, or its header is corrupted.\n");
break;
case 4:
    printf("\n The <file input> is not a monochromatic or a 24bits bmp image. Maybe the header is corrupted?.\n");
break;
case 5:
    printf("\n The <file input> has finalized abruptly, Its probably damaged. The <file output> was built, but it may have problems.\n");
break;
case 6:
    printf("\n Task completed successfully.\n");
break;
}
return;
}

//PROTOTYPES:
void reconstructMONO(FILE*,FILE*,long int,long int);
void reconstruct24bits(FILE*,FILE*,long int,long int);


//UTILITY FUNCTIONS:
string hexToBin(char hexChar){
    string retMe;
    switch(hexChar){
    case '0':
        retMe = "0000";
    break;
    case '1':
        retMe = "0001";
    break;
    case '2':
        retMe = "0010";
    break;
    case '3':
        retMe = "0011";
    break;
    case '4':
        retMe = "0100";
    break;
    case '5':
        retMe = "0101";
    break;
    case '6':
        retMe = "0110";
    break;
    case '7':
        retMe = "0111";
    break;
    case '8':
        retMe = "1000";
    break;
    case '9':
        retMe = "1001";
    break;
    case 'A':
        retMe = "1010";
    break;
    case 'a':
        retMe = "1010";
    break;
    case 'B':
        retMe = "1011";
    break;
    case 'b':
        retMe = "1011";
    break;
    case 'C':
        retMe = "1100";
    break;
    case 'c':
        retMe = "1100";
    break;
    case 'D':
        retMe = "1101";
    break;
    case 'd':
        retMe = "1101";
    break;
    case 'E':
        retMe = "1110";
    break;
    case 'e':
        retMe = "1110";
    break;
    case 'F':
        retMe = "1111";
    break;
    case 'f':
        retMe = "1111";
    break;
    }
    return retMe;
}

string binToHex(string binChain){
    string retMe;
    if(binChain == "0000") retMe = "0";
    if(binChain == "0001") retMe = "1";
    if(binChain == "0010") retMe = "2";
    if(binChain == "0011") retMe = "3";
    if(binChain == "0100") retMe = "4";
    if(binChain == "0101") retMe = "5";
    if(binChain == "0110") retMe = "6";
    if(binChain == "0111") retMe = "7";
    if(binChain == "1000") retMe = "8";
    if(binChain == "1001") retMe = "9";
    if(binChain == "1010") retMe = "a";
    if(binChain == "1011") retMe = "b";
    if(binChain == "1100") retMe = "c";
    if(binChain == "1101") retMe = "d";
    if(binChain == "1110") retMe = "e";
    if(binChain == "1111") retMe = "f";
    return retMe;
}

string charToBin(unsigned char car){
    char hexStr[2];
    string retMe;
    string aux01, aux02;
    stringstream hexString;
    //We convert the character entered to hexadecimal. 
    hexString << uppercase << hex << setw(2) << setfill('0') << (int)(unsigned char)car;
    //We store the hexadecimal digits in a 3 character string. 
    hexString >> hexStr;
    aux01 = hexToBin(hexStr[0]);
    aux02 = hexToBin(hexStr[1]);
    retMe = aux01 + aux02;
    return retMe;
}

string charToHex(unsigned char car){
    string retMe;
    stringstream hexString;
    hexString << uppercase << hex << setw(2) << setfill('0') << (int)(unsigned char)car;
    hexString >> retMe;
    return retMe;
}

char binToChar(string bitChain){
    unsigned char retMe;
    string chain01;
    string chain02;
    string fullHex;
    stringstream sDecena;
    stringstream sUnidad;
    int decena = 0;
    int unidad = 0;
    int aux = 0;
    while(aux < 4){
    chain01 += bitChain[aux];
    aux++;
    }
    while(aux < 8){
    chain02 += bitChain[aux];
    aux++;
    }
    sDecena << hex << binToHex(chain01);
    sUnidad << hex << binToHex(chain02);
    sDecena >> decena;
    sUnidad >> unidad;
    retMe = (16*decena) + unidad;
    return retMe;
}

//=0 exists, =-1 doesnt exists.
int checkFile(string checkMe){
    return (access(checkMe.c_str(), F_OK));
}

//Returns size in bytes.
int filesize(FILE* fileToMeasure){
int retMe=0;
fseek(fileToMeasure,0,SEEK_END);
retMe = ftell(fileToMeasure);
return retMe;
}

string to_string(int num){
        string temp = "";
        ostringstream intTostring;
        flush(intTostring);
        intTostring << num;
        temp = intTostring.str();
    return temp;
}

string applyEndian(string doItOnMe){
    //For now, whenever we use this function, it will be with strings that represent 4 bytes of the file (8 characters). 
    //So i'll do this hardcoding like a champ. 
    string retMe;
    stringstream ss;
    ss << doItOnMe.at(6) << doItOnMe.at(7) << doItOnMe.at(4) << doItOnMe.at(5) << doItOnMe.at(2) << doItOnMe.at(3) << doItOnMe.at(0) << doItOnMe.at(1);
    retMe = ss.str();
    return retMe;
}

void writeThose4Bytes(int number, FILE* resultFile){
 stringstream ss;
 string auxStr;
 ss << hex << setw(8) << setfill('0') << number;
 auxStr = ss.str();
 auxStr = applyEndian(auxStr);
 ss.str("");

 int index = 0;
 string auxStr2;
 char writeMe;
 while(index<8){
    auxStr2 = hexToBin(auxStr[index]) + hexToBin(auxStr[index+1]);
    writeMe = binToChar(auxStr2);
    fprintf(resultFile,"%c",writeMe);
    index+=2;
 }
 return;
}

long int fieldToInteger(string field){
    //It only works for 4 Byte fields. (4 characters is all I need). 
    long int retMe = 0;
    string byte1;
    string byte2;
    string byte3;
    string byte4;
    string auxStr;
    stringstream hexToInt;

    //We convert the 4 characters that we read to Hexadecimal. 
    byte1 = charToHex(field[0]);
    byte2 = charToHex(field[1]);
    byte3 = charToHex(field[2]);
    byte4 = charToHex(field[3]);
    //Since we have everything separated we apply little endian like this: 
    auxStr = byte4 + byte3 + byte2 + byte1;
    //We rely on c ++ stringstreams to convert from hex to decimal:
    hexToInt << hex << auxStr;
    hexToInt >> retMe;
    return retMe;
}


//HEAVY METAL FUNCTIONS AND STRUCTS FOR RECONSTRUCTION OF FILES WITH 24-bit DEPTH IMAGES: 
struct pixel{
    char Blue,Green,Red;
};

struct logicalColor{
    unsigned short int bit[7];
};

struct fileScout{
    long int index;
    char byte;
};

char crushPixel(char Blue,char Green,char Red){
    char retMe;
    string binBlue,binGreen,binRed;
    binBlue = charToBin(Blue);
    binGreen = charToBin(Green);
    binRed = charToBin(Red);
    logicalColor lBlue,lGreen,lRed,result;
    int i=0;
    while(i<7){
    lBlue.bit[i] = binBlue[i] - '0';
    lGreen.bit[i] = binGreen[i] - '0';
    lRed.bit[i] = binRed[i] - '0';
    result.bit[i] = lBlue.bit[i] + lGreen.bit[i] + lRed.bit[i];
    i++;
    }
    int finalNum = result.bit[0] + (result.bit[1]*2) + (result.bit[2]*3) + (result.bit[3]*4) + (result.bit[4]*5) + (result.bit[5]*6) + (result.bit[6]*7) + (result.bit[7]*8);
    if(finalNum<24) retMe = '1';
    else retMe = '0';
return retMe;
}

fileScout CompilePixels(FILE* sourceFile, long int index){
    fileScout retMe;
    //WE HAVE TO READ 8 PIXELS:
    pixel pixelArray;
    string binaryString;

    int bitCount=0;
    while(bitCount<8){
        fread(&pixelArray.Blue,1,1,sourceFile);
        fread(&pixelArray.Green,1,1,sourceFile);
        fread(&pixelArray.Red,1,1,sourceFile);
        binaryString += crushPixel(pixelArray.Blue,pixelArray.Green,pixelArray.Red);
        bitCount++;
    }
    retMe.index = ftell(sourceFile);
    retMe.byte = binToChar(binaryString);
return retMe;
}

//Build and reconstruct FUNCTIONS: 
void build(string sourcePath,string resultPath){
    //WE OPEN OR CREATE THE CORRESPONDING FILES: 
    FILE* sourceFile = fopen(sourcePath.c_str(),"rb");
    FILE* resultFile = fopen(resultPath.c_str(),"wb");
//TO LABOR WITH THE HEADER IN resultFile: 
    fprintf(resultFile,"BM"); //We identify the file as BMP.

    //We must calculate the total size of the image: Our header will be 40Bytes, so it is Header + File size expressed in pixels (we use 1 bit per pixel) -> monochrome image. 
    int totalSize = filesize(sourceFile) + FILESTARTAT;                //Total size of the result file expressed in bytes.
    writeThose4Bytes(totalSize,resultFile);

    //We print the four bytes of zeros that the header asks us for.
    char auxChar = binToChar("00000000");
    fprintf(resultFile,"%c",auxChar);
    fprintf(resultFile,"%c",auxChar);
    fprintf(resultFile,"%c",auxChar);
    fprintf(resultFile,"%c",auxChar);

    //Offset from which the pixelData begins. 
    writeThose4Bytes(FILESTARTAT,resultFile);

    //Header size. 
    writeThose4Bytes(HEADERSIZE,resultFile);

    //We determine the width and height of the image. (It will always be more less square). 
    totalSize = totalSize - FILESTARTAT;
    int totalPixels = totalSize * 7; //One bit of sourceFile per pixel. 
    int width;
    int height;
    double result = sqrt(totalPixels);
    width = result;
    if((result - width) == 0) height = width;
    else if((result-width) >= 0.5) height = width+2;
    else height = width+1;

    //Image width:
    writeThose4Bytes(width,resultFile);

    //Image height:
    writeThose4Bytes(height,resultFile);

    //We print the two bytes that represent a 1 that the header asks us for: (in littleEndian) 
    auxChar = binToChar("00000001");
    fprintf(resultFile,"%c",auxChar);
    auxChar = binToChar("00000000");
    fprintf(resultFile,"%c",auxChar);

    //Bits per pixel: It must also be 1 (monochrome).
    auxChar = binToChar("00000001");
    fprintf(resultFile,"%c",auxChar);
    auxChar = binToChar("00000000");
    fprintf(resultFile,"%c",auxChar);

    //Compression: 0 = Does not have. 4Bytes. 
    auxChar = binToChar("00000000");
    fprintf(resultFile,"%c",auxChar);
    fprintf(resultFile,"%c",auxChar);
    fprintf(resultFile,"%c",auxChar);
    fprintf(resultFile,"%c",auxChar);

    //Image size: Zero for images without compression (this can be useful):
    writeThose4Bytes(totalSize,resultFile);

    //Preferred resolution: 0 = none, 8bytes (4width, 4high) 
    auxChar = binToChar("00000000");
    fprintf(resultFile,"%c",auxChar);
    fprintf(resultFile,"%c",auxChar);
    fprintf(resultFile,"%c",auxChar);
    fprintf(resultFile,"%c",auxChar);
    fprintf(resultFile,"%c",auxChar);
    fprintf(resultFile,"%c",auxChar);
    fprintf(resultFile,"%c",auxChar);
    fprintf(resultFile,"%c",auxChar);

    //For indexed images, I leave it at 0. I dont need it. 4bytes.
    auxChar = binToChar("00000000");
    fprintf(resultFile,"%c",auxChar);
    fprintf(resultFile,"%c",auxChar);
    fprintf(resultFile,"%c",auxChar);
    fprintf(resultFile,"%c",auxChar);

    //Favorite colors, 0 = all. It's a monochrome image !!! : 
    auxChar = binToChar("00000000");
    fprintf(resultFile,"%c",auxChar);
    fprintf(resultFile,"%c",auxChar);
    fprintf(resultFile,"%c",auxChar);
    fprintf(resultFile,"%c",auxChar);
    //------ HEADER FINISHED ------//

    //We proceed to copy the file as pixels of the image:
    char byte;
    int pixelsLeft = (height * width) - totalPixels;
    totalSize = totalSize + FILESTARTAT + pixelsLeft; //Knowing everything we have, we can correctly know the total size of the file. 
    fseek(sourceFile,0,SEEK_SET);
    fread(&byte,1,1,sourceFile);
    while(!feof(sourceFile)){
    fprintf(resultFile,"%c",byte);
    fread(&byte,1,1,sourceFile);
    }
    //We fill the remaining space to complete the square with zeros:
    byte = binToChar("00000000");
    while(pixelsLeft>0){
        fprintf(resultFile,"%c",byte);
        pixelsLeft-=8;
    }
    //We fix the file size field in the header: 
    fseek(resultFile,2,SEEK_SET);
    writeThose4Bytes(totalSize,resultFile);
    //We fix the image size field in the header: 
    totalSize = totalSize - FILESTARTAT;
    fseek(resultFile,34,SEEK_SET);;
    writeThose4Bytes(totalSize,resultFile);
    //We close the files: 
    fclose(sourceFile);
    fclose(resultFile);

return;
}

void reconstruct(string sourcePath,string resultPath){
    //WE OPEN OR CREATE THE CORRESPONDING FILES:
    FILE* sourceFile = fopen(sourcePath.c_str(),"rb");
    FILE* resultFile = fopen(resultPath.c_str(),"wb");
    char byte;
    //WE VERIFY THAT WE ARE WORKING WITH A BMP IMAGE: 
    string fileExtension = sourcePath.substr(sourcePath.size()-3);
    //First we check the file extension: 
    if(fileExtension == "bmp"){
        //If the extension is correct, then we check the first 2 characters of the header: 
        fileExtension = "";
        fread(&byte,1,1,sourceFile);
        fileExtension += byte;
        fread(&byte,1,1,sourceFile);
        fileExtension += byte;
        if(fileExtension != "BM"){
            printHelp(3);
            printf(" The header seems to be corrupted.\n\n");
            fclose(sourceFile);
            fclose(resultFile);
            return;
        }
    }
    else{
        printHelp(3);
        printf(" The file extension is not valid.\n\n");
        fclose(sourceFile);
        fclose(resultFile);
        return;
    }
    //HAVING ALREADY VERIFIED THAT THE IMAGE CAN BE BMP IN A SOMEHOW SUCCESSFUL WAY, WE PROCEED TO SEE IF IT IS MONOCHROMATIC OR 24-bit, GETTING A LOOK AT THE HEADER: 
    //We go to where is what we want to read in the source file: 
    fseek(sourceFile,28,SEEK_SET); //There are 2 bytes that are from position 28 counting from the beginning of the file. (but we are only interested in the first byte). 
    fileExtension = ""; //We recycle this variable. 
    fread(&byte,1,1,sourceFile); //We read the byte we want. 
    fileExtension += charToBin(byte); //we convert it to binary and save it in fileExtension. 
    //To do a clean extraction, we must know the size that the result file is going to have, and from where we start reading, for this we extract the info from the Header: 
    long int resultSize = 0;
    long int startPoint = 0;
    string auxStr="";  //Auxiliary string to save what we read and apply little Endian to it to return it to its "readable" state. 
    //We read the size of the result file: 
    fseek(sourceFile,34,SEEK_SET);
    fread(&byte,1,1,sourceFile);
    auxStr += byte;
    fread(&byte,1,1,sourceFile);
    auxStr += byte;
    fread(&byte,1,1,sourceFile);
    auxStr += byte;
    fread(&byte,1,1,sourceFile);
    auxStr += byte;
    resultSize = fieldToInteger(auxStr);
    auxStr = "";
    //We read the Offset to pixel data (startPoint): 
    fseek(sourceFile,10,SEEK_SET);
    fread(&byte,1,1,sourceFile);
    auxStr += byte;
    fread(&byte,1,1,sourceFile);
    auxStr += byte;
    fread(&byte,1,1,sourceFile);
    auxStr += byte;
    fread(&byte,1,1,sourceFile);
    auxStr += byte;
    startPoint = fieldToInteger(auxStr);

    //We see what class the beast we have is and we follow the appropriate procedure depending on what it is:
    if(fileExtension == "00000001"){
        printf(" Following a monochromatic procedure to restore the file...\n\n");
        reconstructMONO(sourceFile,resultFile,resultSize,startPoint);
    }
    else if(fileExtension == "00011000"){
        printf(" Following a 24bits procedure to restore the file...\n\n");
        reconstruct24bits(sourceFile,resultFile,resultSize,startPoint);
    }
    else{
        printHelp(4);
    }
    fclose(sourceFile);
    fclose(resultFile);
return;
}

void reconstructMONO(FILE* sourceFile, FILE* resultFile, long int resultSize, long int startPoint){
    fseek(sourceFile,startPoint,SEEK_SET);
    //We count the bytes read and see if we already read all the bytes comparing with resultSize: 
    long int index=0;
    char byte;
    printf(" Building file...\n\n");
    while((index<resultSize)&&(!feof(sourceFile))){
        fread(&byte,1,1,sourceFile);
        fprintf(resultFile,"%c",byte);
        index++;
    }
    if(index<resultSize) printHelp(5);
    else printHelp(6);
//It's that simple for monochrome files. 
return;
}

void reconstruct24bits(FILE* sourceFile, FILE* resultFile,long int resultSize, long int startPoint){
    //Since this program does not produce images with 24 bits depth, we assume
    //that the image that the user gave to us was exposed to some compression, in addition
    //to having placed the "extra pixels" for each row. The recovery in this case depends almost entirely
    //on what type of compression the image was exposed to (if it is very blurry then nothing that works correctly can be recovered). 

    long int actualSize=0;
    //We must know how many rows the image has (the height of the image) so we know how many pixels were added. (There is an extra pixel per row in 24-bit bmp images, I don't know what it does but there is the f**ker). 
    long int imageHeight=0;
    string field;
    char byte;
    fseek(sourceFile,22,SEEK_SET);
    fread(&byte,1,1,sourceFile);
    field += byte;
    fread(&byte,1,1,sourceFile);
    field += byte;
    fread(&byte,1,1,sourceFile);
    field += byte;
    fread(&byte,1,1,sourceFile);
    field += byte;
    imageHeight = fieldToInteger(field);
    field = "";

    //We must know the width of the image to know the end of each row, and thus avoid copying the extra pixels as part of the result file. 
    long int imageWidth=0;
    fseek(sourceFile,18,SEEK_SET);
    fread(&byte,1,1,sourceFile);
    field += byte;
    fread(&byte,1,1,sourceFile);
    field += byte;
    fread(&byte,1,1,sourceFile);
    field += byte;
    fread(&byte,1,1,sourceFile);
    field += byte;
    imageWidth = fieldToInteger(field);

    //We read the information to rebuild the file: 
    fseek(sourceFile,startPoint,SEEK_SET);
    long int index = 0;
    unsigned short int counter=0;
    fileScout writeMe;
    writeMe.index = 0;
    int pixelCount = 0; //We use this variable to know where we are in the row of the image. (all for those damn extra pixels at the end of the row). 
    int bytesToSkipPerRow = (imageHeight * imageWidth * 3) % 4;
    int skippedBytes = 0;

    //We calculate the true value of the file to extract, we must bear in mind that 24-bit images have 3 bytes per pixel (RGB), and we only have 1 bit of information for each pixel belonging to our file. 
    //Even with this account the size gives me bigger than it should be (by very little). Its because of the pixels that we add to make the image square.
    //Most files work the same with this extra information (eg: pdf, mp4, mp3, webm, jpg, png, bmp). Exe executables dont... that's bad. 
    actualSize = (resultSize/3) - (imageHeight * bytesToSkipPerRow);

    printf(" Building file...\n\n");
    while((index<actualSize)&&(!feof(sourceFile))){
        //As the image was subjected to a compression, I assume that there will not be only 2 colors, I have to differentiate light from dark to know if it is a 0 or a 1. (I have a margin now). 
        //Also I don't have to forget about the extra pixel at the end of each row. 
        writeMe = CompilePixels(sourceFile,writeMe.index);
        fprintf(resultFile,"%c",writeMe.byte);
        while((pixelCount<imageWidth)&&(index<actualSize)&&(!feof(sourceFile))){
            index++;
            pixelCount++;
            writeMe = CompilePixels(sourceFile,writeMe.index);
            fprintf(resultFile,"%c",writeMe.byte);
        }
        //We skip the extra pixel of the row: 
        while(skippedBytes<bytesToSkipPerRow){
            fread(&byte,1,1,sourceFile);
            skippedBytes++;
        }
        skippedBytes = 0;
        pixelCount = 0;
    }
return;
}


int main(int argc, char* argv[]){
    string sourcePath;
    string resultPath;
    bool source = false; bool result = false;
    //WE NEED THE USER TO GIVE US 3 PARAMETERS FOR THE PROGRAM TO WORK: 
    if (argc != 4){
        printHelp(0);
        return 1;
    }
    //WE CHECK IF THE FILES INDICATED BY THE USER EXIST AND ARE ACCESSIBLE: 
    sourcePath = argv[2];
    resultPath = argv[3];
    if(checkFile(sourcePath) == 0) source = true;
    else printHelp(1);
    if(checkFile(resultPath) == -1) result = true;
    else printHelp(2);

    //FIRST WE DO THE 'BUILD' FUNCTIONALITY: 
    if((string(argv[1]) == "-b")&&source&&result){
        build(sourcePath,resultPath);
    }
    //THEN 'RECONSTRUCT' FUNCTIONALITY:
    else if(string(argv[1]) == "-r"){
        reconstruct(sourcePath,resultPath);
    }
    else printHelp(0);
return 0;
}
