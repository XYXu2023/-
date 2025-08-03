#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MAX_TREE_HT 256
#define BUFFER_SIZE 4096

typedef struct HuffmanNode {
    unsigned char data;
    unsigned freq;
    struct HuffmanNode *left, *right;
} HuffmanNode;

typedef struct MinHeap {
    unsigned size;
    unsigned capacity;
    HuffmanNode** array;
} MinHeap;

typedef struct HuffmanCode {
    unsigned char code[MAX_TREE_HT];
    int length;
} HuffmanCode;

HuffmanNode* newNode(unsigned char data, unsigned freq) {
    HuffmanNode* node = (HuffmanNode*)malloc(sizeof(HuffmanNode));
    node->left = node->right = NULL;
    node->data = data;
    node->freq = freq;
    return node;
}

MinHeap* createMinHeap(unsigned capacity) {
    MinHeap* minHeap = (MinHeap*)malloc(sizeof(MinHeap));
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->array = (HuffmanNode**)malloc(minHeap->capacity * sizeof(HuffmanNode*));
    return minHeap;
}

void swapNodes(HuffmanNode** a, HuffmanNode** b) {
    HuffmanNode* t = *a;
    *a = *b;
    *b = t;
}

void minHeapify(MinHeap* minHeap, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < minHeap->size && minHeap->array[left]->freq < minHeap->array[smallest]->freq)
        smallest = left;

    if (right < minHeap->size && minHeap->array[right]->freq < minHeap->array[smallest]->freq)
        smallest = right;

    if (smallest != idx) {
        swapNodes(&minHeap->array[smallest], &minHeap->array[idx]);
        minHeapify(minHeap, smallest);
    }
}

int isSizeOne(MinHeap* minHeap) {
    return (minHeap->size == 1);
}

HuffmanNode* extractMin(MinHeap* minHeap) {
    HuffmanNode* temp = minHeap->array[0];
    minHeap->array[0] = minHeap->array[minHeap->size - 1];
    --minHeap->size;
    minHeapify(minHeap, 0);
    return temp;
}

