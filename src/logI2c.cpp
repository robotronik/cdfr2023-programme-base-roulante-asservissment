#include "logI2c.h"

static char logBuffers[2][LOGBUFFERSIZE];
static volatile int writeOffsets[2] = {0, 0};
static volatile int lockedSizes[2] = {0, 0};
static volatile int currentWriteBuffer = 0;
static volatile int readLockedBuffer = -1;

// Fonction principale d'écriture
void logWrite(char* message, int size) {
    if (size <= 0 || message == NULL || size > LOGBUFFERSIZE) return;

    int buf = currentWriteBuffer;
    int offset = writeOffsets[buf];

    if (offset + size > LOGBUFFERSIZE) {
        // L'actuel buffer est plein
        int alt = 1 - buf;
        if (writeOffsets[alt] == 0 && readLockedBuffer != alt) {
            // On switch vers l'autre buffer s'il est libre
            currentWriteBuffer = alt;
            buf = alt;
            offset = 0;
        } else {
            // Aucun buffer dispo pour écrire maintenant
            return;
        }
        memcpy(&logBuffers[buf][offset], message, size);
        writeOffsets[buf] += size;
    }
    else{
        memcpy(&logBuffers[buf][offset], message, size);
        writeOffsets[buf] += size;

        //buffer switch durring memcpy
        if(currentWriteBuffer != buf){
            writeOffsets[buf] = 0;
            memcpy(&logBuffers[currentWriteBuffer][0], message, size);
            writeOffsets[currentWriteBuffer] += size;
        }
    }
}

// Première interruption : lit la taille de la trame complète
int getLogBufferSize(void) {
    int buf = currentWriteBuffer;
    int size = writeOffsets[buf];

    if (size == 0) {
        // Rien à lire
        return 0;
    }

    // Verrouille le buffer pour la lecture
    readLockedBuffer = buf;
    lockedSizes[buf] = size;

    // Change de buffer d'écriture
    currentWriteBuffer = 1 - buf;
    writeOffsets[currentWriteBuffer] = 0;

    return size;
}

// Deuxième interruption : fournit le buffer verrouillé à la lecture
void getLogBuffer(char** outBuffer, int& size) {
    if (readLockedBuffer < 0 || lockedSizes[readLockedBuffer] == 0) {
        return;
    }

    *outBuffer = logBuffers[readLockedBuffer];
    size = lockedSizes[readLockedBuffer];

    // Libération du buffer
    lockedSizes[readLockedBuffer] = 0;
    writeOffsets[readLockedBuffer] = 0;
    readLockedBuffer = -1;
}
