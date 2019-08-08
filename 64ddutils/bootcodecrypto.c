//Handles bootcode crypto of disk started bootcode.
#include <stdio.h>
#include <string.h>

void decrypt(unsigned char* enc, unsigned char* dec)
{
    int i, t, s = 0;
    for (i = 0; i < 0x100; i++)
    {
        t = (i * 0x11) & 0xFF;
        dec[t] = enc[i];
    }
}

void encrypt(unsigned char* dec, unsigned char* enc)
{
    int i, t, s = 0;
    for (i = 0; i < 0x100; i++)
    {
        t = (i * 0x11) & 0xFF;
        enc[i] = dec[t];
    }
}

int main(int argc, char const *argv[])
{
    FILE *pFileIn;
    FILE *pFileOut;
    unsigned char in[0x100];
    unsigned char out[0x100];
    if (argc < 4)
    {
        printf("usage: bootcodecrypto <cmd> <in> <out>\n<cmd> can be:\n  -d = decrypt\n  -e = encrypt\n");
    }
    else
    {
        if (strncmp(argv[1],"-d",2) != 0 && strncmp(argv[1],"-e",2) != 0)
        {
            printf("Unknown command");
            return 1;
        }

        pFileIn = fopen(argv[2], "rb");
        if (pFileIn == NULL)
            return 1;
        
        fread(in, 1, 0x100, pFileIn);
        fclose(pFileIn);

        if (strncmp(argv[1],"-d",2) == 0)
            decrypt(in, out);
        else
            encrypt(in, out);

        pFileOut = fopen(argv[3], "wb");
        if (pFileOut == NULL)
            return 1;
        
        fwrite(out, 1, 0x100, pFileOut);
        fclose(pFileOut);
    }
    
    return 0;
}