void insertMinHeap(MinHeap* minHeap, HuffmanNode* node) {
    ++minHeap->size;
    int i = minHeap->size - 1;
    while (i && node->freq < minHeap->array[(i - 1) / 2]->freq) {
        minHeap->array[i] = minHeap->array[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    minHeap->array[i] = node;
}

void buildMinHeap(MinHeap* minHeap) {
    int n = minHeap->size - 1;
    for (int i = (n - 1) / 2; i >= 0; --i)
        minHeapify(minHeap, i);
}

MinHeap* createAndBuildMinHeap(unsigned char data[], unsigned freq[], int size) {
    MinHeap* minHeap = createMinHeap(size);
    for (int i = 0; i < size; ++i)
        minHeap->array[i] = newNode(data[i], freq[i]);
    minHeap->size = size;
    buildMinHeap(minHeap);
    return minHeap;
}

HuffmanNode* buildHuffmanTree(unsigned char data[], unsigned freq[], int size) {
    HuffmanNode *left, *right, *top;
    MinHeap* minHeap = createAndBuildMinHeap(data, freq, size);

    while (!isSizeOne(minHeap)) {
        left = extractMin(minHeap);
        right = extractMin(minHeap);

        top = newNode('$', left->freq + right->freq);
        top->left = left;
        top->right = right;
        insertMinHeap(minHeap, top);
    }

    return extractMin(minHeap);
}

int isLeaf(HuffmanNode* root) {
    return !(root->left) && !(root->right);
}

void generateCodes(HuffmanNode* root, HuffmanCode codes[], unsigned char arr[], int top) {
    if (root->left) {
        arr[top] = 0;
        generateCodes(root->left, codes, arr, top + 1);
    }
    if (root->right) {
        arr[top] = 1;
        generateCodes(root->right, codes, arr, top + 1);
    }
    if (isLeaf(root)) {
        for (int i = 0; i < top; ++i) {
            codes[root->data].code[i] = arr[i];
        }
        codes[root->data].length = top;
    }
}

void freeHuffmanTree(HuffmanNode* root) {
    if (root == NULL) return;
    freeHuffmanTree(root->left);
    freeHuffmanTree(root->right);
    free(root);
}

void countFrequencies(FILE* file, unsigned freq[]) {
    unsigned char buffer[BUFFER_SIZE];
    size_t bytesRead;
    
    rewind(file);
    while ((bytesRead = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        for (size_t i = 0; i < bytesRead; ++i) {
            freq[buffer[i]]++;
        }
    }
}

void buildHuffmanCodes(FILE* inputFile, HuffmanCode codes[], HuffmanNode** treeRoot) {
    unsigned freq[256] = {0};
    countFrequencies(inputFile, freq);

    int size = 0;
    for (int i = 0; i < 256; ++i) {
        if (freq[i] > 0) size++;
    }

    unsigned char data[size];
    unsigned frequencies[size];
    int index = 0;
    for (int i = 0; i < 256; ++i) {
        if (freq[i] > 0) {
            data[index] = (unsigned char)i;
            frequencies[index] = freq[i];
            index++;
        }
    }

    *treeRoot = buildHuffmanTree(data, frequencies, size);

    unsigned char arr[MAX_TREE_HT];
    generateCodes(*treeRoot, codes, arr, 0);
}

void writeTreeToFile(HuffmanNode* root, FILE* out) {
    if (root == NULL) return;
    
    if (isLeaf(root)) {
        fputc('1', out);
        fputc(root->data, out);
    } else {
        fputc('0', out);
        writeTreeToFile(root->left, out);
        writeTreeToFile(root->right, out);
    }
}

HuffmanNode* readTreeFromFile(FILE* in) {
    int bit = fgetc(in);
    if (bit == '1') {
        unsigned char data = fgetc(in);
        return newNode(data, 0);
    } else {
        HuffmanNode* left = readTreeFromFile(in);
        HuffmanNode* right = readTreeFromFile(in);
        HuffmanNode* node = newNode('$', 0);
        node->left = left;
        node->right = right;
        return node;
    }
}

void compressFile(const char* inputFileName, const char* outputFileName) {
    FILE* inputFile = fopen(inputFileName, "rb");
    if (!inputFile) {
        perror("Unable to open input file");
        exit(EXIT_FAILURE);
    }

    FILE* outputFile = fopen(outputFileName, "wb");
    if (!outputFile) {
        perror("Unable to create output file");
        fclose(inputFile);
        exit(EXIT_FAILURE);
    }

    HuffmanCode codes[256] = {0};
    HuffmanNode* huffmanTree = NULL;
    buildHuffmanCodes(inputFile, codes, &huffmanTree);

    writeTreeToFile(huffmanTree, outputFile);
    fputc('\0', outputFile);

    unsigned char buffer[BUFFER_SIZE];
    unsigned char byte = 0;
    int bitCount = 0;
    size_t bytesRead;

    rewind(inputFile);
    while ((bytesRead = fread(buffer, 1, BUFFER_SIZE, inputFile)) > 0) {
        for (size_t i = 0; i < bytesRead; ++i) {
            unsigned char c = buffer[i];
            for (int j = 0; j < codes[c].length; ++j) {
                byte = (byte << 1) | codes[c].code[j];
                bitCount++;
                if (bitCount == 8) {
                    fputc(byte, outputFile);
                    byte = 0;
                    bitCount = 0;
                }
            }
        }
    }

    if (bitCount > 0) {
        byte <<= (8 - bitCount);
        fputc(byte, outputFile);
    }

    fclose(inputFile);
    fclose(outputFile);
    freeHuffmanTree(huffmanTree);
    printf("SUCCESS: %s -> %s\n", inputFileName, outputFileName);
}

void decompressFile(const char* inputFileName, const char* outputFileName) {
    FILE* inputFile = fopen(inputFileName, "rb");
    if (!inputFile) {
        perror("Unable to open the file");
        exit(EXIT_FAILURE);
    }

    FILE* outputFile = fopen(outputFileName, "wb");
    if (!outputFile) {
        perror("Unable to create output file");
        fclose(inputFile);
        exit(EXIT_FAILURE);
    }

    HuffmanNode* huffmanTree = readTreeFromFile(inputFile);
    fgetc(inputFile); 

    HuffmanNode* currentNode = huffmanTree;
    int bitPos = 7;
    unsigned char byte;
    int inputChar;

    while (inputChar = fgetc(inputFile)) {
        if (inputChar == EOF) break;
        
        byte = (unsigned char)inputChar;
        bitPos = 7;
        
        while (bitPos >= 0) {
            int bit = (byte >> bitPos) & 1;
            bitPos--;
            
            if (bit) {
                currentNode = currentNode->right;
            } else {
                currentNode = currentNode->left;
            }
            
            if (isLeaf(currentNode)) {
                fputc(currentNode->data, outputFile);
                currentNode = huffmanTree;
            }
        }
    }

    fclose(inputFile);
    fclose(outputFile);
    freeHuffmanTree(huffmanTree);
    printf("File decompressed successfully: %s -> %s\n", inputFileName, outputFileName);
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf("Usage:\n");
        printf("  Compress: %s -c inputfile outputfile\n", argv[0]);
        printf("  Decompress: %s -d inputfile outputfile\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (strcmp(argv[1], "-c") == 0) {
        compressFile(argv[2], argv[3]);
    } else if (strcmp(argv[1], "-d") == 0) {
        decompressFile(argv[2], argv[3]);
    } else {
        printf("Invalid option. Use -c to compress or -d to decompress.\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
