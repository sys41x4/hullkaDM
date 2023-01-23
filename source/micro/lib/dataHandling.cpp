#include "dataHandling.h"

char* DATA::replaceStr(char* arrTo, char* arrFrom, const char* startBound, const char* endBound, const char* replacementOf, const char* replaceBy){

    // StartBound and endBound Should Contain char array of size 2
    //if(arrTo==nullptr || )
    //free(&arrTo);
    // int replacementOfLen = strlen(replacementOf);
    // int replaceByLen = strlen(replaceBy);
    int replacementOfLen = 0;
    int replaceByLen = 0;
    int fullArrayLen=0;
    int replacementOccurence=0;

    while(replacementOf[replacementOfLen]!='\0'){replacementOfLen++;}
    while(replaceBy[replaceByLen]!='\0'){replaceByLen++;}

    while(arrFrom[fullArrayLen]!='\0'){
        if(arrFrom[fullArrayLen+replacementOfLen+1]!='\0' &&
            (arrFrom[fullArrayLen]==startBound[0]) && (arrFrom[fullArrayLen+1]==startBound[1]) &&
            (arrFrom[fullArrayLen+replacementOfLen+2]==endBound[0]) && (arrFrom[fullArrayLen+replacementOfLen+1+2]==endBound[1])
        ){replacementOccurence++;};
        fullArrayLen++;
    }
    int newArrayLen = (replacementOccurence*replaceByLen)+fullArrayLen-(replacementOccurence*(replacementOfLen+4));
    arrTo = static_cast<char*>(std::malloc(newArrayLen * sizeof(char)));

    int j=0;
    int replaceCompleteCount = 0;

    for(int i=0; i<fullArrayLen; i++){

        if( (replaceCompleteCount<=replacementOccurence) && (arrFrom[i+replacementOfLen+1]!='\0') &&
            (arrFrom[i]==startBound[0]) && (arrFrom[i+1]==startBound[1]) &&
            (arrFrom[i+replacementOfLen+2]==endBound[0]) && (arrFrom[i+replacementOfLen+1+2]==endBound[1])
        ){

             for(int k=0; k<replaceByLen; k++){arrTo[j+k] = replaceBy[k];}
             i+=replacementOfLen+3;
             j+=replaceByLen-1;
             replaceCompleteCount++;

         }
         else{arrTo[j]=arrFrom[i];}
         j++;
    }

    return arrTo;
}


char* DATA::replaceStr(char* arrTo, char* arrFrom, char startBound[2], char endBound[2], const char* replacementOf, const char* replaceBy){

    // StartBound and endBound Should Contain char array of size 2
    //if(arrTo==nullptr || )
    //free(&arrTo);
    // int replacementOfLen = strlen(replacementOf);
    // int replaceByLen = strlen(replaceBy);
    int replacementOfLen = 0;
    int replaceByLen = 0;
    int fullArrayLen=0;
    int replacementOccurence=0;

    while(replacementOf[replacementOfLen]!='\0'){replacementOfLen++;}
    while(replaceBy[replaceByLen]!='\0'){replaceByLen++;}

    while(arrFrom[fullArrayLen]!='\0'){
        if(arrFrom[fullArrayLen+replacementOfLen+1]!='\0' &&
            (arrFrom[fullArrayLen]==startBound[0]) && (arrFrom[fullArrayLen+1]==startBound[1]) &&
            (arrFrom[fullArrayLen+replacementOfLen+2]==endBound[0]) && (arrFrom[fullArrayLen+replacementOfLen+1+2]==endBound[1])
        ){replacementOccurence++;};
        fullArrayLen++;
    }
    int newArrayLen = (replacementOccurence*replaceByLen)+fullArrayLen-(replacementOccurence*(replacementOfLen+4));
    arrTo = static_cast<char*>(std::malloc(newArrayLen * sizeof(char)));

    int j=0;
    int replaceCompleteCount = 0;

    for(int i=0; i<fullArrayLen; i++){

        if( (replaceCompleteCount<=replacementOccurence) && (arrFrom[i+replacementOfLen+1]!='\0') &&
            (arrFrom[i]==startBound[0]) && (arrFrom[i+1]==startBound[1]) &&
            (arrFrom[i+replacementOfLen+2]==endBound[0]) && (arrFrom[i+replacementOfLen+1+2]==endBound[1])
        ){

             for(int k=0; k<replaceByLen; k++){arrTo[j+k] = replaceBy[k];}
             i+=replacementOfLen+3;
             j+=replaceByLen-1;
             replaceCompleteCount++;

         }
         else{arrTo[j]=arrFrom[i];}
         j++;
    }

    return arrTo;
}


int DATA::getCharFreq(char character, char* arr){
    int count=0;
    for(int i=0;arr[i]!='\0';i++){if(arr[i]==character){count++;}}
    return count;
}

int DATA::maxFlatItemLen(char character, char* arr){
    int cnt=0;
    int maxLen=0;
    for(int i=0;arr[i]!='\0';i++){
        if(arr[i]==character || arr[i+1]=='\0'){if(maxLen<cnt){maxLen=cnt;}cnt=0;}
        else{cnt++;}
    }
    return maxLen;
}

char* DATA::flatKeyValue(char* arrTo, char seperator, const char* keyArr, const char* valueArr){

    /*int i;
    for(i=0; i)*/
    int keylen=0, valuelen=0;// = strlen(arrFrom);
    while(keyArr[keylen]!='\0'){keylen++;}
    while(valueArr[valuelen]!='\0'){valuelen++;}
    /*for(int i=0; i<rlen; i++){
        for(int j=0; arrFrom[i][j]!='\0'; j++){
            clen++;

        }
        clen++;
    }*/
    arrTo = static_cast<char*>(std::malloc(keylen+valuelen+2 * sizeof(char)));
    // arrTo[0] = '\0';
    /*clen=0;
    for(int i=0; i<rlen; i++){
        for(int j=0; arrFrom[i][j]!='\0'; j++){
            arrTo[clen]=arrFrom[i][j];
            clen++;

        }
        arrTo[clen]='\7';
        clen++;
    }*/
    for(int i=0; i<=keylen; i++){arrTo[i] = keyArr[i];}
    arrTo[keylen+1]=seperator;
    for(int i=0; i<=valuelen; i++){arrTo[i+keylen+2] = valueArr[i];}

    return arrTo;
}

char* DATA::getFlatValue(char* arrTo, char seperator, const char* flatKeyValueArr){
    int keylen=0, valuelen=0;
    while(flatKeyValueArr[keylen]!=seperator){keylen++;}
    while(flatKeyValueArr[keylen+valuelen+1]!='\0'){valuelen++;}

    arrTo = static_cast<char*>(std::malloc(valuelen+1 * sizeof(char)));
    for(int i=0;i<=valuelen; i++){arrTo[i]=flatKeyValueArr[keylen+1+i];}
    return arrTo;
}

char* DATA::getFlatKey(char* arrTo, char seperator, const char* flatKeyValueArr){
    int keylen=0;
    while(flatKeyValueArr[keylen]!=seperator){keylen++;}
    arrTo = static_cast<char*>(std::malloc(keylen+1 * sizeof(char)));
    for(int i=0;i<=keylen; i++){arrTo[i]=flatKeyValueArr[i];}
    return arrTo;
}

char* DATA::cpArray(char* arrTo, const char* arrFrom){

    /*int i;
    for(i=0; i)*/
    int len=0;// = strlen(arrFrom);
    while(arrFrom[len]!='\0'){len++;}
    arrTo = static_cast<char*>(std::malloc(len * sizeof(char)));
    // arrTo[0] = '\0';
    for(int i=0; i<len; i++){arrTo[i] = arrFrom[i];}
    return arrTo;
}

char* DATA::fillArray(int c, char* arr){

    arr = static_cast<char*>(std::malloc(c * sizeof(char)));
    arr[0] = '\0';
    return arr;
}

char** DATA::fillArray(int r, int c, char** arr){

    arr = static_cast<char**>(std::malloc(r * sizeof(char*)));
    for (int i = 0; i < c; i++) {
        arr[i] = static_cast<char*>(std::malloc(c * sizeof(char)));
        arr[i][0] = '\0';
    }
    return arr;
}

char*** DATA::fillArray(int r, int rc, int c, char*** arr){

    arr = static_cast<char***>(std::malloc(r * sizeof(char**)));
    for (int i = 0; i < rc; i++) {
        arr[i] = static_cast<char**>(std::malloc(rc * sizeof(char*)));
        for (int j = 0; j < c; j++) {
            arr[i][j] = static_cast<char*>(std::malloc(c * sizeof(char)));
            arr[i][j][0] = '\0';
        }
    }
    return arr;
}

void DATA::freeArray(int r, char** arr){
    for (int i = 0; i < r; i++) {
        free(arr[i]);
    }
    free(arr);
}

void DATA::freeArray(int r, int c, char*** arr){
    for (int i = 0; i < r; i++) {
        //for(int j=0; j<c; j++){
          //  free(arr[i][j]);
        //}
        free(arr[i]);
    }
    free(arr);
}